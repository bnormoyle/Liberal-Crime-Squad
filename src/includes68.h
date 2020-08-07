




/*
	File created by Chris Johnson.
	These were previously all in game.cpp.

	This file includes all the the enums, defines, and included headers required globally by all components.

	All code released under GNU GPL.
*/
#ifndef INCLUDES_H_INCLUDED
#define INCLUDES_H_INCLUDED0
constexpr auto __USE_MINGW_ANSI_STDIO = 0;
#include <windows.h>
#include <io.h> //needed for unlink()
#include <direct.h>
#include <ciso646> // alternate keywords included in the ISO C++ standard
// but not directly supported by Microsoft Visual Studio C++
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <time.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <deque>
#include <queue>
#include <math.h>
#include <cstring>
#include "cmarkup/Markup.h" //For XML.
#include <locale.h>

#include <mbctype.h>




#include <string.h>

using namespace std;

#include "includesDeprecated.h"
//#include "vehicle/vehicletype.h"
//#include "vehicle/vehicle.h"


//just a float that is initialized to 0
#include "floatZero.h"
//Interrogation information for the InterrogationST system, to be
//dynamically created on capture and deleted when InterrogationST ends,
//referenced using a pointer typecast into one of the arguments
//of the target's current action.
#include "activityST.h"

int get_associated_attribute(int skill_type);


#include "includesDeprecatedB.h"
#ifdef	ACTIVATE_SLEEPERS_CPP
//activate_sleepers.cpp
#include "../creature/creature.h"
//#include "../common/commonactions.h"
void sorting_prompt(short listforsorting);
#include "../common/commonactionsCreature.h"
// for void sortliberals(std::vector<Creature *>&,short,bool)
#include "../common/commondisplay.h"
//#include "../common/commondisplayCreature.h"
void printcreatureinfo(DeprecatedCreature *cr, unsigned char knowledge = 255);
//#include "../common/getnames.h"
#include "../cursesAlternative.h"
#include "../cursesAlternativeConstants.h"
#include "../set_color_support.h"
#include "../locations/locationsPool.h"
#include "../common/musicClass.h"
#include "../common/creaturePoolCreature.h"
//extern string spaceDashSpace;
//extern string percentSign;


//	extern string percentSign;
extern MusicClass music;
extern short activesortingchoice[SORTINGCHOICENUM];
//// #include "../common/commonactions.h"
void sorting_prompt(short listforsorting);
const string tag_value = "value";
const string tag_attribute = "attribute";
const string tag_skill = "skill";

const string WILL_STEAL = " will steal equipment and send it to the Shelter.";
const string WILL_EMBEZZLE = " will embezzle money for the LCS.";
const string WILL_SNOOP = " will snoop around for secrets and enemy plans.";
const string WILL_TRY_TO_RECRUIT = " will try to recruit additional sleeper agents.";
const string WILL_BUILD_SUPPORT = " will build support for Liberal causes.";
const string WILL_STAY_OUT_OF_TROUBLE = " will stay out of trouble.";
const string THREE_STEAL = "3 - Steal Equipment";
const string TWO_EMBEZZLE = "2 - Embezzle Funds";
const string ONE_UNCOVER_SECRETS = "1 - Uncover Secrets";
const string THREE_NEED_MORE_JUICE_TO_RECRUIT = "3 - [Need More Juice to Recruit]";
const string THREE_ENLIGHTENED_CANNOT_RECRUIT = "3 - [Enlightened Can't Recruit]";
const string THREE_EXPAND_SLEEPER_NETWORK = "3 - Expand Sleeper Network";
const string TWO_ADVOCATE_LIBERALISM = "2 - Advocate Liberalism";
const string ONE_LAY_LOW = "1 - Lay Low";
const string ENTER_CONFIRM_SELECTION = "Enter - Confirm Selection";
const string C_JOIN_ACTIVE_LCS = "C - Join the Active LCS";
const string B_ESPIONAGE = "B - Espionage";
const string A_COMMUNICATION = "A - Communication and Advocacy";
const string FOCUS_ON = " focus on?";
const string TAKING_UNDERCOVER_ACTION_WHAT_WILL = "Taking Undercover Action:   What will ";

const string T_TO_SORT_PEOPLE = " T to sort people.";
const string PRESS_A_LETTER_TO_ASSIGN = "Press a Letter to Assign an Activity.";
const string EFFECTIVENESS_COLON = "Effectiveness: ";
const string ACTIVITY_HEADER = "ACTIVITY";
const string SITE_HEADER = "SITE";
const string JOB_HEADER = "JOB";
const string CODE_NAME_HEADER = "CODE NAME";
const string ACTIVATE_SLEEPER = "Activate Sleeper Agents";

#endif	//ACTIVATE_SLEEPERS_CPP
#endif // INCLUDES_H_INCLUDED
