/*
Copyright (c) 2002,2003,2004 by Tarn Adams                                            //
                                                                                      //
This file is part of Liberal Crime Squad.                                             //
                                                                                    //
    Liberal Crime Squad is free software; you can redistribute it and/or modify     //
    it under the terms of the GNU General Public License as published by            //
    the Free Software Foundation; either version 2 of the License, or               //
    (at your option) any later version.                                             //
                                                                                    //
    Liberal Crime Squad is distributed in the hope that it will be useful,          //
    but WITHOUT ANY WARRANTY; without even the implied warranty of                  //
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the                  //
    GNU General Public License for more details.                                    //
                                                                                    //
    You should have received a copy of the GNU General Public License               //
    along with Liberal Crime Squad; if not, write to the Free Software              //
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   02111-1307   USA     //
*/

#include <includes.h>

#include "common/ledger.h"

#include "basemode/activate.h"
// for recruitFindDifficulty and recruitName

#include "common/consolesupport.h"
// for void set_color(short,short,bool)

#include "log/log.h"
// for commondisplay.h
#include "common/commondisplay.h"
// for void printcreatureinfo(Creature *,unsigned char=255)

#include "common/getnames.h"
// for getview

#include "common/commonactions.h"
// for subordinatesleft

#include "combat/fight.h"
//for void delenc(short e,char loot);

char talk(Creature &a, const int t);


#include <cursesAlternative.h>
#include <customMaps.h>
#include <constant_strings.h>
#include <gui_constants.h>
#include <set_color_support.h>
extern vector<Creature *> pool;
extern Log gamelog;
extern vector<Location *> location;
extern MusicClass music;
extern int stat_recruits;
extern short cursite;
extern short attitude[VIEWNUM];
extern Creature encounter[ENCMAX];
extern string commaSpace;
extern class Ledger ledger;
typedef map<short, string> shortAndString;
/* recruit struct constructor */
recruitst::recruitst() : timeleft(0), level(0), eagerness1(0), task(0)
{
	//Has heard of the LCS
	if ((int)LCSrandom(100) < attitude[VIEW_LIBERALCRIMESQUAD])
	{
		//Likes the LCS
		if ((int)LCSrandom(100) < attitude[VIEW_LIBERALCRIMESQUADPOS])
			eagerness1 = 3;
		//Doesn't like the LCS
		else eagerness1 = 0;
	}
	else eagerness1 = 2;
}
recruitst::~recruitst()
{
	delete recruit;
}
char recruitst::eagerness()
{
	char eagerness_temp = eagerness1;
	//Moderates are decidedly less interested
	if (recruit->align == 0) eagerness_temp -= 2;
	//Conservatives are extremely uninterested
	if (recruit->align == -1) eagerness_temp -= 4;
	return eagerness_temp;
}
 shortAndString issueEventString;
