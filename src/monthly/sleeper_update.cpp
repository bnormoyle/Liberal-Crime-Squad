/*
This file is part of Liberal Crime Squad.                                           //
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
/* monthly - sleeper behavior */

#include <includes.h>

#include "common/ledger.h"

#include "sitemode/newencounter.h"
// for prepareencounter

#include "items/loottype.h"

#include "items/loot.h"

#include "common/commonactions.h"
// for void change_public_opinion(int,int ,char =1,char =100);

#include "log/log.h"
// for commondisplay.h
#include "common/commondisplay.h"
// for addstr

#include "common/consolesupport.h"
// for int getkey();

#include "common/translateid.h"
// for  int getloottype(int id);

#include "monthly/sleeper_update.h"
//own header


#include <cursesAlternative.h>
#include <customMaps.h>
#include <constant_strings.h>
#include <gui_constants.h>
#include <set_color_support.h>
extern vector<Creature *> pool;
extern Log gamelog;
extern vector<Location *> location;
extern vector<LootType *> loottype;
extern int stat_recruits;
extern string string_sleeper;
 string they_are_stashed;
extern string singleDot;

string hasBeenCaughtSnooping;
string isNowHomeless;
string hasLeakedIntelligence;
string hasLeakedPolice;
string hasLeakedCorporate;
string hasLeakedPrison;
string hasLeakedCableNews;
string hasLeakedAMRadio;
string hasLeakedAnimalResearch;
string hasLeakedJudiciary;
string papersAreStashed;
string hasLeakedCCS;
string diskIsStashed;

string arrestedWhileEmbezzling;
string arrestedWhileStealing;

string droppedOffPackage;
string itemNotFound;
string lostStolenItem;
string contactModAuthor;

string hasRecruited;
string looksForwardToServing;

extern short lawList[LAWNUM];
extern char disbanding;
extern short attitude[VIEWNUM];
extern class Ledger ledger;
extern vector<ArmorType *> armortype;
extern Log xmllog;
extern Creature encounter[ENCMAX];
extern vector<WeaponType *> weapontype;

