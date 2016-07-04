#ifndef EQAPP_H
#define EQAPP_H

const char* g_applicationName    = "EQAPP";
const char* g_applicationExeName = "eqapp";

volatile int g_bLoaded  = 0;
volatile int g_bExit    = 0;
volatile int g_bConsole = 0;

HMODULE g_module;

HANDLE g_handleThreadLoad;
HANDLE g_handleThreadLoop;
HANDLE g_handleThreadConsole;

DWORD WINAPI EQAPP_ThreadLoad(LPVOID param);
DWORD WINAPI EQAPP_ThreadLoop(LPVOID param);
DWORD WINAPI EQAPP_ThreadConsole(LPVOID param);
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved);

//types
typedef struct _EQAPPESPSPAWN
{
    DWORD spawnInfo;
    char name[0x40];
    char lastName[0x20];
    float y;
    float x;
    float z;
    float distance;
    int level;
    int type;
    int _class;
    int guildId;
    int standingState;
    int isHoldingPrimary;
    int isHoldingSecondary;
    int isPet;
    int isLfg;
    int isPvp;
    int isTrader;
    int isAfk;
    int isGm;
    bool showAtAnyDistance = false;
    bool isTarget = false;
    bool isFindSpawn = false;
    bool isHalfDistance = false;
} EQAPPESPSPAWN, *PEQAPPESPSPAWN;

typedef struct _EQAPPESPCUSTOM
{
    float y;
    float x;
    float z;
    DWORD color;
    unsigned int size = 2;
    std::string text;
} EQAPPESPCUSTOM, *PEQAPPESPCUSTOM;

typedef struct _EQAPPSPAWNCASTSPELL
{
    DWORD spawnInfo;
    std::string spellName;
    DWORD spellCastTime;
    DWORD timer = 0;
} EQAPPSPAWNCASTSPELL, *PEQAPPSPAWNCASTSPELL;

typedef std::shared_ptr<EQAPPSPAWNCASTSPELL> SPEQAPPSPAWNCASTSPELL;

// globals
std::ios g_coutFlags(NULL);

int g_killSwitchKey = VK_PAUSE;

bool g_debugIsEnabled = false;
bool g_neverFrozenIsEnabled = true;
bool g_replaceRacesIsEnabled = true;
bool g_autoScreenshotIsEnabled = false;
bool g_hideCorpseLootedIsEnabled = true;

unsigned int g_videoModeWidth  = 1920;
unsigned int g_videoModeHeight = 1080;

std::vector<std::string> g_zoneShortNamesList;
std::vector<std::string>::iterator g_zoneShortNamesList_it;

bool g_hudIsEnabled = true;
int g_hudX = 5;
int g_hudY = 28;
float g_hudWidth  = 127.0f;
float g_hudHeight = 0.0f;

bool g_consoleIsEnabled = true;
GLFWwindow* g_consoleWindow;
HWND g_hwndConsoleWindow;
const char* g_consoleWindowTitle = "EQAPP Console";
unsigned int g_consoleWindowWidth = 1440;
unsigned int g_consoleWindowHeight = 900;
std::stringstream g_consoleStringStream;
std::streambuf* g_consoleStreamBuffer;

bool g_onScreenTextIsEnabled = true;
std::list<std::string> g_onScreenTextList;
unsigned int g_onScreenTextCountMax = 10;
unsigned int g_onScreenTextX = 1200;
unsigned int g_onScreenTextY = 500;
DWORD g_onScreenTextTimer = 0;
DWORD g_onScreenTextTimerDelay = 5000;
DWORD g_onScreenTextColor = 0xFFFFFFFF;

bool g_espIsEnabled = true;
bool g_espSpawnIsEnabled = true;
bool g_espGroundSpawnIsEnabled = true;
bool g_espDoorIsEnabled = false;
bool g_espSkeletonIsEnabled = false;
bool g_espZoneObjectIsEnabled = false;
bool g_espCustomIsEnabled = true;
bool g_espWaypointIsEnabled = true;
float g_espSpawnDistance = 400.0f;
float g_espGroundSpawnDistance = 200.0f;
float g_espDoorDistance = 100.0f;
float g_espSkeletonDistance = 100.0f;
float g_espZoneObjectDistance = 200.0f;
float g_espCustomDistance = 400.0f;
float g_espWaypointDistance = 1000.0f;
unsigned int g_espNpcCorpseMax = 10;
unsigned int g_espNumDrawText = 0;
std::vector<EQAPPESPSPAWN> g_espSpawnList;
DWORD g_espSpawnListTimer = 0;
DWORD g_espSpawnListTimerDelay = 1000;
std::vector<EQAPPESPCUSTOM> g_espCustomList;