static void getissueeventstring(char* str)
{
	strcat(str, issueEventString[LCSrandom(VIEWNUM - 3)].data());
}
extern string singleDot;
/* recruiting */
char recruitment_activity(Creature &cr, char &clearformess)
{
	clearformess = 1;
	int ocursite = cursite;
	cursite = cr.location;
	int type = cr.activity.arg;
	int difficulty = recruitFindDifficulty(type);
	char* name = recruitName(type);
	if (type >= 0)
	{
		music.play(MUSIC_RECRUITING);
		cr.train(SKILL_STREETSENSE, 5);
		eraseAlt();
		set_color_easy(WHITE_ON_BLACK_BRIGHT);
		mvaddstrAlt(0,  0, "Adventures in Liberal Recruitment");
		printcreatureinfo(&cr);
		makedelimiter();
		set_color_easy(WHITE_ON_BLACK);
		mvaddstr_f(10, 0, "%s asks around for a %s...", cr.name, name);
		getkey();
		int recruitCount = 0;
		if (difficulty < 10)
			// Generate recruitment candidates
			for (recruitCount = 0; recruitCount<5; recruitCount++)
			{
				if (recruitCount == 0 || cr.skill_roll(SKILL_STREETSENSE)>(difficulty + recruitCount * 2))
				{
					makecreature(encounter[recruitCount], type);
					encounter[recruitCount].namecreature();
				}
				else break;
			}
		if (recruitCount == 0) {
			mvaddstr_f(11, 0, "%s was unable to track down a %s.", cr.name, name);
			getkey();
			cursite = ocursite;
			return 0;
		}
		else if (recruitCount == 1) {
			mvaddstr_f(11, 0, "%s managed to set up a meeting with ", cr.name);
			set_alignment_color(encounter[0].align);
			addstrAlt(encounter[0].name);
			add_age(encounter[0]);
			set_color_easy(WHITE_ON_BLACK);
			addstrAlt(singleDot);
			getkey();
			eraseAlt();
			set_color_easy(WHITE_ON_BLACK_BRIGHT);
			mvaddstrAlt(0,  0, "Adventures in Liberal Recruitment");
			printcreatureinfo(&encounter[0]);
			makedelimiter();
			talk(cr, 0);
		}
		else {
			while (true)
			{
				eraseAlt();
				set_color_easy(WHITE_ON_BLACK_BRIGHT);
				mvaddstrAlt(0,  0, "Adventures in Liberal Recruitment");
				printcreatureinfo(&cr);
				makedelimiter();
				set_color_easy(WHITE_ON_BLACK);
				mvaddstr_f(10, 0, "%s was able to get information on multiple people.", cr.name);
				for (int i = 0; i < recruitCount; i++) {
					set_color_easy(WHITE_ON_BLACK);
					mvaddstr_f(12 + i, 0, "%c - ", 'a' + i);
					set_alignment_color(encounter[i].align);
					addstrAlt(encounter[i].name);
					add_age(encounter[i]);
				}
				set_color_easy(WHITE_ON_BLACK);
				mvaddstrAlt(12 + recruitCount + 1, 0, "Press enter or escape to call it a day.");
				int c = getkey();
				if (c == ENTER || c == ESC) break;
				c -= 'a';
				if (c >= 0 && c < ENCMAX - 1 && encounter[c].exists)
				{
					int id = encounter[c].id;
					eraseAlt();
					set_color_easy(WHITE_ON_BLACK_BRIGHT);
					mvaddstrAlt(0,  0, "Adventures in Liberal Recruitment");
					printcreatureinfo(&encounter[c]);
					makedelimiter();
					talk(cr, c);
					if (encounter[c].id == id) delenc(c, 0);
					recruitCount--;
					if (recruitCount <= 0) break;
				}
			}
		}
	}
	cursite = ocursite;
	return 1;
}
/* daily - recruit - recruit meeting */
char completerecruitmeeting(recruitst &r, int p, char &clearformess)
{
	music.play(MUSIC_RECRUITING);
	clearformess = 1;
	eraseAlt();
	set_color_easy(WHITE_ON_BLACK_BRIGHT);
	moveAlt(0, 0);
	if (pool[p]->meetings++ > 5 && LCSrandom(pool[p]->meetings - 5))
	{
		addstrAlt(pool[p]->name, gamelog);
		addstrAlt(" accidentally missed the meeting with ", gamelog);
		addstrAlt(r.recruit->name, gamelog);
		mvaddstrAlt(1,  0, "due to multiple booking of recruitment sessions.", gamelog);
		gamelog.newline();
		mvaddstrAlt(3,  0, "Get it together, ", gamelog);
		addstrAlt(pool[p]->name, gamelog);
		addstrAlt("!", gamelog);
		gamelog.nextMessage();
		getkey();
		return 1;
	}
	addstrAlt("Meeting with ", gamelog);
	addstrAlt(r.recruit->name, gamelog);
	addstrAlt(commaSpace, gamelog);
	addstrAlt(r.recruit->get_type_name(), gamelog);
	addstrAlt(commaSpace, gamelog);
	addstrAlt(location[r.recruit->location]->name, gamelog);
	gamelog.newline();
	set_color_easy(WHITE_ON_BLACK);
	printfunds();
	printcreatureinfo(r.recruit);
	makedelimiter();
	mvaddstrAlt(10,  0, r.recruit->name);
	switch (r.eagerness())
	{
	case 1: addstrAlt(" will take a lot of persuading."); break;
	case 2: addstrAlt(" is interested in learning more."); break;
	case 3: addstrAlt(" feels something needs to be done."); break;
	default: if (r.eagerness() >= 4) addstrAlt(" is ready to fight for the Liberal Cause.");
			 else addstrAlt(" kind of regrets agreeing to this."); break;
	}
	mvaddstrAlt(11,  0, "How should ");
	addstrAlt(pool[p]->name);
	addstrAlt(" approach the situation?");
	moveAlt(13, 0);
	if (ledger.get_funds() < 50) set_color_easy(BLACK_ON_BLACK_BRIGHT);
	addstrAlt("A - Spend $50 on props and a book for them to keep afterward.");
	set_color_easy(WHITE_ON_BLACK);
	mvaddstrAlt(14,  0, "B - Just casually chat with them and discuss politics.");
	moveAlt(15, 0);
	if (subordinatesleft(*pool[p]) && r.eagerness() >= 4)
	{
		addstrAlt("C - Offer to let ");
		addstrAlt(r.recruit->name);
		addstrAlt(" join the LCS as a full member.");
	}
	else if (!subordinatesleft(*pool[p]))
	{
		set_color_easy(BLACK_ON_BLACK_BRIGHT);
		addstrAlt("C - ");
		addstrAlt(pool[p]->name);
		addstrAlt(" needs more Juice to recruit.");
		set_color_easy(WHITE_ON_BLACK);
	}
	else
	{
		set_color_easy(BLACK_ON_BLACK_BRIGHT);
		addstrAlt("C - ");
		addstrAlt(r.recruit->name);
		addstrAlt(" isn't ready to join the LCS.");
		set_color_easy(WHITE_ON_BLACK);
	}
	mvaddstrAlt(16,  0, "D - Break off the meetings.");
	int y = 18;
	while (true)
	{
		int c = getkey();
		if (c == 'c' && subordinatesleft(*pool[p]) && r.eagerness() >= 4)
		{
			mvaddstrAlt(y,  0, pool[p]->name, gamelog);
			addstrAlt(" offers to let ", gamelog);
			addstrAlt(r.recruit->name, gamelog);
			addstrAlt(" join the Liberal Crime Squad.", gamelog);
			gamelog.newline();
			getkey();
			set_color_easy(GREEN_ON_BLACK_BRIGHT);
			moveAlt(y += 2, 0);
			addstrAlt(r.recruit->name, gamelog);
			addstrAlt(" accepts, and is eager to get started.", gamelog);
			gamelog.nextMessage();
			liberalize(*r.recruit, false);
			getkey();
			eraseAlt();
			sleeperize_prompt(*r.recruit, *pool[p], 6);
			r.recruit->hireid = pool[p]->id;
			pool[p]->train(SKILL_PERSUASION, 25);
			pool.push_back(r.recruit);
			r.recruit = NULL;
			stat_recruits++;
			return 1;
		}
		if (c == 'b' || (c == 'a' && ledger.get_funds() >= 50))
		{
			if (c == 'a')
				ledger.subtract_funds(50, EXPENSE_RECRUITMENT);
			pool[p]->train(SKILL_PERSUASION,
				max(12 - pool[p]->get_skill(SKILL_PERSUASION), 5));
			pool[p]->train(SKILL_SCIENCE,
				max(r.recruit->get_skill(SKILL_SCIENCE) - pool[p]->get_skill(SKILL_SCIENCE), 0));
			pool[p]->train(SKILL_RELIGION,
				max(r.recruit->get_skill(SKILL_RELIGION) - pool[p]->get_skill(SKILL_RELIGION), 0));
			pool[p]->train(SKILL_LAW,
				max(r.recruit->get_skill(SKILL_LAW) - pool[p]->get_skill(SKILL_LAW), 0));
			pool[p]->train(SKILL_BUSINESS,
				max(r.recruit->get_skill(SKILL_BUSINESS) - pool[p]->get_skill(SKILL_BUSINESS), 0));
			int lib_persuasiveness = pool[p]->get_skill(SKILL_BUSINESS) +
				pool[p]->get_skill(SKILL_SCIENCE) +
				pool[p]->get_skill(SKILL_RELIGION) +
				pool[p]->get_skill(SKILL_LAW) +
				pool[p]->get_attribute(ATTRIBUTE_INTELLIGENCE, true);
			int recruit_reluctance = 5 +
				r.recruit->get_skill(SKILL_BUSINESS) +
				r.recruit->get_skill(SKILL_SCIENCE) +
				r.recruit->get_skill(SKILL_RELIGION) +
				r.recruit->get_skill(SKILL_LAW) +
				r.recruit->get_attribute(ATTRIBUTE_WISDOM, true) +
				r.recruit->get_attribute(ATTRIBUTE_INTELLIGENCE, true);
			if (lib_persuasiveness > recruit_reluctance) recruit_reluctance = 0;
			else recruit_reluctance -= lib_persuasiveness;
			int difficulty = recruit_reluctance;
			char str[75];
			strcpy(str, "");
			if (c == 'a')
			{
				difficulty -= 5;
				mvaddstrAlt(y++,  0, pool[p]->name, gamelog);
				addstrAlt(" shares ", gamelog);
				getissueeventstring(str);
				addstrAlt(str, gamelog);
				addstrAlt(singleDot, gamelog);
				gamelog.newline();
				getkey();
			}
			else
			{
				mvaddstrAlt(y++,  0, pool[p]->name, gamelog);
				addstrAlt(" explains ", gamelog);
				addstrAlt(pool[p]->hisher(), gamelog);
				addstrAlt(" views on ", gamelog);
				addstrAlt(getview(LCSrandom(VIEWNUM - 3), true), gamelog);
				addstrAlt(singleDot, gamelog);
				gamelog.newline();
				getkey();
			}
			// Liberals with juice increase difficulty as if their Wisdom were increased by said juice
			if (r.recruit->juice >= 10)
			{
				if (r.recruit->juice < 50) //Activist
					difficulty += 1;
				else if (r.recruit->juice < 100) //Socialist Threat
					difficulty += static_cast<int>(2 + 0.1*r.recruit->get_attribute(ATTRIBUTE_WISDOM, false));
				else if (r.recruit->juice < 200) //Revolutionary
					difficulty += static_cast<int>(3 + 0.2*r.recruit->get_attribute(ATTRIBUTE_WISDOM, false));
				else if (r.recruit->juice < 500) //Urban Commando
					difficulty += static_cast<int>(4 + 0.3*r.recruit->get_attribute(ATTRIBUTE_WISDOM, false));
				else if (r.recruit->juice < 1000) //Liberal Guardian
					difficulty += static_cast<int>(5 + 0.4*r.recruit->get_attribute(ATTRIBUTE_WISDOM, false));
				else //Elite Liberal
					difficulty += static_cast<int>(6 + 0.5*r.recruit->get_attribute(ATTRIBUTE_WISDOM, false));
			}
			if (difficulty > 18) difficulty = 18; // difficulty above 18 is impossible, we don't want that
			if (pool[p]->skill_check(SKILL_PERSUASION, difficulty))
			{
				set_color_easy(CYAN_ON_BLACK_BRIGHT);
				if (r.level < 127) r.level++;
				if (r.eagerness1 < 127) r.eagerness1++;
				mvaddstrAlt(y++,  0, r.recruit->name, gamelog);
				addstrAlt(" found ", gamelog);
				addstrAlt(pool[p]->name, gamelog);
				addstrAlt("'s views to be insightful.", gamelog);
				gamelog.newline();
				mvaddstrAlt(y++,  0, "They'll definitely meet again tomorrow.", gamelog);
				gamelog.nextMessage();
			}
			else if (pool[p]->skill_check(SKILL_PERSUASION, difficulty)) // Second chance to not fail horribly
			{
				if (r.level < 127) r.level++;
				if (r.eagerness1 > -128) r.eagerness1--;
				mvaddstrAlt(y++,  0, r.recruit->name, gamelog);
				addstrAlt(" is skeptical about some of ", gamelog);
				addstrAlt(pool[p]->name, gamelog);
				addstrAlt("'s arguments.", gamelog);
				gamelog.newline();
				mvaddstrAlt(y++,  0, "They'll meet again tomorrow.", gamelog);
				gamelog.nextMessage();
			}
			else
			{
				set_color_easy(MAGENTA_ON_BLACK_BRIGHT);
				moveAlt(y++, 0);
				if (r.recruit->talkreceptive() && r.recruit->align == ALIGN_LIBERAL)
				{
					addstrAlt(r.recruit->name, gamelog);
					addstrAlt(" isn't convinced ", gamelog);
					addstrAlt(pool[p]->name, gamelog);
					addstrAlt(" really understands the problem.", gamelog);
					gamelog.newline();
					mvaddstrAlt(y++,  0, "Maybe ", gamelog);
					addstrAlt(pool[p]->name, gamelog);
					addstrAlt(" needs more experience.", gamelog);
					gamelog.nextMessage();
				}
				else
				{
					addstrAlt(pool[p]->name, gamelog);
					addstrAlt(" comes off as slightly insane.", gamelog);
					gamelog.newline();
					mvaddstrAlt(y++,  0, "This whole thing was a mistake. There won't be another meeting.", gamelog);
					gamelog.nextMessage();
				}
				getkey();
				return 1;
			}
			getkey();
			return 0;
		}
		if (c == 'd') return 1;
	}
}