/**********************************************************************
** *JDS*
** ----- The sleeper system has been completely reworked.
** - Sleepers no longer directly influence the issues. They now affect
** the broad "liberal power" stats across many issues, which are used
** as a kind of monthly liberal roll akin to AM Radio and Cable News.
** - Each sleeper can affect one or more issue, throwing their power
** into the "abstracted debate" on that issue.
** - After all of the sleepers have contributed to the liberal power
** stats, a roll is made on each issue to see whether the liberals
** make background progress on those issues.
** - Several sleepers have special abilities. Lawyers and Judges, as
** always, can aid your people in the legal system. Police officers,
** corporate managers, CEOs, and agents can all now leak secret
** documents of the appropriate types, and they will make a check
** each month. This will only happen if the homeless shelter is not
** under siege, and "canseethings" is enabled (eg, you're not in prison
** or disbanded or some other situation where your sleeper can't get
** in touch with anyone in your squad).
** - News Anchors and Radio Personalities remain the two most powerful
** sleepers.
**********************************************************************/
void sleepereffect(Creature &cr, char &clearformess, char canseethings, int(&libpower)[VIEWNUM])
{
	if (disbanding)cr.activity.type = ACTIVITY_SLEEPER_LIBERAL;
	int infiltrate = 1;
	switch (cr.activity.type)
	{
	case ACTIVITY_SLEEPER_LIBERAL:
		sleeper_influence(cr, clearformess, canseethings, libpower);
		cr.infiltration -= 0.02f;
		break;
	case ACTIVITY_SLEEPER_EMBEZZLE:
		sleeper_embezzle(cr, clearformess, canseethings, libpower);
		break;
	case ACTIVITY_SLEEPER_STEAL:
		sleeper_steal(cr, clearformess, canseethings, libpower);
		infiltrate = 0;
		break;
	case ACTIVITY_SLEEPER_RECRUIT:
		sleeper_recruit(cr, clearformess, canseethings, libpower);
		break;
	case ACTIVITY_SLEEPER_SPY:
		sleeper_spy(cr, clearformess, canseethings, libpower);
		break;
	case ACTIVITY_SLEEPER_SCANDAL:
		sleeper_scandal(cr, clearformess, canseethings, libpower);
		break;
	case ACTIVITY_NONE:
	case ACTIVITY_SLEEPER_JOINLCS:
	default:
		break;
	}
	if (infiltrate) cr.infiltration += LCSrandom(8)*0.01f - 0.02f;
	if (cr.infiltration >= 1)
		cr.infiltration = 1;
	if (cr.infiltration <= 0)
		cr.infiltration = 0;
}
/*********************************
**
**   SLEEPERS INFLUENCING
**     PUBLIC OPINION
**
**********************************/
void sleeper_influence(Creature &cr, char &clearformess, char canseethings, int(&libpower)[VIEWNUM])
{
	int power = (cr.get_attribute(ATTRIBUTE_CHARISMA, true) +
		cr.get_attribute(ATTRIBUTE_HEART, true) +
		cr.get_attribute(ATTRIBUTE_INTELLIGENCE, true) +
		cr.get_skill(SKILL_PERSUASION));
	// Profession specific skills
	switch (cr.type)
	{
	case CREATURE_CRITIC_ART:
		power += cr.get_skill(SKILL_WRITING);
	case CREATURE_PAINTER:
	case CREATURE_SCULPTOR:
		power += cr.get_skill(SKILL_ART);
		break;
	case CREATURE_CRITIC_MUSIC:
		power += cr.get_skill(SKILL_WRITING);
	case CREATURE_MUSICIAN:
		power += cr.get_skill(SKILL_MUSIC);
		break;
	case CREATURE_AUTHOR:
	case CREATURE_JOURNALIST:
		power += cr.get_skill(SKILL_WRITING);
		break;
	case CREATURE_JUDGE_CONSERVATIVE:
		power += cr.get_skill(SKILL_WRITING);
	case CREATURE_LAWYER:
		power += cr.get_skill(SKILL_LAW);
		break;
	case CREATURE_SCIENTIST_LABTECH:
	case CREATURE_SCIENTIST_EMINENT:
		power += cr.get_skill(SKILL_SCIENCE);
		break;
	case CREATURE_CORPORATE_CEO:
	case CREATURE_CORPORATE_MANAGER:
		power += cr.get_skill(SKILL_BUSINESS);
		break;
	case CREATURE_PRIEST:
	case CREATURE_NUN:
		power += cr.get_skill(SKILL_RELIGION);
		break;
	case CREATURE_EDUCATOR:
		power += cr.get_skill(SKILL_PSYCHOLOGY);
		break;
	}
	// Adjust power for super sleepers
	switch (cr.type)
	{
	case CREATURE_CORPORATE_CEO:
	case CREATURE_POLITICIAN:
	case CREATURE_SCIENTIST_EMINENT:
		power *= 20;
		break;
	case CREATURE_DEATHSQUAD:
	case CREATURE_EDUCATOR:
	case CREATURE_MILITARYOFFICER:
		power *= 6;
		break;
	case CREATURE_ACTOR:
	case CREATURE_GANGUNIT:
	case CREATURE_MILITARYPOLICE:
	case CREATURE_SEAL:
		power *= 4;
		break;
	default:
		power *= 2;
		break;
	}
	power = static_cast<int>(power*cr.infiltration);
	switch (cr.type)
	{
		/* Radio Personalities and News Anchors subvert Conservative news stations by
		reducing their audience and twisting views on the issues. As their respective
		media establishments become marginalized, so does their influence. */
	case CREATURE_RADIOPERSONALITY:
		change_public_opinion(VIEW_AMRADIO, 1);
		for (int i = 0; i < VIEWNUM - 3; i++)
		{
			libpower[i] += power*(100 - attitude[VIEW_AMRADIO]) / 100;
		}
		break;
	case CREATURE_NEWSANCHOR:
		change_public_opinion(VIEW_CABLENEWS, 1);
		for (int i = 0; i < VIEWNUM - 3; i++)
		{
			libpower[i] += power*(100 - attitude[VIEW_CABLENEWS]) / 100;
		}
		break;
		/* Cultural leaders block - influences cultural issues */
	case CREATURE_PRIEST:
	case CREATURE_PAINTER:
	case CREATURE_SCULPTOR:
	case CREATURE_AUTHOR:
	case CREATURE_JOURNALIST:
	case CREATURE_PSYCHOLOGIST:
	case CREATURE_MUSICIAN:
	case CREATURE_CRITIC_ART:
	case CREATURE_CRITIC_MUSIC:
	case CREATURE_ACTOR:
		libpower[VIEW_WOMEN] += power;
		libpower[VIEW_CIVILRIGHTS] += power;
		libpower[VIEW_GAY] += power;
		libpower[VIEW_FREESPEECH] += power;
		libpower[VIEW_DRUGS] += power;
		libpower[VIEW_IMMIGRATION] += power;
		break;
		/* Legal block - influences an array of social issues */
	case CREATURE_JUDGE_CONSERVATIVE:
		libpower[VIEW_JUSTICES] += power;
		libpower[VIEW_FREESPEECH] += power;
		libpower[VIEW_INTELLIGENCE] += power;
	case CREATURE_LAWYER:
		libpower[VIEW_POLICEBEHAVIOR] += power;
		libpower[VIEW_DEATHPENALTY] += power;
		libpower[VIEW_GUNCONTROL] += power;
		libpower[VIEW_DRUGS] += power;
		break;
		/* Scientists block */
	case CREATURE_SCIENTIST_EMINENT:
		libpower[VIEW_POLLUTION] += power;
	case CREATURE_SCIENTIST_LABTECH:
		libpower[VIEW_NUCLEARPOWER] += power;
		libpower[VIEW_ANIMALRESEARCH] += power;
		libpower[VIEW_GENETICS] += power;
		break;
		/* Corporate block */
	case CREATURE_CORPORATE_CEO:
		libpower[VIEW_CEOSALARY] += power;
	case CREATURE_CORPORATE_MANAGER:
		libpower[VIEW_WOMEN] += power;
		libpower[VIEW_TAXES] += power;
		libpower[VIEW_CORPORATECULTURE] += power;
		libpower[VIEW_SWEATSHOPS] += power;
		libpower[VIEW_POLLUTION] += power;
		libpower[VIEW_CIVILRIGHTS] += power;
		break;
		/* Law enforcement block */
	case CREATURE_DEATHSQUAD:
		libpower[VIEW_PRISONS] += power;
		libpower[VIEW_DEATHPENALTY] += power;
	case CREATURE_SWAT:
	case CREATURE_COP:
	case CREATURE_GANGUNIT:
		libpower[VIEW_POLICEBEHAVIOR] += power;
		libpower[VIEW_DRUGS] += power;
		libpower[VIEW_TORTURE] += power;
		libpower[VIEW_GUNCONTROL] += power;
		libpower[VIEW_PRISONS] += power;
		break;
		/* Prison block */
	case CREATURE_EDUCATOR:
	case CREATURE_PRISONGUARD:
	case CREATURE_PRISONER:
		libpower[VIEW_POLICEBEHAVIOR] += power;
		libpower[VIEW_DEATHPENALTY] += power;
		libpower[VIEW_DRUGS] += power;
		libpower[VIEW_TORTURE] += power;
		libpower[VIEW_PRISONS] += power;
		break;
		/* Intelligence block */
	case CREATURE_SECRET_SERVICE:
		libpower[VIEW_INTELLIGENCE] += power;
		break;
	case CREATURE_AGENT:
		libpower[VIEW_INTELLIGENCE] += power;
		libpower[VIEW_TORTURE] += power;
		libpower[VIEW_PRISONS] += power;
		libpower[VIEW_FREESPEECH] += power;
		break;
		/* Military block */
	case CREATURE_MERC:
		libpower[VIEW_GUNCONTROL] += power;
		break;
	case CREATURE_SOLDIER:
	case CREATURE_VETERAN:
	case CREATURE_MILITARYPOLICE:
	case CREATURE_MILITARYOFFICER:
	case CREATURE_SEAL:
		libpower[VIEW_MILITARY] += power;
		libpower[VIEW_TORTURE] += power;
		libpower[VIEW_GAY] += power;
		libpower[VIEW_WOMEN] += power;
		break;
		/* Sweatshop workers */
	case CREATURE_WORKER_SWEATSHOP:
		libpower[VIEW_IMMIGRATION] += power;
		libpower[VIEW_SWEATSHOPS] += power;
		break;
		/* No influence at all block - for people were liberal anyway, or have no way of doing any good */
	case CREATURE_WORKER_FACTORY_CHILD:
	case CREATURE_GENETIC:
	case CREATURE_GUARDDOG:
	case CREATURE_BUM:
	case CREATURE_CRACKHEAD:
	case CREATURE_TANK:
	case CREATURE_HIPPIE: // too liberal to be a proper sleeper
	case CREATURE_WORKER_FACTORY_UNION: // same
	case CREATURE_JUDGE_LIBERAL: // more again
	case CREATURE_MUTANT:
		return;
		/* Miscellaneous block -- includes everyone else */
	case CREATURE_POLITICIAN:
	{
		int a = LCSrandom(VIEWNUM - 5);
		int b = LCSrandom(VIEWNUM - 5);
		while (b == a)b = LCSrandom(VIEWNUM - 5);
		int c = LCSrandom(VIEWNUM - 5);
		while (c == a || c == b)c = LCSrandom(VIEWNUM - 5);
		libpower[a] += power;
		libpower[b] += power;
		libpower[c] += power;
	}
	break;
	case CREATURE_FIREFIGHTER:
		if (lawList[LAW_FREESPEECH] == -2)
		{
			libpower[VIEW_FREESPEECH] += power;
			break;
		}
	default: // Affect a random issue
		pickrandom(libpower) += power;
	}
}
/*********************************
**
**   SLEEPERS SNOOPING AROUND
**
**********************************/
void sleeper_spy(Creature &cr, char &clearformess, char canseethings, int(&libpower)[VIEWNUM])
{
	int homes = find_site_index_in_same_city(SITE_RESIDENTIAL_SHELTER, cr.location);
	if (LCSrandom(100) > 100 * cr.infiltration)
	{
		cr.juice -= 1;
		if (cr.juice < -2)
		{
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(hasBeenCaughtSnooping, gamelog);
			gamelog.newline();
			mvaddstrAlt(8,  1, isNowHomeless, gamelog);
			gamelog.nextMessage();
			getkey();
			removesquadinfo(cr);
			cr.location = homes;
			cr.base = homes;
			cr.drop_weapons_and_clips(NULL);
			cr.activity.type = ACTIVITY_NONE;
			cr.flag &= ~CREATUREFLAG_SLEEPER;
		}
		return;
	}
	// Improves juice, as confidence improves
	if (cr.juice < 100)
	{
		cr.juice += 10;
		if (cr.juice > 100) cr.juice = 100;
	}
	location[cr.base]->mapped = 1;
	bool pause = false;
	switch (cr.type)
	{
	case CREATURE_SECRET_SERVICE:
	case CREATURE_AGENT:
	case CREATURE_POLITICIAN:
		// Agents can leak intelligence files to you
		if (!location[homes]->siege.siege&&canseethings)
		{
			if (LCSrandom(lawList[LAW_PRIVACY] + 3)) break;
			Item *it = new Loot(*loottype[getloottype(tag_LOOT_SECRETDOCUMENTS)]);
			location[homes]->loot.push_back(it);
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(hasLeakedIntelligence, gamelog);
			gamelog.newline();
			mvaddstrAlt(7,  1, they_are_stashed, gamelog);
			gamelog.nextMessage();
			pause = true;
		}
		break;
	case CREATURE_DEATHSQUAD:
	case CREATURE_SWAT:
	case CREATURE_COP:
	case CREATURE_GANGUNIT:
		// Cops can leak police files to you
		if (!location[homes]->siege.siege&&canseethings)
		{
			if (LCSrandom(lawList[LAW_POLICEBEHAVIOR] + 3)) break;
			Item *it = new Loot(*loottype[getloottype(tag_LOOT_POLICERECORDS)]);
			location[homes]->loot.push_back(it);
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(hasLeakedPolice, gamelog);
			gamelog.newline();
			mvaddstrAlt(7,  1, they_are_stashed, gamelog);
			gamelog.nextMessage();
			pause = true;
		}
		break;
	case CREATURE_CORPORATE_MANAGER:
	case CREATURE_CORPORATE_CEO:
		// Can leak corporate files to you
		if (!location[homes]->siege.siege&&canseethings)
		{
			if (LCSrandom(lawList[LAW_CORPORATE] + 3) && cr.type != CREATURE_CORPORATE_CEO) break;
			Item *it = new Loot(*loottype[getloottype(tag_LOOT_CORPFILES)]);
			location[homes]->loot.push_back(it);
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(hasLeakedCorporate, gamelog);
			gamelog.newline();
			mvaddstrAlt(7,  1, they_are_stashed, gamelog);
			gamelog.nextMessage();
			pause = true;
		}
		break;
	case CREATURE_EDUCATOR:
	case CREATURE_PRISONGUARD:
		if (!location[homes]->siege.siege&&canseethings)
		{
			if (LCSrandom(lawList[LAW_POLICEBEHAVIOR] + 3)) break;
			Item *it = new Loot(*loottype[getloottype(tag_LOOT_PRISONFILES)]);
			location[homes]->loot.push_back(it);
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(hasLeakedPrison, gamelog);
			gamelog.newline();
			mvaddstrAlt(7,  1, they_are_stashed, gamelog);
			gamelog.nextMessage();
			pause = true;
		}
		break;
	case CREATURE_NEWSANCHOR:
		if (!location[homes]->siege.siege&&canseethings)
		{
			// More likely to leak these documents the more restrictive
			// free speech is -- because the more free the society, the
			// less any particular action the media takes seems scandalous
			if (LCSrandom(lawList[LAW_FREESPEECH] + 3)) break;
			Item *it = new Loot(*loottype[getloottype(tag_LOOT_CABLENEWSFILES)]);
			location[homes]->loot.push_back(it);
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(hasLeakedCableNews, gamelog);
			gamelog.newline();
			mvaddstrAlt(7,  1, papersAreStashed, gamelog);
			gamelog.nextMessage();
			pause = true;
		}
		break;
	case CREATURE_RADIOPERSONALITY:
		if (!location[homes]->siege.siege&&canseethings)
		{
			// More likely to leak these documents the more restrictive
			// free speech is -- because the more free the society, the
			// less any particular action the media takes seems scandalous
			if (LCSrandom(lawList[LAW_FREESPEECH] + 3)) break;
			Item *it = new Loot(*loottype[getloottype(tag_LOOT_AMRADIOFILES)]);
			location[homes]->loot.push_back(it);
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(hasLeakedAMRadio, gamelog);
			gamelog.newline();
			mvaddstrAlt(7,  1, papersAreStashed, gamelog);
			gamelog.nextMessage();
			pause = true;
		}
		break;
	case CREATURE_SCIENTIST_LABTECH:
	case CREATURE_SCIENTIST_EMINENT:
		if (!location[homes]->siege.siege&&canseethings)
		{
			if (LCSrandom(lawList[LAW_ANIMALRESEARCH] + 3)) break;
			Item *it = new Loot(*loottype[getloottype(tag_LOOT_RESEARCHFILES)]);
			location[homes]->loot.push_back(it);
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(hasLeakedAnimalResearch, gamelog);
			gamelog.newline();
			mvaddstrAlt(7,  1, they_are_stashed, gamelog);
			gamelog.nextMessage();
			pause = true;
		}
		break;
	case CREATURE_JUDGE_CONSERVATIVE:
		if (!location[homes]->siege.siege&&canseethings)
		{
			if (LCSrandom(5)) break;
			Item *it = new Loot(*loottype[getloottype(tag_LOOT_JUDGEFILES)]);
			location[homes]->loot.push_back(it);
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(hasLeakedJudiciary, gamelog);
			gamelog.newline();
			mvaddstrAlt(7,  1, papersAreStashed, gamelog);
			gamelog.nextMessage();
			pause = true;
		}
		break;
	case CREATURE_CCS_ARCHCONSERVATIVE:
		if (!location[homes]->siege.siege&&canseethings)
		{
			if (ccsexposure >= CCSEXPOSURE_LCSGOTDATA) break;
			Item *it = new Loot(*loottype[getloottype(tag_LOOT_CCS_BACKERLIST)]);
			location[homes]->loot.push_back(it);
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(hasLeakedCCS, gamelog);
			gamelog.newline();
			mvaddstrAlt(7,  1, diskIsStashed, gamelog);
			gamelog.nextMessage();
			pause = true;
			ccsexposure = CCSEXPOSURE_LCSGOTDATA;
		}
		break;
	}
	if (pause) getkey();
}
/*********************************
**
**   SLEEPERS EMBEZZLING FUNDS
**
**********************************/
void sleeper_embezzle(Creature &cr, char &clearformess, char canseethings, int(&libpower)[VIEWNUM])
{
	if (LCSrandom(100) > 100 * cr.infiltration)
	{
		cr.juice -= 1;
		if (cr.juice < -2)
		{
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(arrestedWhileEmbezzling, gamelog);
			gamelog.nextMessage();
			getkey();
			cr.crimes_suspected[LAWFLAG_COMMERCE]++;
			removesquadinfo(cr);
			cr.location = find_site_index_in_same_city(SITE_GOVERNMENT_POLICESTATION, cr.location);
			cr.drop_weapons_and_clips(NULL);
			cr.activity.type = ACTIVITY_NONE;
			cr.flag &= ~CREATUREFLAG_SLEEPER;
		}
		return;
	}
	// Improves juice, as confidence improves
	if (cr.juice < 100)
	{
		cr.juice += 10;
		if (cr.juice > 100) cr.juice = 100;
	}
	int income;
	switch (cr.type)
	{
	case CREATURE_CORPORATE_CEO:
		income = static_cast<int>(50000 * cr.infiltration);
		break;
	case CREATURE_SCIENTIST_EMINENT:
	case CREATURE_CORPORATE_MANAGER:
	case CREATURE_BANK_MANAGER:
	case CREATURE_POLITICIAN:
		income = static_cast<int>(5000 * cr.infiltration);
		break;
	default:
		income = static_cast<int>(500 * cr.infiltration);
		break;
	}
	ledger.add_funds(income, INCOME_EMBEZZLEMENT);
}
struct stringAndInt
{
	string str;
	int integer;
	stringAndInt(string str_, int integer_) : str(str_), integer(integer_) {}
	stringAndInt(int integer_, string str_) : str(str_), integer(integer_) {}
};
string randomString(vector<stringAndInt>);
string randomString(vector<stringAndInt> outputList) {
	for (stringAndInt s : outputList) {
		if (!LCSrandom(s.integer)) {
			return s.str;
		}
	}
	return outputList[len(outputList) - 1].str;
}
/*********************************
**
**   SLEEPERS STEALING THINGS
**
**********************************/
void sleeper_steal(Creature &cr, char &clearformess, char canseethings, int(&libpower)[VIEWNUM])
{
	if (LCSrandom(100) > 100 * cr.infiltration)
	{
		cr.juice -= 1;
		if (cr.juice < -2)
		{
			eraseAlt();
			mvaddstrAlt(6,  1, string_sleeper, gamelog);
			addstrAlt(cr.name, gamelog);
			addstrAlt(arrestedWhileStealing, gamelog);
			gamelog.nextMessage();
			getkey();
			cr.crimes_suspected[LAWFLAG_THEFT]++;
			removesquadinfo(cr);
			cr.location = find_site_index_in_same_city(SITE_GOVERNMENT_POLICESTATION, cr.location);
			cr.drop_weapons_and_clips(NULL);
			cr.activity.type = ACTIVITY_NONE;
			cr.flag &= ~CREATUREFLAG_SLEEPER;
		}
		return;
	}
	// Improves juice, as confidence improves
	if (cr.juice < 100)
	{
		cr.juice += 10;
		if (cr.juice > 100)cr.juice = 100;
	}
	cr.infiltration -= LCSrandom(10)*0.01f - 0.02f; //No effectiveness drop before? -Niel
													//Item *item;
	string item;
	Location *shelter = location[find_site_index_in_same_city(SITE_RESIDENTIAL_SHELTER, cr.location)];
	int number_of_items = LCSrandom(10) + 1;
	int itemindex = -1; // have to check case item not found to avoid brave modders segfaults.
	int numberofxmlfails = 0; // Tell them how many fails
	while (number_of_items--)
	{
		bool loot = false;
		bool armor = false;
		bool weapon = false;
		switch (location[cr.location]->type) //Temporary (transitionally) solution until sites are done. -XML
		{
		case SITE_RESIDENTIAL_TENEMENT:
			item = randomString({ stringAndInt(3, tag_LOOT_KIDART),
				stringAndInt(2, tag_LOOT_DIRTYSOCK),
				stringAndInt(1, tag_LOOT_FAMILYPHOTO) });
			loot = true;
			break;
		case SITE_RESIDENTIAL_APARTMENT:
			item = randomString({ stringAndInt(5, tag_LOOT_CELLPHONE),
				stringAndInt(4, tag_LOOT_SILVERWARE),
				stringAndInt(3, tag_LOOT_TRINKET),
				stringAndInt(2, tag_LOOT_CHEAPJEWELERY),
				stringAndInt(1,  tag_LOOT_COMPUTER) });
			loot = true;
			break;
		case SITE_RESIDENTIAL_APARTMENT_UPSCALE:
			item = randomString({ stringAndInt(10, tag_LOOT_EXPENSIVEJEWELERY),
				stringAndInt(5, tag_LOOT_CELLPHONE),
				stringAndInt(4, tag_LOOT_SILVERWARE),
				stringAndInt(3, tag_LOOT_PDA),
				stringAndInt(2, tag_LOOT_CHEAPJEWELERY),
				stringAndInt(1,  tag_LOOT_COMPUTER) });
			loot = true;
			break;
		case SITE_LABORATORY_COSMETICS:
		case SITE_INDUSTRY_NUCLEAR:
		case SITE_LABORATORY_GENETIC:
			item = randomString({ stringAndInt(5, tag_LOOT_RESEARCHFILES),
				stringAndInt(2, tag_LOOT_LABEQUIPMENT),
				stringAndInt(2, tag_LOOT_COMPUTER),
				stringAndInt(5, tag_LOOT_PDA),
				stringAndInt(5, tag_LOOT_CHEMICAL),
				stringAndInt(1,  tag_LOOT_COMPUTER) });
			loot = true;
			break;
		case SITE_GOVERNMENT_COURTHOUSE:
			item = randomString({ stringAndInt(5, tag_LOOT_JUDGEFILES),
				stringAndInt(3, tag_LOOT_CELLPHONE),
				stringAndInt(2, tag_LOOT_PDA),
				stringAndInt(1,  tag_LOOT_COMPUTER) });
			loot = true;
			break;
		case SITE_GOVERNMENT_PRISON:
			item = randomString({ stringAndInt(1, tag_WEAPON_SHANK) });
			weapon = true;
			break;
		case SITE_BUSINESS_BANK:
		case SITE_GOVERNMENT_FIRESTATION:
			item = randomString({ stringAndInt(2, tag_LOOT_TRINKET),
				stringAndInt(1,  tag_LOOT_COMPUTER) });
			loot = true;
			break;
		case SITE_INDUSTRY_SWEATSHOP:
			item = randomString({ stringAndInt(1, tag_LOOT_FINECLOTH) });
			loot = true;
			break;
		case SITE_INDUSTRY_POLLUTER:
			item = randomString({ stringAndInt(1, tag_LOOT_CHEMICAL) });
			loot = true;
			break;
		case SITE_CORPORATE_HEADQUARTERS:
			item = randomString({ stringAndInt(5, tag_LOOT_CORPFILES),
				stringAndInt(3, tag_LOOT_CELLPHONE),
				stringAndInt(2, tag_LOOT_PDA),
				stringAndInt(1,  tag_LOOT_COMPUTER) });
			loot = true;
			break;
		case SITE_CORPORATE_HOUSE:
			item = randomString({ stringAndInt(8, tag_LOOT_TRINKET),
				stringAndInt(7, tag_LOOT_WATCH),
				stringAndInt(6, tag_LOOT_PDA),
				stringAndInt(5, tag_LOOT_CELLPHONE),
				stringAndInt(4, tag_LOOT_SILVERWARE),
				stringAndInt(3, tag_LOOT_CHEAPJEWELERY),
				stringAndInt(2, tag_LOOT_FAMILYPHOTO),
				stringAndInt(1,  tag_LOOT_COMPUTER) });
			loot = true;
			break;
		case SITE_MEDIA_AMRADIO:
			item = randomString({ stringAndInt(5, tag_LOOT_AMRADIOFILES),
				stringAndInt(4, tag_LOOT_MICROPHONE),
				stringAndInt(3, tag_LOOT_PDA),
				stringAndInt(2, tag_LOOT_CELLPHONE),
				stringAndInt(1,  tag_LOOT_COMPUTER) });
			loot = true;
			break;
		case SITE_MEDIA_CABLENEWS:
			item = randomString({ stringAndInt(5, tag_LOOT_CABLENEWSFILES),
				stringAndInt(4, tag_LOOT_MICROPHONE),
				stringAndInt(3, tag_LOOT_PDA),
				stringAndInt(2, tag_LOOT_CELLPHONE),
				stringAndInt(1,  tag_LOOT_COMPUTER) });
			loot = true;
			break;
		case SITE_GOVERNMENT_POLICESTATION:
			if (!LCSrandom(3))
			{
				item = randomString({ stringAndInt(4, tag_WEAPON_SMG_MP5),
					stringAndInt(3, tag_WEAPON_SEMIPISTOL_45),
					stringAndInt(2, tag_WEAPON_SHOTGUN_PUMP),
					stringAndInt(1,  tag_WEAPON_SEMIRIFLE_AR15) });
				weapon = true;
			}
			else if (!LCSrandom(2))
			{
				if (lawList[LAW_POLICEBEHAVIOR] == -2 && lawList[LAW_DEATHPENALTY] == -2 && !LCSrandom(4))item = tag_ARMOR_DEATHSQUADUNIFORM;
				else item = randomString({ stringAndInt(3, tag_ARMOR_POLICEUNIFORM),
					stringAndInt(2, tag_ARMOR_SWATARMOR),
					stringAndInt(1,  tag_ARMOR_POLICEARMOR) });
				armor = true;
			}
			else
			{
				item = randomString({ stringAndInt(5, tag_LOOT_POLICERECORDS),
					stringAndInt(3, tag_LOOT_CELLPHONE),
					stringAndInt(2, tag_LOOT_PDA),
					stringAndInt(1,  tag_LOOT_COMPUTER) });
				loot = true;
			}
			break;
		case SITE_GOVERNMENT_ARMYBASE:
			if (!LCSrandom(3))
			{
				item = randomString({
					stringAndInt(3,  tag_WEAPON_CARBINE_M4),
					stringAndInt(1, tag_WEAPON_AUTORIFLE_M16) });
				weapon = true;
			}
			else if (!LCSrandom(2))
			{
				item = randomString({ stringAndInt(1, tag_ARMOR_ARMYARMOR) });
				armor = true;
			}
			else
			{
				item = randomString({ stringAndInt(5, tag_LOOT_SECRETDOCUMENTS),
					stringAndInt(3, tag_LOOT_CELLPHONE),
					stringAndInt(2, tag_LOOT_CHEMICAL),
					stringAndInt(1,  tag_LOOT_SILVERWARE) });
				loot = true;
			}
			break;
		case SITE_GOVERNMENT_WHITE_HOUSE:
		case SITE_GOVERNMENT_INTELLIGENCEHQ:
			if (!LCSrandom(3))
			{
				item = randomString({ stringAndInt(4, tag_WEAPON_SMG_MP5),
					stringAndInt(3, tag_WEAPON_AUTORIFLE_M16),
					stringAndInt(2, tag_WEAPON_SHOTGUN_PUMP),
					stringAndInt(1,  tag_WEAPON_CARBINE_M4) });
				weapon = true;
			}
			else if (!LCSrandom(2))
			{
				item = randomString({ stringAndInt(1, tag_ARMOR_BLACKSUIT) });
				armor = true;
			}
			else
			{
				item = randomString({ stringAndInt(5, tag_LOOT_SECRETDOCUMENTS),
					stringAndInt(3, tag_LOOT_CELLPHONE),
					stringAndInt(2, tag_LOOT_PDA),
					stringAndInt(1,  tag_LOOT_COMPUTER) });
				loot = true;
			}
			break;
		}
		if (loot) {
			itemindex = getloottype(item);
			if (itemindex > -1) { shelter->loot.push_back(new Loot(*loottype[itemindex])); }
			else { numberofxmlfails++; }
		}
		else if (armor) {
			itemindex = getarmortype(item);
			if (itemindex > -1) { shelter->loot.push_back(new Armor(*armortype[itemindex])); }
			else { numberofxmlfails++; }
		}
		else if (weapon) {
			itemindex = getweapontype(item);
			if (itemindex > -1) { shelter->loot.push_back(new Weapon(*weapontype[itemindex])); }
			else { numberofxmlfails++; }
		}
		else {
			numberofxmlfails++;
		}
	}
	eraseAlt();
	set_color_easy(WHITE_ON_BLACK);   mvaddstrAlt(6,  1, string_sleeper, gamelog);
	addstrAlt(cr.name, gamelog);
	addstrAlt(droppedOffPackage, gamelog);
	gamelog.nextMessage();
	if (numberofxmlfails > 0) {
		set_color(COLOR_RED, COLOR_BLUE, 1);
		mvaddstrAlt(8,  1, itemNotFound, xmllog);
		mvaddstrAlt(9,  1, numberofxmlfails, xmllog);
		addstrAlt(lostStolenItem, xmllog);
		set_color(COLOR_RED, COLOR_GREEN, 1);
		mvaddstrAlt(11,  1, contactModAuthor, xmllog);
		xmllog.nextMessage();
	}
	getkey();
}
/*********************************
**
**   SLEEPERS CREATING SCANDALS
**
**********************************/
void sleeper_scandal(Creature &cr, char &clearformess, char canseethings, int(&libpower)[VIEWNUM])
{
	// Add content here!
	return;
}
/*********************************
**
**   SLEEPERS RECRUITING
**
**********************************/
void sleeper_recruit(Creature &cr, char &clearformess, char canseethings, int(&libpower)[VIEWNUM])
{
	if (subordinatesleft(cr))
	{
		prepareencounter(location[cr.worklocation]->type, 0);
		for (int e = 0; e < 18; e++)
		{
			if (encounter[e].exists == false)
				break;
			if (encounter[e].worklocation == cr.worklocation || !LCSrandom(5))
			{
				if (encounter[e].align != 1 && LCSrandom(5))continue;
				Creature* recruit = new Creature(encounter[e]);
				liberalize(*recruit, 0);
				recruit->namecreature();
				recruit->hireid = cr.id;
				if (recruit->infiltration > cr.infiltration)
				{
					recruit->infiltration = cr.infiltration;
				}
				recruit->flag |= CREATUREFLAG_SLEEPER;
				location[recruit->worklocation]->mapped = 1;
				location[recruit->worklocation]->hidden = 0;
				pool.push_back(recruit);
				eraseAlt();
				mvaddstrAlt(6,  1, string_sleeper, gamelog);
				addstrAlt(cr.name, gamelog);
				addstrAlt(hasRecruited, gamelog);
				addstrAlt(recruit->get_type_name(), gamelog);
				addstrAlt(singleDot, gamelog);
				gamelog.newline();
				mvaddstrAlt(8,  1, recruit->name, gamelog);
				addstrAlt(looksForwardToServing, gamelog);
				gamelog.nextMessage();
				getkey();
				if (!subordinatesleft(cr))cr.activity.type = ACTIVITY_NONE;
				stat_recruits++;
				break;
			}
		}
	}
	return;
}