DWORD g_espDoorColor        = 0xFFFFFFFF;
DWORD g_espZoneObjectColor  = 0xFFFFFFFF;
DWORD g_espLocatorColor     = 0xFFFFFFFF;
DWORD g_espGroundSpawnColor = 0xFFFFFFFF;
DWORD g_espWaypointColor    = 0xFFC0C0C0;
DWORD g_espFindColor        = 0xFF00FF80;

bool g_speedHackIsEnabled = true;
float g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_SPIRIT_OF_WOLF;

bool g_locatorIsEnabled = false;
float g_locatorY = 0.0f;
float g_locatorX = 0.0f;
float g_locatorZ = 0.0f;

bool g_findIsEnabled = false;
std::string g_findSpawnName;

bool g_alwaysAttackIsEnabled = false;

bool g_combatHotbuttonIsEnabled = false;
unsigned int g_combatHotbuttonIndex = 0;
DWORD g_combatHotbuttonTimer = 0;
DWORD g_combatHotbuttonTimerDelay = 1000;

bool g_alwaysHotbuttonIsEnabled = false;
unsigned int g_alwaysHotbuttonIndex = 0;
DWORD g_alwaysHotbuttonTimer = 0;
DWORD g_alwaysHotbuttonTimerDelay = 1000;

bool g_targetBeepIsEnabled = false;
DWORD g_targetBeepTimer = 0;
DWORD g_targetBeepTimerDelay = 1000;
std::string g_targetBeepName;

bool g_spawnBeepIsEnabled = false;
std::string g_spawnBeepName;

bool g_heightIsEnabled = true;
float g_heightMaximum = 5.0f;

bool g_drawDistanceIsEnabled = true;
float g_drawDistance = 1000.0f;
DWORD g_drawDistanceTimer = 0;
DWORD g_drawDistanceTimerDelay = 1000;

bool g_mapLabelsIsEnabled = false;
bool g_mapLabelsFilterIsEnabled = false;
DWORD g_mapLabelsData = 255;
DWORD g_mapLabelsTimer = 0;
DWORD g_mapLabelsTimerDelay = 10000;
std::string g_mapLabelsFilterName;

bool g_spawnCastSpellIsEnabled = true;
std::vector<SPEQAPPSPAWNCASTSPELL> g_spawnCastSpellList;
DWORD g_spawnCastSpellTimerDelay = 5000;

bool g_autoLootIsEnabled = false;
std::vector<std::string> g_autoLootList;
DWORD g_autoLootTimer = 0;
DWORD g_autoLootTimerDelay = 500;

bool g_waypointBotIsEnabled = false;
std::string g_waypointBotSpawnName;
DWORD g_waypointBotSpawnInfo;

const std::vector<DWORD> g_importantWindowsList
{
    EQ_POINTER_CAAWnd,
    EQ_POINTER_CBankWnd,
    EQ_POINTER_CBazaarWnd,
    EQ_POINTER_CBazaarSearchWnd,
    EQ_POINTER_CInspectWnd,
    EQ_POINTER_CInventoryWnd,
    EQ_POINTER_CLootWnd,
    EQ_POINTER_CMapViewWnd,
    EQ_POINTER_CMerchantWnd,
    EQ_POINTER_CSkillsWnd,
    EQ_POINTER_CSkillsSelectWnd,
    EQ_POINTER_CSocialEditWnd,
    EQ_POINTER_CSpellBookWnd,
    EQ_POINTER_CTextEntryWnd,
    EQ_POINTER_CTrackingWnd,
    EQ_POINTER_CTradeWnd,
    EQ_POINTER_CTrainWnd,
};

