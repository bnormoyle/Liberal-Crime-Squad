
const string tag_value = "value";
const string tag_attribute = "attribute";
const string tag_skill = "skill";
const string MOSTLY_ENDINGS_FOLDER = "mostlyendings\\";
const string CONST_TEMPORARY_SQUAD_CAMELCAPS = "Temporary Squad";
const string CONST_ACTING_INDIVIDUALLY_CAMELCAPS = "Acting Individually";

void printIAmLeader();
void printPromotionScreenSetup(const int iteration);
void printPromotionScreenSetupB(const int iteration);
void printPromotionScreenSetupD(const int iteration, const int level);
void printIsLoveSlave();
void printIsBrainWashed();
void printJustThis(const string name);
void printPromotionFooter(const bool addPage);
void printPromotionHeader();
void printBaseAssignmentFooter(const bool longPool, const bool manyLocations);
void printBaseName(const bool selectedbase, const int iteration, const string locname);
void printLiberalNameInLocation(const bool currentLocation, const bool underSiege, const bool excludeMention, const int iteration, const string tname, const string lname);
void printBaseAssignmentHeader();
void printLosesHeart(const string name);
void printGainsWisdom(const string name);
void printSwapSquadMember();
void printNameWith(const string name);
void printPerformsExecution(const string executor, const string victim);
void printKillAllyPrompt(const string executor);
void printTestifiesAgainstBoss(const string tname, const string bname);
void printHasBeenReleased(const string name);
void printReleaseLiberalPrompt();
void printWhatIsNewName();
void printReviewModeNameFooter(const bool conservative, const bool morePages);
void printRemoveLiberal();
void printKillLiberal();
void printProfileHeader(const bool conservative);
void printSortPeople();
void printSwapMe(const string name);
void printReorderLiberals();
void printPressLetterToViewStats();
void printReviewStringsHeader(const short mode);
void printREVIEWMODE_CLINIC(const int clinic);
void printREVIEWMODE_SLEEPERS(const int align, const string tname);
void printREVIEWMODE_DEAD(const int deathdays);
void printREVIEWMODE_AWAY(const int hiding, const int dating);
void printREVIEWMODE_JUSTICE(const bool deathpenalty, const int sentence, const int location);
void printREVIEWMODE_HOSTAGES(const int joindays);
void printEvaluateLiberalsHeader(const int iteration, const string tname);
void printLiberalHealthStat(const bool bright, const int iteration, const int skill, const string hstat);
void printREVIEWMODE_LIBERALS();
void printREVIEWMODE_LIBERALS(const int type, const string activity);
void setColorAndPositionForReviewmode(const short mode, const int iteration, const string locationname);
void printGiveThisSquadAName(char* name);
void printSquadCannotBeOnlyConservative();
void printPressLetterToViewLiberalDetails();
void printSquadMustBeAbleToMove();
void printSquadMustBeInSameLocation();
void printAddOrRemoveFromSquad(const int partysize);
void printSquadGreen(const int iteration);
void printSquadYellow(const int iteration);
void printSquadAway(const int iteration);
void printCreatureTypename(const int iteration, const int align, const string type);
void printTotalSkill(const int iteration, const int skill, const bool bright);
void printCreatureNameForSquad(const int iteration, const string name);
void printAssembleSquadHeader(const int partysize, const char newsquad, const string name);
void printReviewModeFooter(const bool musicIsEnabled);
void printReviewModeOptions(const int activity, const int iteration, const int numMembers);
void printReviewHeader();
void printReviewActivity(const int activityTypeID, const int iteration, const string str);
void printSquadLocationAndSiegeStatus(const int cursite, const int y, const bool p);
void printSquadName(const string sname, const bool active, const int iteration);