




/*
	File created by Chris Johnson.
	These were previously all in game.cpp.

	This file includes all the the enums, defines, and included headers required globally by all components.

	All code released under GNU GPL.
*/
#include <windows.h>
#include <vector>
#include <map>
#include <deque>
#include "cmarkup/Markup.h"
using namespace std;

#include "includesDeprecated.h"
//#include "vehicle/vehicletype.h"
//#include "vehicle/vehicle.h"


//just a float that is initialized to 0
//#include "floatZero.h"
//Interrogation information for the InterrogationST system, to be
//dynamically created on capture and deleted when InterrogationST ends,
//referenced using a pointer typecast into one of the arguments
//of the target's current action.
//#include "activityST.h"

//int get_associated_attribute(int skill_type);

//#include "includesDeprecatedB.h"
//#ifdef	ARMOR_CPP
// armor.cpp

#include "../items/itemtype.h"
#include "../items/item.h"
#include "../items/armortype.h"
#include "../items/armor.h"
//own header
#include "../common/stringconversion.h"
//for int stringtobool(std::string)
#include "../common/translateid.h"
// for  int getarmortype

extern vector<ArmorType *> armortype;
//#endif//ARMOR_CPP
//#endif// INCLUDES_H_INCLUDED