const std::vector<std::string> g_slashCommandsList
{
    "//help",
    "//unload",
    "//debug",
    "//test",
    "//hud",
    "//lootdebug",
    "//loot (item name)",
    "//autoloot, //al",
    "//resetautoloot, //resetal, //ral",
    "//autoloot (item name), //al (item name)",
    "//castray, //cr",
    "//getmeleerange, //getmr, //gmr",
    "//getspellname (spell id), //getsn (spell id), //gsn (spell id)",
    "//gettargetinfo, //getti, //gti",
    "//bank, //money",
    "//banklist, //bl",
    "//banklist (item name), //bl (item name)",
    "//inventorylist, //invlist, //il",
    "//inventorylist (item name), //invlist (item name), //il (item name)",
    "//spawnlist, //sl",
    "//spawnlist (spawn name), //sl (spawn name)",
    "//esp",
    "//espspawn, //esps",
    "//espgroundspawn, //espg",
    "//espdoor, //espd",
    "//espskeleton, //espsk",
    "//espzoneobject, //espzo",
    "//espcustom, //espc",
    "//setespspawndistance (distance), //setesps (distance), //setesp (distance)",
    "//setespgroundspawndistance (distance), //setespg (distance)",
    "//setespdoordistance (distance), //setespd (distance)",
    "//setespskeletondistance (distance), //setespsk (distance)",
    "//setespzoneobjectdistance (distance), //setespzo (distance)",
    "//setespcustomdistance (distance), //setespc (distance)",
    "//speed",
    "//setspeed sow",
    "//setspeed run1",
    "//setspeed run2",
    "//setspeed run3",
    "//setspeed (speed), //sets (speed), //ss (speed)",
    "//locator, //loc",
    "//setlocator (y) (x) (z), //setloc (y) (x) (z)",
    "//find",
    "//setfind (spawn name)",
    "//linetotarget, //ltt",
    "//memorylist, //memlist",
    "//memory (index), //mem (index)",
    "//alwaysattack, //aa",
    "//combathotbutton, //chb",
    "//setcombathotbutton (index), //setchb (index), //schb (index)",
    "//alwayshotbutton, //ahb",
    "//setalwayshotbutton (index), //setahb (index), //sahb (index)",
    "//setcollisionradius (radius), //setcr (radius), //scr (radius)",
    "//drawdistance, //dd",
    "//setdrawdistance (distance), //setdd  (distance)",
    "//setfieldofview (fov), //setfov (fov)",
    "//targetbeep, //tbeep",
    "//settargetbeep (name) (delay), //settbeep (name) (delay), //stb (name) (delay)",
    "//spawnbeep, //sbeep",
    "//setspawnbeep (name), //setsbeep (name), //ssb (name)",
    "//height",
    "//setheight (height), //seth (height), //sh (height)",
    "//maplabels, //ml",
    "//addmaplabels, //addml, //aml",
    "//removemaplabels, //removeml, //rml",
    "//getmaplabels, //getml, //gml",
    "//filtermaplabels, //filterml, //fml",
    "//filtermaplabels (spawn name), //filterml (spawn name), //fml (spawn name)",
    "//getzoneinfo, //getzone, //getzi, //gzi",
    "//loadmemory, //loadm, //lm",
    "//sounds",
    "//loadsounds, //loads, //ls",
    "//namedspawns, //ns",
    "//loadnamedspawns, //loadns, //lns",
    "//loadespcustom, //loadec, //lec",
    "//getnamedspawns, //getns, //gns",
    "//writecharacterfile, //writechar, //writecf, //wcf",
    "//loadtextoverlaychattext, //loadtoct, //ltoct",
    "//loadnobeep, //loadnb, //lnb",
    "//loadshortzonenames, //loadzsn, //lzsn",
    "//hidecorpselooted, //hcl",
    "//openalldoors, //opendoors, //oad",
    "//census",
    "//freecamera, //freecam, //fc",
    "//setviewactor (spawn name), //setview (spawn name), //setva (spawn name), //sva (spawn name)",
    "//setviewtarget, //setvt, //svt",
    "//resetviewactor, //resetview, //resetva, //rva",
    "//maplocations, //maplocs",
    "//getmaplocation, //getmaploc",
    "//getespcustomlocation, //getecl, //gecl",
    "//openmapfile, //omf",
};

#endif // EQAPP_H
