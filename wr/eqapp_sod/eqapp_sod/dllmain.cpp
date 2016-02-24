#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <ctime>

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <tlhelp32.h>
#include <fcntl.h>
#include <io.h>

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

//#define DIRECTINPUT_VERSION 0x0800
//#include "dinput.h"
//#pragma comment(lib, "dinput.lib")

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "jansson.h"
#pragma comment(lib, "jansson.lib")

#include "eqsod.h"
#include "eqsod_functions.h"

const char* g_applicationName    = "EQAPP";
const char* g_applicationExeName = "eqapp";

volatile int g_bLoaded  = 0;
volatile int g_bExit    = 0;
volatile int g_bConsole = 0;

HWND g_hwnd;

HMODULE g_module;

HANDLE g_handleThreadLoad;
HANDLE g_handleThreadConsole;
HANDLE g_handleThreadLoop;

// detours
EQ_FUNCTION_TYPE_Exit EQAPP_REAL_Exit = NULL;
EQ_FUNCTION_TYPE_DrawNetStatus EQAPP_REAL_DrawNetStatus = NULL;
EQ_FUNCTION_TYPE_ExecuteCmd EQAPP_REAL_ExecuteCmd = NULL;

EQ_FUNCTION_TYPE_CDisplay__CreatePlayerActor EQAPP_REAL_CDisplay__CreatePlayerActor = NULL;
EQ_FUNCTION_TYPE_CDisplay__DeleteActor EQAPP_REAL_CDisplay__DeleteActor = NULL;
//EQ_FUNCTION_TYPE_CDisplay__SetNameSpriteState EQAPP_REAL_CDisplay__SetNameSpriteState = NULL;
//EQ_FUNCTION_TYPE_CDisplay__SetNameSpriteTint EQAPP_REAL_CDisplay__SetNameSpriteTint = NULL;

EQ_FUNCTION_TYPE_CEverQuest__dsp_chat EQAPP_REAL_CEverQuest__dsp_chat = NULL;
EQ_FUNCTION_TYPE_CEverQuest__EnterZone EQAPP_REAL_CEverQuest__EnterZone = NULL;
EQ_FUNCTION_TYPE_CEverQuest__InterpretCmd EQAPP_REAL_CEverQuest__InterpretCmd = NULL;
EQ_FUNCTION_TYPE_CEverQuest__MoveToZone EQAPP_REAL_CEverQuest__MoveToZone = NULL;
EQ_FUNCTION_TYPE_CEverQuest__SetGameState EQAPP_REAL_CEverQuest__SetGameState = NULL;
EQ_FUNCTION_TYPE_CEverQuest__StartCasting EQAPP_REAL_CEverQuest__StartCasting = NULL;

EQ_FUNCTION_TYPE_CMapViewWnd__dCMapViewWnd EQAPP_REAL_CMapViewWnd__dCMapViewWnd = NULL;
EQ_FUNCTION_TYPE_CMapViewWnd__DrawMap EQAPP_REAL_CMapViewWnd__DrawMap = NULL;

EQ_FUNCTION_TYPE_EQ_Character__eqspa_movement_rate EQAPP_REAL_EQ_Character__eqspa_movement_rate = NULL;

EQ_FUNCTION_TYPE_EQPlayer__ChangePosition EQAPP_REAL_EQPlayer__ChangePosition = NULL;
EQ_FUNCTION_TYPE_EQPlayer__do_change_form EQAPP_REAL_EQPlayer__do_change_form = NULL;
EQ_FUNCTION_TYPE_EQPlayer__SetRace EQAPP_REAL_EQPlayer__SetRace = NULL;

EQ_FUNCTION_TYPE_MapViewMap__Save EQAPP_REAL_MapViewMap__Save = NULL;
EQ_FUNCTION_TYPE_MapViewMap__SaveEx EQAPP_REAL_MapViewMap__SaveEx = NULL;

// types
typedef struct _EQAPPMEMORY
{
    size_t index;
    std::string name;
    std::string filename;
    std::string description;
    bool isEnabled;
} EQAPPMEMORY, *PEQAPPMEMORY;

typedef struct _EQAPPSOUND
{
    size_t index;
    std::string text;
    std::string filename;
    bool isEnabled;
} EQAPPSOUND, *PEQAPPSOUND;

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
    bool useSmallFont = false;
} EQAPPESPSPAWN, *PEQAPPESPSPAWN;

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

bool g_debugIsEnabled = false;

bool g_onScreenTextIsEnabled = true;
std::list<std::string> g_onScreenTextList;
unsigned int g_onScreenTextMax = 10;
unsigned int g_onScreenTextX = 1200;
unsigned int g_onScreenTextY = 500;
DWORD g_onScreenTextTimer = 0;
DWORD g_onScreenTextDelay = 5000;

bool g_memoryIsEnabled = true;
std::vector<EQAPPMEMORY> g_memoryList;

bool g_soundsIsEnabled = true;
std::vector<EQAPPSOUND> g_soundsList;

bool g_espIsEnabled = true;
bool g_espSpawnIsEnabled = true;
bool g_espGroundSpawnIsEnabled = true;
bool g_espDoorIsEnabled = false;
bool g_espSkeletonIsEnabled = false;
bool g_espZoneObjectIsEnabled = false;
float g_espSpawnDistance = 400.0f;
float g_espGroundSpawnDistance = 200.0f;
float g_espDoorDistance = 100.0f;
float g_espSkeletonDistance = 100.0f;
float g_espZoneObjectDistance = 200.0f;
unsigned int g_espNpcCorpseMax = 10;
unsigned int g_espNumDrawText = 0;
std::vector<EQAPPESPSPAWN> g_espSpawnList;
DWORD g_espSpawnListTimer = 0;
DWORD g_espSpawnListDelay = 1000;

bool g_speedHackIsEnabled = true;
float g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_SPIRIT_OF_WOLF;

bool g_locatorIsEnabled = false;
float g_locatorY = 0.0f;
float g_locatorX = 0.0f;
float g_locatorZ = 0.0f;

bool g_findIsEnabled = false;
std::string g_findSpawnName;

bool g_textOverlayOnChatTextIsEnabled = true;
std::vector<std::string> g_textOverlayChatTextList;
int g_textOverlayChatTextDuration = 5000;

bool g_lineToTargetIsEnabled = true;

bool g_namedSpawnsIsEnabled = true;
std::vector<std::string> g_namedSpawnsList;
unsigned int g_namedSpawnsX = 1200;
unsigned int g_namedSpawnsY = 28;
float g_namedSpawnsWidth  = 256.0f;
float g_namedSpawnsHeight = 512.0f;

bool g_alwaysAttackIsEnabled = false;

bool g_combatHotbuttonIsEnabled = false;
unsigned int g_combatHotbuttonIndex = 0;
DWORD g_combatHotbuttonTimer = 0;
DWORD g_combatHotbuttonDelay = 1000;

bool g_alwaysHotbuttonIsEnabled = false;
unsigned int g_alwaysHotbuttonIndex = 0;
DWORD g_alwaysHotbuttonTimer = 0;
DWORD g_alwaysHotbuttonDelay = 1000;

bool g_targetBeepIsEnabled = false;
DWORD g_targetBeepTimer = 0;
DWORD g_targetBeepDelay = 1000;
std::string g_targetBeepName;

bool g_spawnBeepIsEnabled = false;
std::string g_spawnBeepName;

bool g_heightIsEnabled = true;
float g_heightMaximum = 5.0f;

bool g_drawDistanceIsEnabled = true;
float g_drawDistance = 1000.0f;
DWORD g_drawDistanceTimer = 0;
DWORD g_drawDistanceDelay = 1000;

bool g_hideCorpseLootedIsEnabled = true;

bool g_healthBarsIsEnabled = false;
bool g_healthBarsShowGroupIsEnabled = false;
unsigned int g_healthBarsMax = 20;
unsigned int g_healthBarsX = 1200;
unsigned int g_healthBarsY = 28;
float g_healthBarsWidth  = 256.0f;
float g_healthBarsHeight = 512.0f;
std::vector<std::string> g_healthBarsSpawnNamesList;

bool g_mapLabelsIsEnabled = false;
bool g_mapLabelsFilterIsEnabled = false;
DWORD g_mapLabelsData = 255;
DWORD g_mapLabelsTimer = 0;
DWORD g_mapLabelsDelay = 10000;
std::string g_mapLabelsFilterName;

bool g_neverFrozenIsEnabled = true;

bool g_spawnCastSpellIsEnabled = true;
std::vector<SPEQAPPSPAWNCASTSPELL> g_spawnCastSpellList;
DWORD g_spawnCastSpellDelay = 5000;

bool g_autoLootIsEnabled = false;
std::vector<std::string> g_autoLootList;
DWORD g_autoLootTimer = 0;
DWORD g_autoLootDelay = 1000;

bool g_freeCameraIsEnabled = false;
float g_freeCameraMultiplier = 1.0f;

bool g_replaceRacesIsEnabled = true;

bool g_autoScreenshotIsEnabled = false;

bool g_censusIsActive = false;
std::vector<std::string> g_shortZoneNamesList;

// important windows
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

// slash commands
// TODO
const std::vector<std::string> g_slashCommandsList
{
    "//help",
    "//unload",
    "//debug",
    "//test",
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
    "//setespspawndistance (distance), //setesps (distance), //setesp (distance)",
    "//setespgroundspawndistance (distance), //setespg (distance)",
    "//setespdoordistance (distance), //setespd (distance)",
    "//setespskeletondistance (distance), //setespsk (distance)",
    "//setespzoneobjectdistance (distance), //setespzo (distance)",
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
    "//healthbars, //hb",
    "//resethealthbars, //resethb, //rhb",
    "//healthbars (spawnname), //hb (spawnname)",
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
    "//getnamedspawns, //getns, //gns",
    "//writecharacterfile, //writechar, //writecf, //wcf",
    "//loadtextoverlaychattext, //loadtoct, //ltoct",
    "//loadshortzonenames, //loadszn, //lszn",
    "//hidecorpselooted, //hcl",
    "//openalldoors, //opendoors, //oad",
    "//census",
    "//freecamera, //freecam, //fc",
    "//setviewactor (spawn name), //setview (spawn name), //setva (spawn name), //sva (spawn name)",
    "//setviewtarget, //setvt, //svt",
    "//resetviewactor, //resetview, //resetva, //rva",
    "//maplocations, //maplocs",
    "//getmaplocation, //getmaploc",
    "//openmapfile, //omf",
};

// function prototypes
void EQAPP_EnableDebugPrivileges();
void EQAPP_LoadConsole();
void EQAPP_CoutSaveFlags();
void EQAPP_CoutRestoreFlags();
void EQAPP_DoWindowTitles();
bool EQAPP_IsForegroundWindowCurrentProcessId();
DWORD EQAPP_GetModuleBaseAddress(const wchar_t* moduleName);
void EQAPP_LoadGraphicsDllFunctions();
void EQAPP_DoLoad();
void EQAPP_DoUnload();
void EQAPP_AddDetours();
void EQAPP_RemoveDetours();
void EQAPP_LoadMemory();
void EQAPP_UnloadMemory();
void EQAPP_ToggleMemory(PEQAPPMEMORY pMemory, bool bEnabled);
void EQAPP_LoadSounds();
void EQAPP_LoadNamedSpawns();
void EQAPP_DoNamedSpawns();
void EQAPP_LoadTextOverlayChatText();
void EQAPP_DoHud();
void EQAPP_DoHealthBars();
void EQAPP_DoTargetBeep();
void EQAPP_DoSpawnBeep(DWORD spawnInfo);
void EQAPP_DoSpawnMessage(DWORD spawnInfo);
void EQAPP_DoDespawnMessage(DWORD spawnInfo);
void EQAPP_DoLineToTarget();
void EQAPP_DoAlwaysAttack();
void EQAPP_DoCombatHotbutton();
void EQAPP_DoAlwaysHotbutton();
void EQAPP_UpdateEspSpawnList();
void EQAPP_DoEsp();
void EQAPP_DrawSpawnSkeleton(DWORD spawnInfo);
void EQAPP_DoMaxSwimming();
void EQAPP_DoHeight();
void EQAPP_DoDrawDistance();
void EQAPP_DoHideCorpseLooted();
void EQAPP_DoMapLabels();
void EQAPP_RemoveMapLabels();
void EQAPP_DoSpawnCastSpell();
void EQAPP_DoSpawnCastSpellAddToList(DWORD spawnInfo, DWORD spellId, DWORD spellCastTime);
void EQAPP_DoInterpretCommand(const char* command);
void EQAPP_DoSpawnList(const char* filterSpawnName);
void EQAPP_DoAutoLoot();
void EQAPP_DoBankList(const char* filterItemName);
void EQAPP_DoInventoryList(const char* filterItemName);
void EQAPP_WriteCharacterFile();
void EQAPP_WriteCharacterFileJson();
void EQAPP_DoOpenAllDoors();
void EQAPP_ToggleFreeCamera(bool b);
void EQAPP_DoFreeCameraKeys();
void EQAPP_DoOnScreenText();
void EQAPP_DoOnScreenTextAddToList(std::string str);
void EQAPP_DoMapLocations();
void EQAPP_LoadShortZoneNames();
void EQAPP_DoCensus();
bool EQAPP_AreImportantWindowsOpen();
DWORD WINAPI EQAPP_ThreadLoop(LPVOID param);
DWORD WINAPI EQAPP_ThreadLoad(LPVOID param);
DWORD WINAPI EQAPP_ThreadConsole(LPVOID param);
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved);

void EQAPP_EnableDebugPrivileges()
{
    HANDLE token;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
    {
        TOKEN_PRIVILEGES tp;
        TOKEN_PRIVILEGES tpPrevious;

        DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);

        LUID luid;

        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
        {
            tp.PrivilegeCount           = 1;
            tp.Privileges[0].Luid       = luid;
            tp.Privileges[0].Attributes = 0;

            AdjustTokenPrivileges
            (
                token,
                FALSE,
                &tp,
                sizeof(TOKEN_PRIVILEGES),
                &tpPrevious,
                &cbPrevious
            );

            tpPrevious.PrivilegeCount            = 1;
            tpPrevious.Privileges[0].Luid        = luid;
            tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
    
            AdjustTokenPrivileges
            (
                token,
                FALSE,
                &tpPrevious,
                cbPrevious,
                NULL,
                NULL
            );
        }
    }

    CloseHandle(token);
}

void EQAPP_LoadConsole()
{
    static const WORD MAX_CONSOLE_LINES = 500;
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
    FILE* fp;

    AllocConsole();

    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleScreenBufferInfo);
    consoleScreenBufferInfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), consoleScreenBufferInfo.dwSize);

    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);

    // redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "r");
    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);

    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stderr = *fp;
    setvbuf(stderr, NULL, _IONBF, 0);

    std::ios::sync_with_stdio();

    HWND hwnd = GetConsoleWindow();
    if (hwnd != NULL)
    {
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(0, IDI_APPLICATION));
        SendMessage(hwnd, WM_SETICON, ICON_BIG  , (LPARAM)LoadIcon(0, IDI_APPLICATION));

        // disable the close button
        HMENU hMenu = GetSystemMenu(hwnd, FALSE);
        if (hMenu != NULL)
        {
            DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
        }
    }

    g_bConsole = 1;
}

void EQAPP_CoutSaveFlags()
{
    g_coutFlags.copyfmt(std::cout);
}

void EQAPP_CoutRestoreFlags()
{
    std::cout.copyfmt(g_coutFlags);
}

bool EQAPP_IsForegroundWindowCurrentProcessId()
{
    HWND foregroundHwnd = GetForegroundWindow();

    DWORD foregroundProcessId;
    GetWindowThreadProcessId(foregroundHwnd, &foregroundProcessId);

    return (foregroundProcessId == GetCurrentProcessId());
}

DWORD EQAPP_GetModuleBaseAddress(const wchar_t* moduleName)
{
    DWORD moduleBaseAddress = 0;

    DWORD processId = GetCurrentProcessId();

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);

    if (snapshot != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 ModuleEntry32 = {0};
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(snapshot, &ModuleEntry32))
        {
            do
            {
                if (wcscmp(ModuleEntry32.szModule, moduleName) == 0)
                {
                    moduleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                    break;
                }
            } while (Module32Next(snapshot, &ModuleEntry32));
        }

        CloseHandle(snapshot);
    }

    return moduleBaseAddress;
}

template <class T>
void EQAPP_Log(const char* text, T number)
{
    std::fstream file;
    file.open("eqapp/log.txt", std::ios::out | std::ios::app);

    file << "[" << __TIME__ << "] " << text << " (" << number << ")" << " Hex(" << std::hex << number << std::dec << ")" << std::endl;

    file.close();
}

int EQAPP_IniReadInt(const char* filename, const char* section, const char* key, int defaultValue)
{
    return GetPrivateProfileIntA(section, key, defaultValue, filename);
}

bool EQAPP_IniReadBool(const char* filename, const char* section, const char* key, int defaultValue)
{
    int result = GetPrivateProfileIntA(section, key, defaultValue, filename);

    return (result != 0);
}

float EQAPP_IniReadFloat(const char* filename, const char* section, const char* key, float defaultValue)
{
    char bufferDefault[128];
    _snprintf_s(bufferDefault, sizeof(bufferDefault), _TRUNCATE, "%f", defaultValue);

    char bufferResult[128];
    GetPrivateProfileStringA(section, key, bufferDefault, bufferResult, sizeof(bufferResult), filename);

    float result = (float)atof(bufferResult);

    return result;
}

std::string EQAPP_IniReadString(const char* filename, const char* section, const char* key, const char* defaultValue)
{
    char result[1024];
    GetPrivateProfileStringA(section, key, defaultValue, result, 1024, filename);
    return result;
}

void EQAPP_DoLoad()
{
    AttachConsole(-1);

    std::cout << g_applicationName << std::endl;

    EQAPP_LoadMemory();
    EQAPP_LoadSounds();
    EQAPP_LoadNamedSpawns();
    EQAPP_LoadTextOverlayChatText();
    EQAPP_LoadShortZoneNames();

    EQAPP_WriteCharacterFile();
    EQAPP_WriteCharacterFileJson();

    std::cout << "Loaded." << std::endl;

    g_bLoaded = 0;
}

void EQAPP_DoUnload()
{
    g_spawnCastSpellList.clear();

    EQ_ResetViewActor();

    EQAPP_RemoveMapLabels();

    EQAPP_UnloadMemory();

    EQAPP_ToggleFreeCamera(false);

    std::cout << "Unloaded." << std::endl;

    g_bExit = 1;

    HWND window = EQ_ReadMemory<HWND>(EQ_WINDOW_HWND);
    if (window != NULL)
    {
        SetWindowTextA(window, EQ_STRING_WINDOW_TITLE);
    }
}

void EQAPP_LoadMemory()
{
    g_memoryList.clear();

    g_memoryIsEnabled = EQAPP_IniReadBool("eqapp/memory.ini", "Memory", "bEnabled", 1);

    if (g_memoryIsEnabled == false)
    {
        std::cout << "Memory is disabled." << std::endl;
        return;
    }

    for (size_t i = 0; i < 255; i++)
    {
        std::stringstream ssFilename;
        ssFilename << "sFilename" << i;

        std::string filename = EQAPP_IniReadString("eqapp/memory.ini", "Memory", ssFilename.str().c_str(), "");
        if (filename.size() == 0)
        {
            break;
        }

        std::stringstream filepath;
        filepath << "eqapp/memory/" << filename;

        std::stringstream ssEnabled;
        ssEnabled << "bEnabled" << i;

        EQAPPMEMORY memory;
        memory.isEnabled = EQAPP_IniReadBool("eqapp/memory.ini", "Memory", ssEnabled.str().c_str(), 0);

        std::cout << __FUNCTION__ << ": " << filename << " isEnabled: " << std::boolalpha << memory.isEnabled << std::noboolalpha << std::endl;

        memory.index = i;

        memory.filename = filepath.str();

        memory.name        = EQAPP_IniReadString(filepath.str().c_str(), "Memory", "Name", "");
        memory.description = EQAPP_IniReadString(filepath.str().c_str(), "Memory", "Description", "");
        
        g_memoryList.push_back(memory);
    }

    for (auto& memory : g_memoryList)
    {
        if (memory.isEnabled == true)
        {
            EQAPP_ToggleMemory(&memory, true);

            std::cout << __FUNCTION__ << ": " << memory.filename << " has been enabled." << std::endl;
        }
    }
}

void EQAPP_UnloadMemory()
{
    for (auto& memory : g_memoryList)
    {
        EQAPP_ToggleMemory(&memory, false);
    }
}

void EQAPP_ToggleMemory(PEQAPPMEMORY pMemory, bool bEnable)
{
    if (pMemory == NULL)
    {
        std::cout << __FUNCTION__ << ": " << "pMemory is NULL" << std::endl;
        return;
    }

    pMemory->isEnabled = bEnable;

    for (size_t i = 0; i < 8; i++)
    {
        std::stringstream ssAddress;
        ssAddress << "Address" << i;

        std::string sAddress = EQAPP_IniReadString(pMemory->filename.c_str(), "Memory", ssAddress.str().c_str(), "");
            
        if (sAddress.size() == 0)
        {
            continue;
        }

        if (sAddress.find("+") == std::string::npos)
        {
            continue;
        }

        std::transform(sAddress.begin(), sAddress.end(), sAddress.begin(), ::tolower);

        std::stringstream ssEnable;
        ssEnable << "Enable" << i;

        std::stringstream ssDisable;
        ssDisable << "Disable" << i;

        std::string sEnable  = EQAPP_IniReadString(pMemory->filename.c_str(), "Memory", ssEnable.str().c_str(), "");
        std::string sDisable = EQAPP_IniReadString(pMemory->filename.c_str(), "Memory", ssDisable.str().c_str(), "");

        DWORD baseAddress = 0;

        if (sAddress.find("eqgame.exe") != std::string::npos)
        {
            baseAddress = EQ_BASE_ADDRESS;
        }
        else if (sAddress.find("eqgraphicsdx9.dll") != std::string::npos || sAddress.find("EQGraphicsDX9.dll") != std::string::npos)
        {
            baseAddress = EQ_ReadMemory<DWORD>(EQ_POINTER_GRAPHICS_DLL_BASE_ADDRESS);
        }

        std::istringstream addressSplit(sAddress);
        std::vector<std::string> addressTokens;
        for (std::string addressToken; std::getline(addressSplit, addressToken, '+'); addressTokens.push_back(addressToken));

        DWORD offset = std::stoul(addressTokens.back(), nullptr, 16);

        DWORD address = baseAddress + offset;

        std::string sBytes;
        if (bEnable == true)
        {
            sBytes = sEnable;
        }
        else
        {
            sBytes = sDisable;
        }

        if (sBytes.size() == 0)
        {
            break;
        }

        std::istringstream bytesSplit(sBytes);
        std::vector<std::string> bytesTokens;
        for (std::string bytesToken; std::getline(bytesSplit, bytesToken, ' '); bytesTokens.push_back(bytesToken));

        BYTE value[256];
        size_t valueSize = 0;

        for (auto& token : bytesTokens)
        {
            value[valueSize] = (BYTE)(std::stoul(token, nullptr, 16));
            valueSize++;
        }

        DWORD oldProtection;
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, valueSize, PAGE_EXECUTE_READWRITE, &oldProtection);

        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, value, valueSize, 0);

        DWORD tempProtection;
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, valueSize, oldProtection, &tempProtection);
    }
}

void EQAPP_LoadSounds()
{
    g_soundsList.clear();

    g_soundsIsEnabled = EQAPP_IniReadBool("eqapp/sounds.ini", "Sounds", "bEnabled", 1);

    if (g_soundsIsEnabled == false)
    {
        std::cout << "Sounds are disabled." << std::endl;
        return;
    }

    for (size_t i = 0; i < 255; i++)
    {
        std::stringstream ssFilename;
        ssFilename << "sFilename" << i;

        std::string filename = EQAPP_IniReadString("eqapp/sounds.ini", "Sounds", ssFilename.str().c_str(), "");
        if (filename.size() == 0)
        {
            break;
        }

        std::stringstream filepath;
        filepath << "eqapp\\sounds\\" << filename;

        std::stringstream ssText;
        ssText << "sText" << i;

        std::string text = EQAPP_IniReadString("eqapp/sounds.ini", "Sounds", ssText.str().c_str(), "");
        if (text.size() == 0)
        {
            break;
        }

        std::stringstream ssEnabled;
        ssEnabled << "bEnabled" << i;

        bool isEnabled = EQAPP_IniReadBool("eqapp/sounds.ini", "Sounds", ssEnabled.str().c_str(), 0);

        std::cout << __FUNCTION__ << ": " << filename << " isEnabled: " << std::boolalpha << isEnabled << std::noboolalpha << std::endl;

        EQAPPSOUND sound;
        sound.index     = i;
        sound.text      = text;
        sound.filename  = filepath.str();
        sound.isEnabled = isEnabled;
        
        g_soundsList.push_back(sound);
    }
}

void EQAPP_LoadNamedSpawns()
{
    g_namedSpawnsList.clear();

    std::ifstream file;
    std::string line;

    file.open("eqapp/namedspawns.txt", std::ios::in);

    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": Failed to open file: eqapp/namedspawns.txt" << std::endl;
    }

    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        std::cout << __FUNCTION__ << ": "<< line << std::endl;

        g_namedSpawnsList.push_back(line);
    }

    file.close();

    char zoneShortName[0x20] = {0};
    memcpy(zoneShortName, (LPVOID)(EQ_ZONEINFO_SHORT_NAME), sizeof(zoneShortName));

    if (strlen(zoneShortName) == 0)
    {
        return;
    }

    std::string sZoneShortName = zoneShortName;

    auto find = EQ_KEYVALUE_SHORT_ZONE_NAMES_WR.find(zoneShortName);
    if (find != EQ_KEYVALUE_SHORT_ZONE_NAMES_WR.end())
    {
        sZoneShortName = find->second;
    }

    std::stringstream filePath;
    filePath << "eqapp/namedspawns/" << sZoneShortName << ".txt";

    file.open(filePath.str().c_str(), std::ios::in);

    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": Failed to open file: " << filePath.str() << std::endl;
    }

    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        std::cout << __FUNCTION__ << ": "<< line << std::endl;

        g_namedSpawnsList.push_back(line);
    }

    file.close();
}

void EQAPP_DoMapLocations()
{
    char zoneShortName[0x20] = {0};
    memcpy(zoneShortName, (LPVOID)(EQ_ZONEINFO_SHORT_NAME), sizeof(zoneShortName));

    if (strlen(zoneShortName) == 0)
    {
        return;
    }

    std::stringstream filePath;
    filePath << "eqapp/maplocations/" << zoneShortName << ".txt";

    std::fstream file;
    file.open(filePath.str().c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125);

        if (spawnType != EQ_SPAWN_TYPE_NPC)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        std::string mapLocation = EQ_GetSpawnMapLocationString(spawn);

        file << mapLocation << std::endl;

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    file.close();
}

void EQAPP_DoNamedSpawns()
{
    if (g_namedSpawnsIsEnabled == false || g_namedSpawnsList.size() == 0)
    {
        return;
    }

    unsigned int fontSize   = 2;
    unsigned int fontHeight = EQ_GetFontHeight(fontSize);

    unsigned int numSpawns = 0;

    unsigned int x = g_namedSpawnsX;
    unsigned int y = g_namedSpawnsY;

    if (g_namedSpawnsWidth > 0 && g_namedSpawnsHeight > 0)
    {
        EQ_DrawQuad((float)x, (float)y, g_namedSpawnsWidth, g_namedSpawnsHeight, EQ_TOOLTIP_COLOR);
    }

    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125);

        if (spawnType != EQ_SPAWN_TYPE_NPC)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        int spawnIsPet = EQ_ReadMemory<DWORD>(spawn + 0x260);

        if (spawnIsPet != 0)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        int spawnLevel = EQ_ReadMemory<BYTE>(spawn + 0x315);

        if (spawnLevel < 1 || spawnLevel > 100)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        char spawnName[0x40] = {0};
        memcpy(spawnName, (LPVOID)(spawn + 0xE4), sizeof(spawnName));

        for (auto& namedSpawn : g_namedSpawnsList)
        {
            if (strstr(spawnName, namedSpawn.c_str()) != NULL)
            {
                numSpawns++;

                EQ_DrawText(spawnName, x, y, 0xFFFFFFFF, fontSize);
                y += fontHeight;

                break;
            }
        }

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    if (numSpawns > 0)
    {
        g_namedSpawnsHeight = (float)(numSpawns * fontHeight);
    }
    else
    {
        g_namedSpawnsHeight = 0;
    }
}

void EQAPP_DoOnScreenTextAddToList(std::string str)
{
    if (EQ_IsInGame() == false)
    {
        return;
    }

    g_onScreenTextTimer = EQ_GetTimer();
    g_onScreenTextList.push_back(str);
}

void EQAPP_DoOnScreenText()
{
    if (g_onScreenTextIsEnabled == false || g_onScreenTextList.size() == 0)
    {
        return;
    }

    if (EQ_HasTimePassed(g_onScreenTextTimer, g_onScreenTextDelay) == true)
    {
        g_onScreenTextList.pop_front();
    }

    unsigned int fontSize   = 2;
    unsigned int fontHeight = EQ_GetFontHeight(fontSize);

    unsigned int x = g_onScreenTextX;
    unsigned int y = g_onScreenTextY;

    size_t textCount = 0;

    for (auto g_onScreenTextList_it = g_onScreenTextList.rbegin(); g_onScreenTextList_it != g_onScreenTextList.rend(); g_onScreenTextList_it++)
    {
        std::string text = *g_onScreenTextList_it;

        if (textCount >= g_onScreenTextMax)
        {
            break;
        }

        std::stringstream ss;
        ss << text;

        EQ_DrawText(ss.str().c_str(), x, y, 0xFFFFFFFF, fontSize);
        y -= fontHeight;

        textCount++;
    }
}

void EQAPP_LoadTextOverlayChatText()
{
    g_textOverlayChatTextList.clear();

    std::ifstream file;
    file.open("eqapp/textoverlaychattext.txt", std::ios::in);

    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": Failed to open file." << std::endl;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        g_textOverlayChatTextList.push_back(line);
    }

    file.close();
}

void EQAPP_LoadShortZoneNames()
{
    g_shortZoneNamesList.clear();

    std::ifstream file;
    file.open("eqapp/shortzonenames.txt", std::ios::in);

    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": Failed to open file." << std::endl;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        g_shortZoneNamesList.push_back(line);
    }

    file.close();
}

bool EQAPP_AreImportantWindowsOpen()
{
    for (auto& window : g_importantWindowsList)
    {
        if (EQ_IsWindowVisible(window) == true)
        {
            return true;
        }
    }

    return false;
}

void EQAPP_DoCensus()
{
    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);
    if (playerSpawn == NULL)
    {
        return;
    }

    std::srand(unsigned(std::time(0)));
    std::random_shuffle(g_shortZoneNamesList.begin(), g_shortZoneNamesList.end());

    g_censusIsActive = true;

    for (auto& shortZoneName : g_shortZoneNamesList)
    {
        if (g_censusIsActive == false)
        {
            break;
        }

        std::stringstream ss;
        ss << "/who all " << shortZoneName;

        std::cout << "Census: " << ss.str() << std::endl;

        EQ_CEverQuest->InterpretCmd(playerSpawn, ss.str().c_str());
        Sleep(1000);
    }

    g_censusIsActive = false;
}

void EQAPP_DoHud()
{
    unsigned int x = 5;
    unsigned int y = 28;

    float boxSize = 127.0f;

    EQ_DrawQuad((float)x, (float)y, boxSize, boxSize, EQ_TOOLTIP_COLOR);

    unsigned int fontSize   = 2;
    unsigned int fontHeight = EQ_GetFontHeight(fontSize);

    std::stringstream ss;
    ss.precision(2);
    ss << g_applicationName << " - " << std::dec << EQ_GetAverageFps() << " FPS";

    EQ_DrawText(ss.str().c_str(), x, y, 0xFF808080, fontSize);
    y += fontHeight;

    DWORD numPlayersInZone = EQ_GetNumPlayersInZone();
    if (numPlayersInZone != 0)
    {
        std::stringstream ss;
        ss << "Players in zone: " << numPlayersInZone;

        EQ_DrawText(ss.str().c_str(), x, y, 0xFFFFFFFF, fontSize);
        y += fontHeight;
    }

    if (g_freeCameraIsEnabled == true)
    {
        EQ_DrawText("Free Camera is on.", x, y, 0xFFFF0000, fontSize);
        y += fontHeight;
    }

    if (g_speedHackIsEnabled == true)
    {
        std::stringstream ss;
        ss << "Speed: " << g_speedHackModifier;

        EQ_DrawText(ss.str().c_str(), x, y, 0xFFFFFFFF, fontSize);
        y += fontHeight;
    }

    if (EQ_IsAutoAttackEnabled() == true)
    {
        EQ_DrawText("Auto attack is on.", x, y, 0xFFFF0000, fontSize);
        y += fontHeight;
    }

    if (EQ_IsAutoFireEnabled() == true)
    {
        EQ_DrawText("Auto fire is on.", x, y, 0xFFFF0000, fontSize);
        y += fontHeight;
    }

    if (g_alwaysAttackIsEnabled == true)
    {
        EQ_DrawText("Always attack is on.", x, y, 0xFFFF0000, fontSize);
        y += fontHeight;
    }

    if (g_alwaysHotbuttonIsEnabled == true)
    {
        std::stringstream ss;
        ss << "Always Hotbutton: " << (g_alwaysHotbuttonIndex + 1) << " (" << (g_alwaysHotbuttonDelay / 1000) << "s)";

        EQ_DrawText(ss.str().c_str(), x, y, 0xFF00FF00, fontSize);
        y += fontHeight;
    }

    if (g_combatHotbuttonIsEnabled == true)
    {
        std::stringstream ss;
        ss << "Combat Hotbutton: " << (g_combatHotbuttonIndex + 1) << " (" << (g_combatHotbuttonDelay / 1000) << "s)";

        EQ_DrawText(ss.str().c_str(), x, y, 0xFF00FF00, fontSize);
        y += fontHeight;
    }

    if (g_targetBeepIsEnabled == true)
    {
        std::stringstream ss;
        ss << "Target Beep: " << g_targetBeepName << " (" << (g_targetBeepDelay / 1000) << "s)";

        EQ_DrawText(ss.str().c_str(), x, y, 0xFF00FF00, fontSize);
        y += fontHeight;
    }

    if (g_spawnBeepIsEnabled == true)
    {
        std::stringstream ss;
        ss << "Spawn Beep: " << g_spawnBeepName;

        EQ_DrawText(ss.str().c_str(), x, y, 0xFF00FF00, fontSize);
        y += fontHeight;
    }

    if (g_autoLootIsEnabled == true)
    {
        EQ_DrawText("Auto Loot is on.", x, y, 0xFFFF0000, fontSize);
        y += fontHeight;

        for (auto& itemName : g_autoLootList)
        {
            std::stringstream ss;
            ss << "AL: " << itemName;

            EQ_DrawText(ss.str().c_str(), x, y, 0xFFFFFFFF, fontSize);
            y += fontHeight;
        }
    }

    if (g_debugIsEnabled == true)
    {
        std::stringstream ss;
        ss << "ESP Num Draw Text: " << g_espNumDrawText;

        EQ_DrawText(ss.str().c_str(), x, y, 0xFFFFFFFF, fontSize);
        y += fontHeight;
    }
}

void EQAPP_DrawHealthBar(int x, int y, char* spawnName, int spawnHealth)
{
    if (strlen(spawnName) == 0)
    {
        return;
    }

    unsigned int fontSize   = 2;
    unsigned int fontHeight = EQ_GetFontHeight(fontSize);

    float healthBarWidth = g_healthBarsWidth;

    if (spawnHealth != 0)
    {
        float healthPercent = 100.0f / spawnHealth;

        healthBarWidth = g_healthBarsWidth / healthPercent;
    }

    EQ_DrawQuad((float)x, (float)y, healthBarWidth, (float)fontHeight, 0xC8FF0000);

    EQ_DrawText(spawnName, x, y, 0xFFFFFFFF, fontSize);

    if (spawnHealth < 100)
    {
        std::stringstream ss;

        if (spawnHealth == 0)
        {
            ss << "Dead";
        }
        else
        {
            ss << spawnHealth << "%";
        }

        EQ_DrawText(ss.str().c_str(), (x + (int)(g_healthBarsWidth * 0.5f)), y, 0xFFFFFFFF, fontSize);
    }
}

void EQAPP_DoHealthBars()
{
    if (g_healthBarsIsEnabled == false)
    {
        return;
    }

    unsigned int fontSize   = 2;
    unsigned int fontHeight = EQ_GetFontHeight(fontSize);

    unsigned int numHealthBars = 0;

    unsigned int x = g_healthBarsX;
    unsigned int y = g_healthBarsY;

    EQ_DrawQuad((float)x, (float)y, g_healthBarsWidth, g_healthBarsHeight, EQ_TOOLTIP_COLOR);

    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);

    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        if (numHealthBars >= g_healthBarsMax)
        {
            return;
        }

        if (g_healthBarsShowGroupIsEnabled == false)
        {
            if (EQ_IsSpawnInGroup(spawn) == true)
            {
                spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
                continue;
            }
        }

        if (spawn == playerSpawn)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        int spawnLevel = EQ_ReadMemory<BYTE>(spawn + 0x315);

        if (spawnLevel < 1 || spawnLevel > 100)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        LONG spawnHealth = EQ_ReadMemory<LONG>(spawn + 0x1F8);

        if (spawnHealth <= 0)
        {
            spawnHealth = 0;
        }

        if (spawnHealth > 100)
        {
            spawnHealth = 100;
        }

        bool bFoundSpawn = false;

        char spawnName[0x40] = {0};
        memcpy(spawnName, (LPVOID)(spawn + 0xE4), sizeof(spawnName));

        for (auto& namedSpawn : g_namedSpawnsList)
        {
            if (strstr(spawnName, namedSpawn.c_str()) != NULL)
            {
                numHealthBars++;

                EQAPP_DrawHealthBar(x, y, spawnName, spawnHealth);
                y += fontHeight;

                bFoundSpawn = true;
                break;
            }
        }

        if (bFoundSpawn == true)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        for (auto& healthBarSpawnName : g_healthBarsSpawnNamesList)
        {
            if (strstr(spawnName, healthBarSpawnName.c_str()) != NULL)
            {
                numHealthBars++;

                EQAPP_DrawHealthBar(x, y, spawnName, spawnHealth);
                y += fontHeight;

                bFoundSpawn = true;
                break;
            }
        }

        if (bFoundSpawn == true)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125);

        if (spawnType == EQ_SPAWN_TYPE_PLAYER)
        {
            numHealthBars++;

            EQAPP_DrawHealthBar(x, y, spawnName, spawnHealth);
            y += fontHeight;
        }

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    if (numHealthBars > 0)
    {
        g_healthBarsHeight = (float)(numHealthBars * fontHeight);
    }
}

void EQAPP_DoWindowTitles()
{
    if (g_bExit == 1)
    {
        return;
    }

    HWND window = EQ_ReadMemory<HWND>(EQ_WINDOW_HWND);
    if (window == NULL)
    {
        return;
    }

    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);
    if (playerSpawn == NULL)
    {
        SetWindowTextA(window, EQ_STRING_WINDOW_TITLE);

        SetConsoleTitleA(g_applicationName);
    }
    else
    {
        char playerName[0x40] = {0};
        memcpy(playerName, (LPVOID)(playerSpawn + 0xE4), sizeof(playerName));

        std::stringstream ss;
        ss << EQ_STRING_WINDOW_TITLE << ": " << playerName;

        SetWindowTextA(window, ss.str().c_str());

        ss.str(std::string());
        ss.clear();

        ss << g_applicationName << ": " << playerName;

        SetConsoleTitleA(ss.str().c_str());
    }
}

void EQAPP_DoTargetBeep()
{
    if (g_targetBeepIsEnabled == false)
    {
        return;
    }

    if (EQ_HasTimePassed(g_targetBeepTimer, g_targetBeepDelay) == false)
    {
        return;
    }

    DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);
    if (targetSpawn == NULL)
    {
        return;
    }

    char spawnName[0x40] = {0};
    memcpy(spawnName, (LPVOID)(targetSpawn + 0xE4), sizeof(spawnName));

    if (strlen(spawnName) > 1 && g_targetBeepName.size() != 0)
    {
        if (strstr(spawnName, g_targetBeepName.c_str()) != NULL)
        {
            MessageBeep(0);
        }
    }
}

void EQAPP_DoSpawnBeep(DWORD spawnInfo)
{
    if (spawnInfo == NULL)
    {
        return;
    }

    char spawnName[0x40] = {0};
    memcpy(spawnName, (LPVOID)(spawnInfo + 0xE4), sizeof(spawnName));

    for (auto& namedSpawn : g_namedSpawnsList)
    {
        if (strstr(spawnName, namedSpawn.c_str()) != NULL)
        {
            MessageBeep(0);
            break;
        }
    }

    if (g_spawnBeepIsEnabled == true)
    {
        if (strstr(spawnName, g_spawnBeepName.c_str()) != NULL)
        {
            MessageBeep(0);
        }
    }
}

void EQAPP_DoSpawnMessage(DWORD spawnInfo)
{
    if (spawnInfo == NULL)
    {
        return;
    }

    if (EQ_IsInGame() == false)
    {
        return;
    }

    char spawnName[0x40] = {0};
    memcpy(spawnName, (LPVOID)(spawnInfo + 0xE4), sizeof(spawnName));

    int spawnType = EQ_ReadMemory<BYTE>(spawnInfo + 0x125);

    if (spawnType == EQ_SPAWN_TYPE_PLAYER)
    {
        if (strlen(spawnName) > 2)
        {
            std::stringstream ss;
            ss << spawnName << " has entered the zone.";

            EQAPP_DoOnScreenTextAddToList(ss.str());
        }
    }
    else if (spawnType == EQ_SPAWN_TYPE_NPC)
    {
        for (auto& namedSpawn : g_namedSpawnsList)
        {
            if (strstr(spawnName, namedSpawn.c_str()) != NULL)
            {
                std::stringstream ss;
                ss << spawnName << " has spawned.";

                EQAPP_DoOnScreenTextAddToList(ss.str());

                break;
            }
        }
    }
}

void EQAPP_DoDespawnMessage(DWORD spawnInfo)
{
    if (spawnInfo == NULL)
    {
        return;
    }

    if (EQ_IsInGame() == false)
    {
        return;
    }

    char spawnName[0x40] = {0};
    memcpy(spawnName, (LPVOID)(spawnInfo + 0xE4), sizeof(spawnName));

    int spawnType = EQ_ReadMemory<BYTE>(spawnInfo + 0x125);

    if (spawnType == EQ_SPAWN_TYPE_PLAYER)
    {
        if (EQ_IsSpawnInGroup(spawnInfo) == false)
        {
            if (strlen(spawnName) > 2)
            {
                std::stringstream ss;
                ss << spawnName << " has left the zone.";

                EQAPP_DoOnScreenTextAddToList(ss.str());
            }
        }
    }
    else if (spawnType == EQ_SPAWN_TYPE_NPC)
    {
        for (auto& namedSpawn : g_namedSpawnsList)
        {
            if (strstr(spawnName, namedSpawn.c_str()) != NULL)
            {
                std::stringstream ss;
                ss << spawnName << " has despawned.";

                EQAPP_DoOnScreenTextAddToList(ss.str());

                break;
            }
        }
    }
}

void EQAPP_DoLineToTarget()
{
    if (g_lineToTargetIsEnabled == false)
    {
        return;
    }

    DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);

    if (targetSpawn == NULL)
    {
        return;
    }

    FLOAT targetY = EQ_ReadMemory<FLOAT>(targetSpawn + 0x64);
    FLOAT targetX = EQ_ReadMemory<FLOAT>(targetSpawn + 0x68);
    FLOAT targetZ = EQ_ReadMemory<FLOAT>(targetSpawn + 0x6C);

    int screenX = -1;
    int screenY = -1;
    bool result = EQ_WorldSpaceToScreenSpace(targetX, targetY, targetZ, screenX, screenY);
    if (result == true)
    {
        DWORD windowWidth  = EQ_ReadMemory<DWORD>(EQ_WINDOW_WIDTH);
        DWORD windowHeight = EQ_ReadMemory<DWORD>(EQ_WINDOW_HEIGHT);

        EQ_DrawLine((float)(windowWidth / 2), (float)windowHeight, 0.0f, (float)screenX, (float)screenY, 0.0f, 0xFFFF00FF);
    }
}

void EQAPP_DoHideCorpseLooted()
{
    if (g_hideCorpseLootedIsEnabled == false)
    {
        return;
    }

    EQ_SetHideCorpseLooted(true);
}

void EQAPP_DoAlwaysAttack()
{
    if (g_alwaysAttackIsEnabled == false)
    {
        return;
    }

    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);
    if (playerSpawn == NULL)
    {
        return;
    }

    DWORD standingState = EQ_ReadMemory<BYTE>(playerSpawn + 0x279);
    if (standingState != EQ_STANDING_STATE_STANDING)
    {
        return;
    }

    DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);
    if (targetSpawn == NULL)
    {
        return;
    }

    int spawnType = EQ_ReadMemory<BYTE>(targetSpawn + 0x125);
    if (spawnType != EQ_SPAWN_TYPE_NPC)
    {
        return;
    }

    EQ_WriteMemory<BYTE>(EQ_BOOL_AUTO_ATTACK, 0x01);
}

void EQAPP_DoCombatHotbutton()
{
    if (g_combatHotbuttonIsEnabled == false)
    {
        return;
    }

    if (EQ_IsAutoAttackEnabled() == false)
    {
        return;
    }

    DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);
    if (targetSpawn == NULL)
    {
        return;
    }

    int spawnType = EQ_ReadMemory<BYTE>(targetSpawn + 0x125);
    if (spawnType != EQ_SPAWN_TYPE_NPC)
    {
        return;
    }

    if (EQ_IsPlayerCastingSpell() == true)
    {
        return;
    }

    if (EQ_HasTimePassed(g_combatHotbuttonTimer, g_combatHotbuttonDelay) == false)
    {
        return;
    }

    EQ_CHotButtonWnd->DoHotButton(g_combatHotbuttonIndex, 0);
}

void EQAPP_DoAlwaysHotbutton()
{
    if (g_alwaysHotbuttonIsEnabled == false)
    {
        return;
    }

    if (EQ_HasTimePassed(g_alwaysHotbuttonTimer, g_alwaysHotbuttonDelay) == false)
    {
        return;
    }

    EQ_CHotButtonWnd->DoHotButton(g_alwaysHotbuttonIndex, 0);
}

void EQAPP_DoMaxSwimming()
{
    DWORD charInfo2 = EQ_GetCharInfo2();
    if (charInfo2 == NULL)
    {
        return;
    }

    EQ_WriteMemory<BYTE>(charInfo2 + 0x144C, 0xFA); // 250
}

void EQAPP_DoHeight()
{
    if (g_heightIsEnabled == false)
    {
        return;
    }

    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);
    if (playerSpawn == NULL)
    {
        return;
    }

    float height = EQ_ReadMemory<FLOAT>(playerSpawn + 0x13C);

    if (height > g_heightMaximum)
    {
        ((EQPlayer*)playerSpawn)->ChangeHeight(g_heightMaximum);
    }
}

void EQAPP_DoDrawDistance()
{
    if (g_drawDistanceIsEnabled == false)
    {
        return;
    }

    if (EQ_HasTimePassed(g_drawDistanceTimer, g_drawDistanceDelay) == false)
    {
        return;
    }

    EQ_SetDrawDistance(g_drawDistance);
}

void EQAPP_RemoveMapLabels()
{
    EQ_MapWindow_RemoveLabel(g_mapLabelsData);
}

void EQAPP_DoMapLabels()
{
    if (g_mapLabelsIsEnabled == false)
    {
        return;
    }

    if (EQ_HasTimePassed(g_mapLabelsTimer, g_mapLabelsDelay) == false)
    {
        return;
    }

    EQAPP_RemoveMapLabels();

    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);

    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        if (spawn == playerSpawn)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        int spawnLevel = EQ_ReadMemory<BYTE>(spawn + 0x315);

        if (spawnLevel < 1 || spawnLevel > 100)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125);

        if (g_mapLabelsFilterIsEnabled == true && spawnType == EQ_SPAWN_TYPE_NPC)
        {
            char spawnName[0x40] = {0};
            memcpy(spawnName, (LPVOID)(spawn + 0xA4), sizeof(spawnName));

            if (strstr(spawnName, g_mapLabelsFilterName.c_str()) == NULL)
            {
                spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
                continue;
            }
        }

        FLOAT spawnY = EQ_ReadMemory<FLOAT>(spawn + 0x64);
        FLOAT spawnX = EQ_ReadMemory<FLOAT>(spawn + 0x68);
        FLOAT spawnZ = EQ_ReadMemory<FLOAT>(spawn + 0x6C);

        EQMAPLABEL mapLabel;

        EQXYZ location;
        location.X = -spawnX;
        location.Y = -spawnY;
        location.Z = spawnZ;

        mapLabel.Location = location;

        EQARGBCOLOR color;
        color.A = 255; 

        if (spawnType == EQ_SPAWN_TYPE_PLAYER)
        {
            color.R = 255;
            color.G = 0;
            color.B = 0;
        }
        else if (spawnType == EQ_SPAWN_TYPE_NPC)
        {
            color.R = 0;
            color.G = 255;
            color.B = 255;
        }
        else if (spawnType == EQ_SPAWN_TYPE_PLAYER_CORPSE)
        {
            color.R = 255;
            color.G = 128;
            color.B = 0;
        }
        else if (spawnType == EQ_SPAWN_TYPE_NPC_CORPSE)
        {
            color.R = 255;
            color.G = 255;
            color.B = 0;
        }

        mapLabel.Color = color;

        mapLabel.Size = 2;

        mapLabel.Label = (PCHAR)(spawn + 0xE4); // spawn name

        mapLabel.Layer = 3;
        mapLabel.Width = 20;
        mapLabel.Height = 12;

        mapLabel.Data = g_mapLabelsData;

        EQ_MapWindow_AddLabel(&mapLabel);

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    EQ_UpdateMap();
}

void EQAPP_DrawSpawnSkeleton(DWORD spawnInfo, DWORD argbColor)
{
    if (spawnInfo == NULL)
    {
        return;
    }

    //char spawnName[0x40] = {0};
    //memcpy(spawnName, (LPVOID)(spawnInfo + 0xA4), sizeof(spawnName));

    //EQAPP_Log("----------", 0);
    //EQAPP_Log(spawnName, 0);

    DWORD actorInfo = EQ_ReadMemory<DWORD>(spawnInfo + 0xF84);
    if (actorInfo == NULL)
    {
        return;
    }

    DWORD modelInfo = EQ_ReadMemory<DWORD>(actorInfo + 0x11C);
    if (modelInfo == NULL)
    {
        return;
    }

    DWORD boneList = EQ_ReadMemory<DWORD>(modelInfo + 0x58);
    if (boneList == NULL)
    {
        return;
    }

    DWORD firstBoneInfo = EQ_ReadMemory<DWORD>(boneList + 0x00);
    if (firstBoneInfo == NULL)
    {
        return;
    }

    DWORD firstBoneMagicNumber = EQ_ReadMemory<DWORD>(firstBoneInfo + 0x00);
    if (firstBoneMagicNumber == NULL)
    {
        return;
    }

    for (size_t i = 0; i < 64; i++)
    {
        //EQAPP_Log("boneIndex", i);

        DWORD boneInfo = EQ_ReadMemory<DWORD>(boneList + (i * 4));
        if (boneInfo == NULL)
        {
            continue;
        }

        if ((boneInfo > (boneList + 0x4000)) || (boneInfo < boneList))
        {
            continue;
        }

        if (boneInfo == firstBoneMagicNumber)
        {
            break;
        }

        //EQAPP_Log("boneInfo", boneInfo);

        DWORD boneMagicNumber = EQ_ReadMemory<DWORD>(boneInfo + 0x00);
        if (boneMagicNumber == NULL)
        {
            continue;
        }

        if (boneMagicNumber != firstBoneMagicNumber)
        {
            break;
        }

        //EQAPP_Log("boneMagicNumber", boneMagicNumber);

        FLOAT boneY = EQ_ReadMemory<FLOAT>(boneInfo + 0x90);
        FLOAT boneX = EQ_ReadMemory<FLOAT>(boneInfo + 0x94);
        FLOAT boneZ = EQ_ReadMemory<FLOAT>(boneInfo + 0x98);

        //EQAPP_Log("boneY", boneY);
        //EQAPP_Log("boneX", boneX);
        //EQAPP_Log("boneZ", boneZ);

        if (boneY == 0.0f || boneX == 0.0f || boneZ == 0.0f)
        {
            continue;
        }

        int screenX = -1;
        int screenY = -1;
        bool result = EQ_WorldSpaceToScreenSpace(boneX, boneY, boneZ, screenX, screenY);
        if (result == false)
        {
            continue;
        }

        //EQ_DrawText(".", screenX, screenY, argbColor, fontSize);
        EQ_DrawLine((float)screenX, (float)screenY, 1.0f, (float)screenX, (float)(screenY + 1.0f), 1.0f, argbColor);
    }
}

void EQAPP_UpdateEspSpawnList()
{
    if (EQ_HasTimePassed(g_espSpawnListTimer, g_espSpawnListDelay) == false)
    {
        return;
    }

    g_espSpawnList.clear();

    unsigned int numNpcCorpse = 0;

    DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);

    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);

    FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
    FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
    FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

    // creature spawn
    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        if (spawn == playerSpawn)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        EQAPPESPSPAWN espSpawn;
        espSpawn.spawnInfo = spawn;

        memcpy(espSpawn.name, (LPVOID)(spawn + 0xE4), sizeof(espSpawn.name));
        memcpy(espSpawn.lastName, (LPVOID)(spawn + 0x38), sizeof(espSpawn.lastName));

        espSpawn.y = EQ_ReadMemory<FLOAT>(spawn + 0x64);
        espSpawn.x = EQ_ReadMemory<FLOAT>(spawn + 0x68);
        espSpawn.z = EQ_ReadMemory<FLOAT>(spawn + 0x6C);

        espSpawn.distance = EQ_CalculateDistance3d(playerX, playerY, playerZ, espSpawn.x, espSpawn.y, espSpawn.z);

        espSpawn.level = EQ_ReadMemory<BYTE>(spawn + 0x315);

        if (espSpawn.level < 1 || espSpawn.level > 100)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        espSpawn.type = EQ_ReadMemory<BYTE>(spawn + 0x125);
        espSpawn._class = EQ_ReadMemory<BYTE>(spawn + 0xE68);
        espSpawn.guildId = EQ_ReadMemory<DWORD>(spawn + 0x30C);

        espSpawn.isHoldingPrimary   = EQ_ReadMemory<BYTE>(spawn + 0x10E0);
        espSpawn.isHoldingSecondary = EQ_ReadMemory<BYTE>(spawn + 0x10E4);

        espSpawn.isPet = EQ_ReadMemory<DWORD>(spawn + 0x260);
        espSpawn.isLfg = EQ_ReadMemory<BYTE>(spawn + 0x1F1);
        espSpawn.isPvp = EQ_ReadMemory<BYTE>(spawn + 0x2BC);
        espSpawn.isTrader = EQ_ReadMemory<DWORD>(spawn + 0x228);
        espSpawn.isAfk = EQ_ReadMemory<DWORD>(spawn + 0x284);
        espSpawn.isGm = EQ_ReadMemory<BYTE>(spawn + 0x4DA);

        if (spawn == targetSpawn)
        {
            espSpawn.isTarget = true;
        }

        if (g_findIsEnabled == true && g_findSpawnName.size() != 0)
        {
            if (strstr(espSpawn.name, g_findSpawnName.c_str()) != NULL)
            {
                espSpawn.showAtAnyDistance = true;
                espSpawn.isFindSpawn = true;
            }
        }

        if (espSpawn.type == EQ_SPAWN_TYPE_NPC && espSpawn.isPet == 0)
        {
            for (auto& namedSpawn : g_namedSpawnsList)
            {
                if (strstr(espSpawn.name, namedSpawn.c_str()) != NULL)
                {
                    espSpawn.showAtAnyDistance = true;
                    break;
                }
            }
        }

        if (espSpawn.type == EQ_SPAWN_TYPE_NPC_CORPSE)
        {
            numNpcCorpse++;

            if (espSpawn.isTarget == false && espSpawn.showAtAnyDistance == false)
            {
                if (numNpcCorpse > g_espNpcCorpseMax)
                {
                    spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
                    continue;
                }
            }
        }

        if (espSpawn.type == EQ_SPAWN_TYPE_NPC && espSpawn.distance > g_espSpawnDistance && espSpawn.showAtAnyDistance == false)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        if (espSpawn.distance > (g_espSpawnDistance * 0.5f))
        {
            espSpawn.useSmallFont = true;
        }

        g_espSpawnList.push_back(espSpawn);

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }
}

void EQAPP_DoEsp()
{
    if (g_espIsEnabled == false)
    {
        return;
    }

    EQAPP_UpdateEspSpawnList();

    g_espNumDrawText = 0;

    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);

    FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
    FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
    FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

    if (g_espSpawnIsEnabled == true)
    {
        for (auto& spawn : g_espSpawnList)
        {
            if (spawn.spawnInfo == NULL)
            {
                continue;
            }

            int fontSize = 3;

            if (spawn.useSmallFont == true)
            {
                fontSize = 2;
            }

            if (spawn.showAtAnyDistance == true || spawn.isFindSpawn == true)
            {
                fontSize = 5;
            }

            int screenX = -1;
            int screenY = -1;
            bool result = EQ_WorldSpaceToScreenSpace(spawn.x, spawn.y, spawn.z, screenX, screenY);
            if (result == false)
            {
                continue;
            }

            int textColor = 0xFFFFFFFF; // white

            if (spawn.type == EQ_SPAWN_TYPE_PLAYER)
            {
                textColor = 0xFFFF0000; // red
            }
            else if (spawn.type == EQ_SPAWN_TYPE_NPC)
            {
                textColor = 0xFF00FFFF; // cyan
            }
            else if (spawn.type == EQ_SPAWN_TYPE_PLAYER_CORPSE)
            {
                textColor = 0xFFFF8000; // orange
            }
            else if (spawn.type == EQ_SPAWN_TYPE_NPC_CORPSE)
            {
                textColor = 0xFFFFFF00; // yellow
            }

            if (spawn.type == EQ_SPAWN_TYPE_PLAYER)
            {
                if (EQ_IsSpawnInGroup(spawn.spawnInfo) == true)
                {
                    textColor = 0xFF00FF00; // green
                }

                if (strlen(spawn.name) < 2)
                {
                    textColor = 0xFF808080;
                }
            }

            if (spawn.showAtAnyDistance == true)
            {
                textColor = 0xFF00FF80; // greenish blue
            }

            if (spawn.isTarget == true || spawn.isGm == 1)
            {
                textColor = 0xFFFF00FF; // pink
            }

            if (g_espSkeletonIsEnabled == true)
            {
                if (spawn.distance < g_espSkeletonDistance)
                {
                    if (EQ_CastRay(playerSpawn, spawn.y, spawn.x, spawn.z) == 0)
                    {
                        EQAPP_DrawSpawnSkeleton(spawn.spawnInfo, textColor);
                    }
                }
            }

            std::stringstream ss;
            ss << "+ " << spawn.name;

            if (spawn.type == EQ_SPAWN_TYPE_PLAYER_CORPSE || spawn.type == EQ_SPAWN_TYPE_NPC_CORPSE)
            {
                ss << " corpse";
            }

            ss << " L" << spawn.level;

            if (spawn._class > EQ_CLASS_UNKNOWN && spawn._class < EQ_CLASS_BANKER)
            {
                if (spawn.type == EQ_SPAWN_TYPE_PLAYER || (spawn.type == EQ_SPAWN_TYPE_NPC && EQ_IsKeyControlPressed() == true))
                {
                    const char* spawnClassThreeLetterCode = EQ_CEverQuest->GetClassThreeLetterCode(spawn._class);

                    ss << " " << spawnClassThreeLetterCode;
                }
            }

            ss << " (" << (int)spawn.distance << ")";

            if (spawn.isLfg == 1)
            {
                ss << "  LFG";
            }

            if (spawn.isPvp == 1)
            {
                ss << "  PVP";
            }

            if (spawn.isTrader == 1)
            {
                ss << "  TRADER";
            }

            if (spawn.isAfk == 1)
            {
                ss << "  AFK";
            }

            if (spawn.isGm == 1)
            {
                ss << "  *GM*";
            }

            EQ_DrawText(ss.str().c_str(), screenX, screenY, textColor, fontSize);
            g_espNumDrawText++;

            unsigned int fontHeight = EQ_GetFontHeight(fontSize);

            unsigned int textOffsetY = fontHeight;

            if (spawn.type == EQ_SPAWN_TYPE_PLAYER && EQ_IsKeyControlPressed() == true)
            {
                if (spawn.guildId > 0 && spawn.guildId < EQ_NUM_GUILDS)
                {
                    const char* spawnGuildName = EQ_EQ_Guilds.GetGuildNameById(spawn.guildId);

                    std::stringstream ss;
                    ss << "<" << spawnGuildName << ">";

                    EQ_DrawText(ss.str().c_str(), screenX, screenY + textOffsetY, textColor, fontSize);
                    textOffsetY = textOffsetY + fontHeight;
                    g_espNumDrawText++;
                }
            }
            else if (spawn.type == EQ_SPAWN_TYPE_NPC)
            {
                if (strlen(spawn.lastName) > 0)
                {
                    std::stringstream ss;
                    ss << "(" << spawn.lastName << ")";

                    EQ_DrawText(ss.str().c_str(), screenX, screenY + textOffsetY, textColor, fontSize);
                    textOffsetY = textOffsetY + fontHeight;
                    g_espNumDrawText++;
                }

                if (EQ_IsKeyControlPressed() == true)
                {
                    const char* spawnBodyTypeDescription = EQ_CEverQuest->GetBodyTypeDesc(spawn.spawnInfo + 0x128);

                    std::stringstream ss;
                    ss << "[" << spawnBodyTypeDescription << "]";

                    EQ_DrawText(ss.str().c_str(), screenX, screenY + textOffsetY, textColor, fontSize);
                    textOffsetY = textOffsetY + fontHeight;
                    g_espNumDrawText++;

                    if (spawn.isHoldingPrimary != 0 && spawn.isHoldingSecondary != 0)
                    {
                        EQ_DrawText("Holding Primary & Secondary", screenX, screenY + textOffsetY, textColor, fontSize);
                        textOffsetY = textOffsetY + fontHeight;
                        g_espNumDrawText++;
                    }
                    else if (spawn.isHoldingPrimary != 0)
                    {
                        EQ_DrawText("Holding Primary", screenX, screenY + textOffsetY, textColor, fontSize);
                        textOffsetY = textOffsetY + fontHeight;
                        g_espNumDrawText++;
                    }
                    else if (spawn.isHoldingSecondary != 0)
                    {
                        EQ_DrawText("Holding Secondary", screenX, screenY + textOffsetY, textColor, fontSize);
                        textOffsetY = textOffsetY + fontHeight;
                        g_espNumDrawText++;
                    }
                }
            }

            if (g_spawnCastSpellIsEnabled == true)
            {
                for (auto& spawnCastSpell : g_spawnCastSpellList)
                {
                    if (spawnCastSpell->spawnInfo == spawn.spawnInfo)
                    {
                        std::stringstream ss;
                        ss << "(" << spawnCastSpell->spellName << ")";

                        EQ_DrawText(ss.str().c_str(), screenX, screenY + textOffsetY, textColor, fontSize);
                        textOffsetY = textOffsetY + fontHeight;
                        g_espNumDrawText++;

                        break;
                    }
                }
            }

            if (g_findIsEnabled == true && spawn.isFindSpawn == true)
            {
                DWORD windowWidth  = EQ_ReadMemory<DWORD>(EQ_WINDOW_WIDTH);
                DWORD windowHeight = EQ_ReadMemory<DWORD>(EQ_WINDOW_HEIGHT);

                EQ_DrawLine((float)(windowWidth / 2), (float)windowHeight, 0.0f, (float)screenX, (float)screenY, 0.0f, 0xFF00FF80);
            }
        }
    }

    if (g_espGroundSpawnIsEnabled == true)
    {
        // ground spawn
        DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_GROUND_SPAWN_INFO);

        while (spawn)
        {
            FLOAT spawnZ = EQ_ReadMemory<FLOAT>(spawn + 0x2C);
            FLOAT spawnX = EQ_ReadMemory<FLOAT>(spawn + 0x30);
            FLOAT spawnY = EQ_ReadMemory<FLOAT>(spawn + 0x34);

            float spawnDistance = EQ_CalculateDistance3d(playerX, playerY, playerZ, spawnX, spawnY, spawnZ);

            if (spawnDistance > g_espGroundSpawnDistance)
            {
                spawn = EQ_ReadMemory<DWORD>(spawn + 0x04); // next
                continue;
            }

            int screenX = -1;
            int screenY = -1;
            bool result = EQ_WorldSpaceToScreenSpace(spawnX, spawnY, spawnZ, screenX, screenY);
            if (result == false)
            {
                spawn = EQ_ReadMemory<DWORD>(spawn + 0x04); // next
                continue;
            }

            char spawnName[0x18] = {0};
            memcpy(spawnName, (LPVOID)(spawn + 0x38), sizeof(spawnName));

            std::string spawnNameEx = spawnName;

            auto find = EQ_KEYVALUE_ACTOR_DEFINITIONS.find(spawnName);
            if (find != EQ_KEYVALUE_ACTOR_DEFINITIONS.end())
            {
                spawnNameEx = find->second;

                std::stringstream ss;
                ss << "+ " << spawnNameEx << " (" << (int)spawnDistance << ")";

                EQ_DrawText(ss.str().c_str(), screenX, screenY, 0xFFFFFFFF, 2);
                g_espNumDrawText++;
            }

            spawn = EQ_ReadMemory<DWORD>(spawn + 0x04); // next
        }
    }

    if (g_espDoorIsEnabled == true)
    {
        DWORD switchManager = EQ_ReadMemory<DWORD>(EQ_POINTER_SWITCH_MANAGER);
        if (switchManager != NULL)
        {
            DWORD numDoors = EQ_ReadMemory<DWORD>(switchManager + 0x00);
            if (numDoors != 0)
            {
                for (size_t i = 0; i < numDoors; i++)
                {
                    DWORD doorInfo = EQ_ReadMemory<DWORD>(switchManager + 0x04 + (i * 4));
                    if (doorInfo == NULL)
                    {
                        continue;
                    }

                    FLOAT doorY = EQ_ReadMemory<FLOAT>(doorInfo + 0x44);
                    FLOAT doorX = EQ_ReadMemory<FLOAT>(doorInfo + 0x48);
                    FLOAT doorZ = EQ_ReadMemory<FLOAT>(doorInfo + 0x4C);

                    float doorDistance = EQ_CalculateDistance3d(playerX, playerY, playerZ, doorX, doorY, doorZ);
                    if (doorDistance > g_espDoorDistance)
                    {
                        continue;
                    }

                    int screenX = -1;
                    int screenY = -1;
                    bool result = EQ_WorldSpaceToScreenSpace(doorX, doorY, doorZ, screenX, screenY);
                    if (result == false)
                    {
                        continue;
                    }

                    char doorName[32] = {0};
                    memcpy(doorName, (LPVOID)(doorInfo + 0x06), sizeof(doorName));

                    std::stringstream ss;
                    ss << "+ " << doorName << " [#" << i + 1 << "]" << " (" << (int)doorDistance << ")";

                    EQ_DrawText(ss.str().c_str(), screenX, screenY, 0xFFFFFFFF, 2);
                    g_espNumDrawText++;
                }
            }
        }
    }

    if (g_espZoneObjectIsEnabled == true)
    {
        DWORD pointer1 = EQ_ReadMemory<DWORD>(0x00B112C0);
        if (pointer1 != NULL)
        {
            DWORD pointer2 = EQ_ReadMemory<DWORD>(pointer1 + 0x94);
            if (pointer2 != NULL)
            {
                DWORD zoneObject = EQ_ReadMemory<DWORD>(pointer2 + 0x5C);

                while (zoneObject)
                {
                    DWORD zoneObject0x0C = EQ_ReadMemory<DWORD>(zoneObject + 0x0C);

                    if (zoneObject0x0C == 2)
                    {
                        zoneObject = EQ_ReadMemory<DWORD>(zoneObject + 0x04); // next
                        continue;
                    }

                    DWORD zoneObject0x2C = EQ_ReadMemory<BYTE>(zoneObject + 0x2C);
                    DWORD zoneObject0x2D = EQ_ReadMemory<BYTE>(zoneObject + 0x2D);

                    // skip player and npc models
                    if (zoneObject0x2C == 1 && zoneObject0x2D == 0)
                    {
                        zoneObject = EQ_ReadMemory<DWORD>(zoneObject + 0x04); // next
                        continue;
                    }

                    FLOAT zoneObjectY = EQ_ReadMemory<FLOAT>(zoneObject + 0x30);
                    FLOAT zoneObjectX = EQ_ReadMemory<FLOAT>(zoneObject + 0x34);
                    FLOAT zoneObjectZ = EQ_ReadMemory<FLOAT>(zoneObject + 0x38);

                    float zoneObjectDistance = EQ_CalculateDistance3d(playerX, playerY, playerZ, zoneObjectX, zoneObjectY, zoneObjectZ);
                    if (zoneObjectDistance > g_espZoneObjectDistance)
                    {
                        zoneObject = EQ_ReadMemory<DWORD>(zoneObject + 0x04); // next
                        continue;
                    }

                    int screenX = -1;
                    int screenY = -1;
                    bool result = EQ_WorldSpaceToScreenSpace(zoneObjectX, zoneObjectY, zoneObjectZ, screenX, screenY);
                    if (result == false)
                    {
                        zoneObject = EQ_ReadMemory<DWORD>(zoneObject + 0x04); // next
                        continue;
                    }

                    std::string zoneObjectName = "ZONEOBJECT";

                    DWORD zoneObject0x14 = EQ_ReadMemory<DWORD>(zoneObject + 0x14);
                    if (zoneObject0x14 != NULL)
                    {
                        DWORD zoneObject0x14x18 = EQ_ReadMemory<DWORD>(zoneObject0x14 + 0x18);
                        if (zoneObject0x14x18 != NULL)
                        {
                            PCHAR zoneObjectNamePointer = EQ_ReadMemory<PCHAR>(zoneObject0x14x18 + 0x08);
                            if (zoneObjectNamePointer != NULL)
                            {
                                zoneObjectName = std::string(zoneObjectNamePointer);
                            }
                        }
                    }

                    std::stringstream ss;
                    ss << "+ " << zoneObjectName << " (" << (int)zoneObjectDistance << ")";
                    EQ_DrawText(ss.str().c_str(), screenX, screenY, 0xFFFFFFFF, 2);

                    unsigned int offsetY = 13;

                    std::stringstream ssY;
                    ssY << "Y: " << zoneObjectY;
                    EQ_DrawText(ssY.str().c_str(), screenX, screenY + offsetY, 0xFFFFFFFF, 2);
                    offsetY += 13;

                    std::stringstream ssX;
                    ssX << "X: " << zoneObjectX;
                    EQ_DrawText(ssX.str().c_str(), screenX, screenY + offsetY, 0xFFFFFFFF, 2);
                    offsetY += 13;

                    std::stringstream ssZ;
                    ssZ << "Z: " << zoneObjectZ;
                    EQ_DrawText(ssZ.str().c_str(), screenX, screenY + offsetY, 0xFFFFFFFF, 2);
                    offsetY += 13;

                    zoneObject = EQ_ReadMemory<DWORD>(zoneObject + 0x04); // next
                }
            }
        }
    }

    // locator
    if (g_locatorIsEnabled == true)
    {
        int screenX = -1;
        int screenY = -1;
        bool result = EQ_WorldSpaceToScreenSpace(g_locatorX, g_locatorY, g_locatorZ, screenX, screenY);
        if (result == true)
        {
            float distance = EQ_CalculateDistance3d(playerX, playerY, playerZ, g_locatorX, g_locatorY, g_locatorZ);

            std::stringstream ss;
            ss << "+ Locator (" << (int)distance << ")";

            EQ_DrawText(ss.str().c_str(), screenX, screenY, 0xFFFFFFFF, 5);
            g_espNumDrawText++;
        }
    }
}

void EQAPP_DoSpawnCastSpell()
{
    if (g_spawnCastSpellIsEnabled == false)
    {
        return;
    }

    if (g_spawnCastSpellList.size() == 0)
    {
        return;
    }

    DWORD currentTime = EQ_GetTimer();

    for (auto spawnCastSpellList_it = g_spawnCastSpellList.begin(); spawnCastSpellList_it != g_spawnCastSpellList.end(); spawnCastSpellList_it++)
    {
        PEQAPPSPAWNCASTSPELL spawnCastSpell = spawnCastSpellList_it->get();
        if (spawnCastSpell == nullptr)
        {
            continue;
        }

        DWORD delay = spawnCastSpell->spellCastTime;
        if (delay < g_spawnCastSpellDelay)
        {
            delay = g_spawnCastSpellDelay;
        }

        DWORD spawnStandingState = EQ_ReadMemory<BYTE>(spawnCastSpell->spawnInfo + 0x278);

        if
        (
            ((currentTime - spawnCastSpell->timer) > delay) || spawnStandingState == EQ_STANDING_STATE_DUCKING
        )
        {
            spawnCastSpellList_it = g_spawnCastSpellList.erase(spawnCastSpellList_it);
            spawnCastSpellList_it--;
            continue;
        }
    }
}

void EQAPP_DoSpawnCastSpellAddToList(DWORD spawnInfo, DWORD spellId, DWORD spellCastTime)
{
    if (spawnInfo == NULL)
    {
        return;
    }

    std::string spellName = EQ_GetSpellNameById(spellId);
    if (spellName.size() == 0)
    {
        return;
    }

    for (auto& spawnCastSpell : g_spawnCastSpellList)
    {
        if (spawnCastSpell->spawnInfo == spawnInfo)
        {
            spawnCastSpell->spellName     = spellName;
            spawnCastSpell->spellCastTime = spellCastTime;
            spawnCastSpell->timer         = EQ_GetTimer();
            return;
        }
    }

    SPEQAPPSPAWNCASTSPELL spawnCastSpell = std::make_shared<EQAPPSPAWNCASTSPELL>();

    spawnCastSpell->spawnInfo     = spawnInfo;
    spawnCastSpell->spellName     = spellName;
    spawnCastSpell->spellCastTime = spellCastTime;
    spawnCastSpell->timer         = EQ_GetTimer();

    g_spawnCastSpellList.push_back(std::move(spawnCastSpell));
}

void EQAPP_DoSpawnList(const char* filterSpawnName)
{
    if (EQ_IsInGame() == false)
    {
        return;
    }

    EQAPP_CoutSaveFlags();

    std::cout << "Spawn List:";
    if (filterSpawnName != NULL)
    {
        std::cout << " " << filterSpawnName;
    }
    std::cout << std::endl;

    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);

    FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
    FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
    FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        char spawnName[0x40] = {0};
        memcpy(spawnName, (LPVOID)(spawn + 0xE4), sizeof(spawnName));

        char spawnLastName[0x20] = {0};
        memcpy(spawnLastName, (LPVOID)(spawn + 0x38), sizeof(spawnLastName));

        int spawnLevel = EQ_ReadMemory<BYTE>(spawn + 0x315);

        if (spawnLevel < 1 || spawnLevel > 100)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        if (filterSpawnName != NULL && strlen(spawnName) > 0)
        {
            if (strstr(spawnName, filterSpawnName) == NULL)
            {
                spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
                continue;
            }
        }

        int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125);

        int spawnClass = EQ_ReadMemory<BYTE>(spawn + 0xE68);

        if (spawnType == EQ_SPAWN_TYPE_PLAYER)
        {
            std::cout << "[PLAYER]    ";
        }
        else if (spawnType == EQ_SPAWN_TYPE_NPC)
        {
            std::cout << "[NPC]       ";
        }
        else
        {
            std::cout << "[CORPSE]    ";
        }

        std::cout << "L" << std::setfill('0') << std::setw(2) << spawnLevel << " ";

        if (spawnClass > EQ_CLASS_UNKNOWN && spawnClass < EQ_CLASS_BANKER && spawnType == EQ_SPAWN_TYPE_PLAYER)
        {
            const char* spawnClassThreeLetterCode = EQ_CEverQuest->GetClassThreeLetterCode(spawnClass);

            std::cout << spawnClassThreeLetterCode << "    ";
        }
        else
        {
            std::cout << "       ";
        }

        std::cout << spawnName << " ";

        if (strlen(spawnLastName) != 0)
        {
            if (spawnType == EQ_SPAWN_TYPE_NPC || spawnType == EQ_SPAWN_TYPE_NPC_CORPSE)
            {
                std::cout << "(";
            }

            std::cout << spawnLastName;

            if (spawnType == EQ_SPAWN_TYPE_NPC || spawnType == EQ_SPAWN_TYPE_NPC_CORPSE)
            {
                std::cout << ")";
            }
        }

        if (spawnType == EQ_SPAWN_TYPE_PLAYER)
        {
            int spawnIsLfg = EQ_ReadMemory<BYTE>(spawn + 0x1F1);
            if (spawnIsLfg == 1)
            {
                std::cout << "  LFG";
            }

            int spawnIsPvp = EQ_ReadMemory<BYTE>(spawn + 0x2BC);
            if (spawnIsPvp == 1)
            {
                std::cout << "  PVP";
            }

            int spawnIsTrader = EQ_ReadMemory<DWORD>(spawn + 0x228);
            if (spawnIsTrader == 1)
            {
                std::cout << "  TRADER";
            }

            int spawnIsAfk = EQ_ReadMemory<DWORD>(spawn + 0x284);
            if (spawnIsAfk == 1)
            {
                std::cout << "  AFK";
            }

            int spawnIsGm = EQ_ReadMemory<BYTE>(spawn + 0x4DA);
            if (spawnIsGm == 1)
            {
                std::cout << "  *GM*";
            }
        }

        if (g_debugIsEnabled == true && playerSpawn != NULL)
        {
            FLOAT spawnY = EQ_ReadMemory<FLOAT>(spawn + 0x64);
            FLOAT spawnX = EQ_ReadMemory<FLOAT>(spawn + 0x68);
            FLOAT spawnZ = EQ_ReadMemory<FLOAT>(spawn + 0x6C);

            float spawnDistance = EQ_CalculateDistance3d(playerX, playerY, playerZ, spawnX, spawnY, spawnZ);

            std::cout << "    (Distance: " << (int)spawnDistance << ")";
        }

        std::cout << std::endl;

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    EQAPP_CoutRestoreFlags();
}

void EQAPP_DoAutoLoot()
{
    if (g_autoLootIsEnabled == false || g_autoLootList.size() == 0)
    {
        return;
    }

    if (EQ_HasTimePassed(g_autoLootTimer, g_autoLootDelay) == false)
    {
        return;
    }

    for (auto& itemName : g_autoLootList)
    {
        bool result = EQ_LootItemByName(itemName.c_str());
    }
}

void EQAPP_DoBankList(const char* filterItemName)
{
    if (EQ_IsInGame() == false)
    {
        return;
    }

    DWORD charInfo = EQ_ReadMemory<DWORD>(EQ_POINTER_CHAR_INFO);
    if (charInfo == NULL)
    {
        return;
    }

    EQAPP_CoutSaveFlags();

    std::cout << "Bank List:";
    if (filterItemName != NULL)
    {
        std::cout << " " << filterItemName;
    }
    std::cout << std::endl;

    for (size_t i = 0; i < EQ_NUM_BANK_SLOTS; i++)
    {
        DWORD itemInfo = EQ_ReadMemory<DWORD>(charInfo + (0x1128 + (i * 4)));
        if (itemInfo == NULL)
        {
            continue;
        }

        DWORD itemCount = EQ_ReadMemory<DWORD>(itemInfo + 0xD0);

        PCHAR itemName = EQ_ReadMemory<PCHAR>(itemInfo + 0xB8);
        if (itemName != NULL)
        {
            if (filterItemName != NULL)
            {
                DWORD itemSubInfo = EQ_ReadMemory<DWORD>(itemInfo + 0xB8);
                if (itemSubInfo == NULL)
                {
                    continue;
                }

                DWORD itemSlots = EQ_ReadMemory<BYTE>(itemSubInfo + 0x475);
                if (itemSlots == 0)
                {
                    if (strstr(itemName, filterItemName) == NULL)
                    {
                        continue;
                    }
                }
            }

            std::cout << std::setfill('0') << std::setw(2) << i + 1;

            // shared bank slots
            if (i == 24 || i == 25)
            {
                std::cout << "(Listsold)";
            }

            std::cout << ": ";

            if (itemCount > 1)
            {
                std::cout << itemCount << " x ";
            }

            std::cout << itemName << std::endl;
        }

        for (size_t j = 0; j < EQ_NUM_CONTAINER_SLOTS; j++)
        {
            DWORD containerItemInfo = EQ_ReadMemory<DWORD>(itemInfo + (0x0C + (j * 4)));
            if (containerItemInfo == NULL)
            {
                continue;
            }

            DWORD containerItemCount = EQ_ReadMemory<DWORD>(containerItemInfo + 0xD0);

            PCHAR containerItemName = EQ_ReadMemory<PCHAR>(containerItemInfo + 0xB8);
            if (containerItemName != NULL)
            {
                if (filterItemName != NULL)
                {
                    if (strstr(containerItemName, filterItemName) == NULL)
                    {
                        continue;
                    }
                }

                std::cout << std::setfill('0') << std::setw(2) << i + 1 << "-" << std::setfill('0') << std::setw(2) << j + 1 << ": ";

                if (containerItemCount > 1)
                {
                    std::cout << containerItemCount << " x ";
                }

                std::cout << containerItemName << std::endl;
            }
        }
    }

    EQAPP_CoutRestoreFlags();
}

void EQAPP_DoInventoryList(const char* filterItemName)
{
    if (EQ_IsInGame() == false)
    {
        return;
    }

    DWORD charInfo2 = EQ_GetCharInfo2();
    if (charInfo2 == NULL)
    {
        return;
    }

    EQAPP_CoutSaveFlags();

    std::cout << "Inventory List:";
    if (filterItemName != NULL)
    {
        std::cout << " " << filterItemName;
    }
    std::cout << std::endl;

    for (size_t i = 0; i < EQ_NUM_INVENTORY_SLOTS; i++)
    {
        std::string slotName = EQ_LIST_EQUIPMENT_SLOT_NAMES.at(i);

        DWORD itemInfo = EQ_ReadMemory<DWORD>(charInfo2 + (0x10 + (i * 4)));
        if (itemInfo == NULL)
        {
            continue;
        }

        DWORD itemCount = EQ_ReadMemory<DWORD>(itemInfo + 0xD0);

        PCHAR itemName = EQ_ReadMemory<PCHAR>(itemInfo + 0xB8);
        if (itemName != NULL)
        {
            if (filterItemName != NULL)
            {
                DWORD itemSubInfo = EQ_ReadMemory<DWORD>(itemInfo + 0xB8);
                if (itemSubInfo == NULL)
                {
                    continue;
                }

                DWORD itemSlots = EQ_ReadMemory<BYTE>(itemSubInfo + 0x475);
                if (itemSlots == 0)
                {
                    if (strstr(itemName, filterItemName) == NULL)
                    {
                        continue;
                    }
                }
            }

            std::cout << std::setfill('0') << std::setw(2) << i + 1 << "(" << slotName << "): ";

            if (itemCount > 1)
            {
                std::cout << itemCount << " x ";
            }

            std::cout << itemName << std::endl;
        }

        for (size_t j = 0; j < EQ_NUM_CONTAINER_SLOTS; j++)
        {
            DWORD containerItemInfo = EQ_ReadMemory<DWORD>(itemInfo + (0x0C + (j * 4)));
            if (containerItemInfo == NULL)
            {
                continue;
            }

            DWORD containerItemCount = EQ_ReadMemory<DWORD>(containerItemInfo + 0xD0);

            PCHAR containerItemName = EQ_ReadMemory<PCHAR>(containerItemInfo + 0xB8);
            if (containerItemName != NULL)
            {
                if (filterItemName != NULL)
                {
                    if (strstr(containerItemName, filterItemName) == NULL)
                    {
                        continue;
                    }
                }

                std::cout << std::setfill('0') << std::setw(2) << i + 1 << "-" << std::setfill('0') << std::setw(2) << j + 1 << ": ";

                if (containerItemCount > 1)
                {
                    std::cout << containerItemCount << " x ";
                }

                std::cout << containerItemName << std::endl;
            }
        }
    }

    EQAPP_CoutRestoreFlags();
}

void EQAPP_WriteCharacterFile()
{
    DWORD charInfo = EQ_ReadMemory<DWORD>(EQ_POINTER_CHAR_INFO);
    if (charInfo == NULL)
    {
        return;
    }

    DWORD charInfo2 = EQ_GetCharInfo2();
    if (charInfo2 == NULL)
    {
        return;
    }

    char characterName[0x40] = {0};
    memcpy(characterName, (LPVOID)(charInfo + 0xF210), sizeof(characterName));

    std::stringstream filePath;
    filePath << "eqapp/characters/" << characterName << ".txt";

    std::cout << "Writing character file: " << filePath.str() << std::endl;

    std::fstream file;
    file.open(filePath.str().c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

    file << "Name: " << characterName << std::endl << std::endl;

    file << "---------- BANK ----------" << std::endl << std::endl;

    DWORD bankPlatinum = EQ_ReadMemory<DWORD>(charInfo + 0xF5A4);
    DWORD bankGold     = EQ_ReadMemory<DWORD>(charInfo + 0xF5A8);
    DWORD bankSilver   = EQ_ReadMemory<DWORD>(charInfo + 0xF5AC);
    DWORD bankCopper   = EQ_ReadMemory<DWORD>(charInfo + 0xF5B0);

    file << "Platinum: " << bankPlatinum << std::endl;
    file << "Gold:     " << bankGold     << std::endl;
    file << "Silver:   " << bankSilver   << std::endl;
    file << "Copper:   " << bankCopper   << std::endl;

    file << std::endl;

    for (size_t i = 0; i < EQ_NUM_BANK_SLOTS; i++)
    {
        DWORD itemInfo = EQ_ReadMemory<DWORD>(charInfo + (0x1128 + (i * 4)));
        if (itemInfo == NULL)
        {
            file << std::setfill('0') << std::setw(2) << i + 1 << ": EMPTY" << std::endl;
            continue;
        }

        DWORD itemCount = EQ_ReadMemory<DWORD>(itemInfo + 0xD0);

        PCHAR itemName = EQ_ReadMemory<PCHAR>(itemInfo + 0xB8);
        if (itemName != NULL)
        {
            file << std::setfill('0') << std::setw(2) << i + 1 << ": ";

            if (itemCount > 1)
            {
                file << itemCount << " x ";
            }

            file << itemName << std::endl;
        }

        for (size_t j = 0; j < EQ_NUM_CONTAINER_SLOTS; j++)
        {
            DWORD containerItemInfo = EQ_ReadMemory<DWORD>(itemInfo + (0x0C + (j * 4)));
            if (containerItemInfo == NULL)
            {
                //file << std::setfill('0') << std::setw(2) << i + 1 << "-" << std::setfill('0') << std::setw(2) << j + 1 << ": EMPTY" << std::endl;
                continue;
            }

            DWORD containerItemCount = EQ_ReadMemory<DWORD>(containerItemInfo + 0xD0);

            PCHAR containerItemName = EQ_ReadMemory<PCHAR>(containerItemInfo + 0xB8);
            if (containerItemName != NULL)
            {
                file << std::setfill('0') << std::setw(2) << i + 1 << "-" << std::setfill('0') << std::setw(2) << j + 1 << ": ";

                if (containerItemCount > 1)
                {
                    file << containerItemCount << " x ";
                }

                file << containerItemName << std::endl;
            }
        }
    }

    file << std::endl;

    file << "---------- INVENTORY ----------" << std::endl << std::endl;

    DWORD inventoryPlatinum = EQ_ReadMemory<DWORD>(charInfo2 + 0x1654);
    DWORD inventoryGold     = EQ_ReadMemory<DWORD>(charInfo2 + 0x1658);
    DWORD inventorySilver   = EQ_ReadMemory<DWORD>(charInfo2 + 0x165C);
    DWORD inventoryCopper   = EQ_ReadMemory<DWORD>(charInfo2 + 0x1660);

    file << "Platinum: " << inventoryPlatinum << std::endl;
    file << "Gold:     " << inventoryGold     << std::endl;
    file << "Silver:   " << inventorySilver   << std::endl;
    file << "Copper:   " << inventoryCopper   << std::endl;

    file << std::endl;

    for (size_t i = 0; i < EQ_NUM_INVENTORY_SLOTS; i++)
    {
        std::string slotName = EQ_LIST_EQUIPMENT_SLOT_NAMES.at(i);

        DWORD itemInfo = EQ_ReadMemory<DWORD>(charInfo2 + (0x10 + (i * 4)));
        if (itemInfo == NULL)
        {
            file << std::setfill('0') << std::setw(2) << i + 1 << "(" << slotName << "): EMPTY" << std::endl;
            continue;
        }

        DWORD itemCount = EQ_ReadMemory<DWORD>(itemInfo + 0xD0);

        PCHAR itemName = EQ_ReadMemory<PCHAR>(itemInfo + 0xB8);
        if (itemName != NULL)
        {
            file << std::setfill('0') << std::setw(2) << i + 1 << "(" << slotName << "): ";

            if (itemCount > 1)
            {
                file << itemCount << " x ";
            }

            file << itemName << std::endl;
        }

        for (size_t j = 0; j < EQ_NUM_CONTAINER_SLOTS; j++)
        {
            DWORD containerItemInfo = EQ_ReadMemory<DWORD>(itemInfo + (0x0C + (j * 4)));
            if (containerItemInfo == NULL)
            {
                //file << std::setfill('0') << std::setw(2) << i + 1 << "-" << std::setfill('0') << std::setw(2) << j + 1 << ": EMPTY" << std::endl;
                continue;
            }

            DWORD containerItemCount = EQ_ReadMemory<DWORD>(containerItemInfo + 0xD0);

            PCHAR containerItemName = EQ_ReadMemory<PCHAR>(containerItemInfo + 0xB8);
            if (containerItemName != NULL)
            {
                file << std::setfill('0') << std::setw(2) << i + 1 << "-" << std::setfill('0') << std::setw(2) << j + 1 << ": ";

                if (containerItemCount > 1)
                {
                    file << containerItemCount << " x ";
                }

                file << containerItemName << std::endl;
            }
        }
    }

    file << std::endl;

    file.close();
}

void EQAPP_WriteCharacterFileJson()
{
    DWORD charInfo = EQ_ReadMemory<DWORD>(EQ_POINTER_CHAR_INFO);
    if (charInfo == NULL)
    {
        return;
    }

    DWORD charInfo2 = EQ_GetCharInfo2();
    if (charInfo2 == NULL)
    {
        return;
    }

    char characterName[0x40] = {0};
    memcpy(characterName, (LPVOID)(charInfo + 0xF210), sizeof(characterName));

    std::stringstream filePath;
    filePath << "eqapp/characters/" << characterName << ".json";

    std::cout << "Writing character file: " << filePath.str() << std::endl;

    json_t* j_root = json_object();

    json_object_set_new(j_root, "name", json_string(characterName));

    json_t* j_bank_root = json_object();
    json_object_set_new(j_root, "bank", j_bank_root);

    DWORD bankPlatinum = EQ_ReadMemory<DWORD>(charInfo + 0xF5A4);
    DWORD bankGold     = EQ_ReadMemory<DWORD>(charInfo + 0xF5A8);
    DWORD bankSilver   = EQ_ReadMemory<DWORD>(charInfo + 0xF5AC);
    DWORD bankCopper   = EQ_ReadMemory<DWORD>(charInfo + 0xF5B0);

    json_t* j_bank_currency_node = json_pack("{s: i, s: i, s: i, s: i}", "platinum", bankPlatinum, "gold", bankGold, "silver", bankSilver, "copper", bankCopper);
    json_object_set_new(j_bank_root, "currency", j_bank_currency_node);

    json_t* j_bank_items_root = json_array();
    json_object_set_new(j_bank_root, "items", j_bank_items_root);

    for (size_t i = 0; i < EQ_NUM_BANK_SLOTS; i++)
    {
        json_t* j_bank_item_node = json_object();

        DWORD itemInfo = EQ_ReadMemory<DWORD>(charInfo + (0x1128 + (i * 4)));
        if (itemInfo == NULL)
        {
            j_bank_item_node = json_pack("{s: i, s: s, s: i, s: i, s: i}", "slot", i + 1, "name", "(empty)", "count", -1, "id", -1, "icon", -1);
            json_array_append_new(j_bank_items_root, j_bank_item_node);
            continue;
        }

        PCHAR itemName = EQ_ReadMemory<PCHAR>(itemInfo + 0xB8);
        if (itemName != NULL)
        {
            DWORD itemCount = EQ_ReadMemory<DWORD>(itemInfo + 0xD0);

            DWORD itemSubInfo = EQ_ReadMemory<DWORD>(itemInfo + 0xB8);

            DWORD itemId   = EQ_ReadMemory<DWORD>(itemSubInfo + 0xD0);
            DWORD itemIcon = EQ_ReadMemory<DWORD>(itemSubInfo + 0xDC);

            j_bank_item_node = json_pack("{s: i, s: s, s: i, s: i, s: i}", "slot", i + 1, "name", itemName, "count", itemCount, "id", itemId, "icon", itemIcon);
            json_array_append_new(j_bank_items_root, j_bank_item_node);
        }

        json_t* j_bank_item_node_container_items_root = json_array();
        json_object_set_new(j_bank_item_node, "containerItems", j_bank_item_node_container_items_root);

        for (size_t j = 0; j < EQ_NUM_CONTAINER_SLOTS; j++)
        {
            json_t* j_bank_container_item_node = json_object();

            DWORD containerItemInfo = EQ_ReadMemory<DWORD>(itemInfo + (0x0C + (j * 4)));
            if (containerItemInfo == NULL)
            {
                j_bank_container_item_node = json_pack("{s: i, s: s, s: i, s: i, s: i}", "slot", j + 1, "name", "(empty)", "count", -1, "id", -1, "icon", -1);
                json_array_append_new(j_bank_item_node_container_items_root, j_bank_container_item_node);
                continue;
            }

            PCHAR containerItemName = EQ_ReadMemory<PCHAR>(containerItemInfo + 0xB8);
            if (containerItemName != NULL)
            {
                DWORD containerItemCount = EQ_ReadMemory<DWORD>(containerItemInfo + 0xD0);

                DWORD containerItemSubInfo = EQ_ReadMemory<DWORD>(containerItemInfo + 0xB8);

                DWORD containerItemId   = EQ_ReadMemory<DWORD>(containerItemSubInfo + 0xD0);
                DWORD containerItemIcon = EQ_ReadMemory<DWORD>(containerItemSubInfo + 0xDC);

                j_bank_container_item_node = json_pack("{s: i, s: s, s: i, s: i, s: i}", "slot", j + 1, "name", containerItemName, "count", containerItemCount, "id", containerItemId, "icon", containerItemIcon);
                json_array_append_new(j_bank_item_node_container_items_root, j_bank_container_item_node);
            }
        }
    }

    json_t* j_inventory_root = json_object();
    json_object_set_new(j_root, "inventory", j_inventory_root);

    DWORD inventoryPlatinum = EQ_ReadMemory<DWORD>(charInfo2 + 0x1654);
    DWORD inventoryGold     = EQ_ReadMemory<DWORD>(charInfo2 + 0x1658);
    DWORD inventorySilver   = EQ_ReadMemory<DWORD>(charInfo2 + 0x165C);
    DWORD inventoryCopper   = EQ_ReadMemory<DWORD>(charInfo2 + 0x1660);

    json_t* j_inventory_currency_node = json_pack("{s: i, s: i, s: i, s: i}", "platinum", inventoryPlatinum, "gold", inventoryGold, "silver", inventorySilver, "copper", inventoryCopper);
    json_object_set_new(j_inventory_root, "currency", j_inventory_currency_node);

    json_t* j_inventory_items_root = json_array();
    json_object_set_new(j_inventory_root, "items", j_inventory_items_root);

    for (size_t i = 0; i < EQ_NUM_INVENTORY_SLOTS; i++)
    {
        json_t* j_inventory_item_node = json_object();

        std::string slotName = EQ_LIST_EQUIPMENT_SLOT_NAMES.at(i);

        DWORD itemInfo = EQ_ReadMemory<DWORD>(charInfo2 + (0x10 + (i * 4)));
        if (itemInfo == NULL)
        {
            j_inventory_item_node = json_pack("{s: i, s: s, s: s, s: i, s: i, s: i}", "slot", i + 1, "slotName", slotName.c_str(), "name", "(empty)", "count", -1, "id", -1, "icon", -1);
            json_array_append_new(j_inventory_items_root, j_inventory_item_node);
            continue;
        }

        PCHAR itemName = EQ_ReadMemory<PCHAR>(itemInfo + 0xB8);
        if (itemName != NULL)
        {
            DWORD itemCount = EQ_ReadMemory<DWORD>(itemInfo + 0xD0);

            DWORD itemSubInfo = EQ_ReadMemory<DWORD>(itemInfo + 0xB8);

            DWORD itemId   = EQ_ReadMemory<DWORD>(itemSubInfo + 0xD0);
            DWORD itemIcon = EQ_ReadMemory<DWORD>(itemSubInfo + 0xDC);

            j_inventory_item_node = json_pack("{s: i, s: s, s: s, s: i, s: i, s: i}", "slot", i + 1, "slotName", slotName.c_str(), "name", itemName, "count", itemCount, "id", itemId, "icon", itemIcon);
            json_array_append_new(j_inventory_items_root, j_inventory_item_node);
        }

        json_t* j_inventory_item_node_container_items_root = json_array();
        json_object_set_new(j_inventory_item_node, "containerItems", j_inventory_item_node_container_items_root);

        for (size_t j = 0; j < EQ_NUM_CONTAINER_SLOTS; j++)
        {
            json_t* j_inventory_container_item_node = json_object();

            DWORD containerItemInfo = EQ_ReadMemory<DWORD>(itemInfo + (0x0C + (j * 4)));
            if (containerItemInfo == NULL)
            {
                j_inventory_container_item_node = json_pack("{s: i, s: s, s: i, s: i, s: i}", "slot", j + 1, "name", "(empty)", "count", -1, "id", -1, "icon", -1);
                json_array_append_new(j_inventory_item_node_container_items_root, j_inventory_container_item_node);
                continue;
            }

            PCHAR containerItemName = EQ_ReadMemory<PCHAR>(containerItemInfo + 0xB8);
            if (containerItemName != NULL)
            {
                DWORD containerItemCount = EQ_ReadMemory<DWORD>(containerItemInfo + 0xD0);

                DWORD containerItemSubInfo = EQ_ReadMemory<DWORD>(containerItemInfo + 0xB8);

                DWORD containerItemId   = EQ_ReadMemory<DWORD>(containerItemSubInfo + 0xD0);
                DWORD containerItemIcon = EQ_ReadMemory<DWORD>(containerItemSubInfo + 0xDC);

                j_inventory_container_item_node = json_pack("{s: i, s: s, s: i, s: i, s: i}", "slot", j + 1, "name", containerItemName, "count", containerItemCount, "id", containerItemId, "icon", containerItemIcon);
                json_array_append_new(j_inventory_item_node_container_items_root, j_inventory_container_item_node);
            }
        }
    }

    json_dump_file(j_root, filePath.str().c_str(), JSON_PRESERVE_ORDER | JSON_INDENT(4));

    json_decref(j_root);
}

void EQAPP_DoOpenAllDoors()
{
    DWORD switchManager = EQ_ReadMemory<DWORD>(EQ_POINTER_SWITCH_MANAGER);
    if (switchManager == NULL)
    {
        return;
    }

    DWORD numDoors = EQ_ReadMemory<DWORD>(switchManager + 0x00);
    if (numDoors == 0)
    {
        return;
    }

    for (size_t i = 0; i < numDoors; i++)
    {
        DWORD doorInfo = EQ_ReadMemory<DWORD>(switchManager + 0x04 + (i * 4));
        if (doorInfo == NULL)
        {
            continue;
        }

        ((EQSwitch*)doorInfo)->ChangeState(EQ_DOOR_STATE_OPEN, 0, 0);
    }
}

void EQAPP_ToggleFreeCamera(bool b)
{
    // updates camera coordinates
    // EQGraphicsDX9.DLL+591A - 89 8F BC000000        - mov [edi+000000BC],ecx
    // EQGraphicsDX9.DLL+5923 - 89 97 C0000000        - mov [edi+000000C0],edx
    // EQGraphicsDX9.DLL+592C - 89 87 C4000000        - mov [edi+000000C4],eax

    DWORD baseAddress = EQ_ReadMemory<DWORD>(EQ_POINTER_GRAPHICS_DLL_BASE_ADDRESS);
    if (baseAddress == NULL)
    {
        return;
    }

    DWORD address = 0;
    DWORD oldProtection = 0;
    DWORD tempProtection = 0;

    if (b == true)
    {
        // Camera Y
        address = baseAddress + 0x591A;
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x90\x90\x90\x90\x90\x90", 6, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, oldProtection, &tempProtection);

        // Camera X
        address = baseAddress + 0x5923;
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x90\x90\x90\x90\x90\x90", 6, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, oldProtection, &tempProtection);

        // Camera Z
        address = baseAddress + 0x592C;
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x90\x90\x90\x90\x90\x90", 6, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, oldProtection, &tempProtection);

        EQ_WriteMemory<DWORD>(EQ_CAMERA_VIEW, EQ_CAMERA_VIEW_FIRST_PERSON);
    }
    else
    {
        // Camera Y
        address = baseAddress + 0x591A;
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x89\x8F\xBC\x00\x00\x00", 6, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, oldProtection, &tempProtection);

        // Camera X
        address = baseAddress + 0x5923;
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x89\x97\xC0\x00\x00\x00", 6, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, oldProtection, &tempProtection);

        // Camera Z
        address = baseAddress + 0x592C;
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, PAGE_EXECUTE_READWRITE, &oldProtection);
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, "\x89\x87\xC4\x00\x00\x00", 6, 0);
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, 6, oldProtection, &tempProtection);
    }

    g_freeCameraIsEnabled = b;
}

void EQAPP_DoFreeCameraKeys()
{
    if (g_freeCameraIsEnabled == false)
    {
        return;
    }

    if (EQAPP_IsForegroundWindowCurrentProcessId() == false)
    {
        return;
    }

    DWORD cameraData = EQ_GetCameraData();
    if (cameraData == NULL)
    {
        return;
    }

    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        FLOAT cameraY = EQ_ReadMemory<FLOAT>(cameraData + 0xBC);
        FLOAT cameraX = EQ_ReadMemory<FLOAT>(cameraData + 0xC0);
        FLOAT cameraZ = EQ_ReadMemory<FLOAT>(cameraData + 0xC4);

        FLOAT cameraHeading = EQ_ReadMemory<FLOAT>(cameraData + 0xB0);
        FLOAT cameraPitch   = EQ_ReadMemory<FLOAT>(cameraData + 0xB4);

        cameraHeading = cameraHeading - 128.0f;

        if (cameraHeading < 0.0f)
        {
            cameraHeading = cameraHeading + 512.0f;
        }

        float cameraHeadingRadians = cameraHeading * EQ_PI / 256.0f;

        cameraPitch = (float)(cameraPitch + fabs(EQ_CAMERA_PITCH_DEFAULT)); // remove negative offset from camera pitch

        float cameraPitchRadians = cameraPitch * EQ_PI / 256.0f;

        float addY = sinf(cameraHeadingRadians);
        float addX = cosf(cameraHeadingRadians);
        float addZ = sinf(cameraPitchRadians);

        if (fabs(cameraPitch) < 90.0f)
        {
            cameraY -= addY * g_freeCameraMultiplier;
            cameraX += addX * g_freeCameraMultiplier;
        }

        cameraZ += addZ * g_freeCameraMultiplier;

        EQ_WriteMemory<FLOAT>(cameraData + 0xBC, cameraY);
        EQ_WriteMemory<FLOAT>(cameraData + 0xC0, cameraX);
        EQ_WriteMemory<FLOAT>(cameraData + 0xC4, cameraZ);
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        FLOAT cameraY = EQ_ReadMemory<FLOAT>(cameraData + 0xBC);
        FLOAT cameraX = EQ_ReadMemory<FLOAT>(cameraData + 0xC0);
        FLOAT cameraZ = EQ_ReadMemory<FLOAT>(cameraData + 0xC4);

        FLOAT cameraHeading = EQ_ReadMemory<FLOAT>(cameraData + 0xB0);
        FLOAT cameraPitch   = EQ_ReadMemory<FLOAT>(cameraData + 0xB4);

        cameraHeading = cameraHeading - 128.0f;

        if (cameraHeading < 0.0f)
        {
            cameraHeading = cameraHeading + 512.0f;
        }

        float cameraHeadingRadians = cameraHeading * EQ_PI / 256.0f;

        cameraPitch = (float)(cameraPitch + fabs(EQ_CAMERA_PITCH_DEFAULT)); // remove negative offset from camera pitch

        float cameraPitchRadians = cameraPitch * EQ_PI / 256.0f;

        float addY = sinf(cameraHeadingRadians);
        float addX = cosf(cameraHeadingRadians);
        float addZ = sinf(cameraPitchRadians);

        if (fabs(cameraPitch) < 90.0f)
        {
            cameraY += addY * g_freeCameraMultiplier;
            cameraX -= addX * g_freeCameraMultiplier;
        }

        cameraZ -= addZ * g_freeCameraMultiplier;

        EQ_WriteMemory<FLOAT>(cameraData + 0xBC, cameraY);
        EQ_WriteMemory<FLOAT>(cameraData + 0xC0, cameraX);
        EQ_WriteMemory<FLOAT>(cameraData + 0xC4, cameraZ);
    }

    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        FLOAT cameraY = EQ_ReadMemory<FLOAT>(cameraData + 0xBC);
        FLOAT cameraX = EQ_ReadMemory<FLOAT>(cameraData + 0xC0);
        FLOAT cameraZ = EQ_ReadMemory<FLOAT>(cameraData + 0xC4);

        FLOAT cameraHeading = EQ_ReadMemory<FLOAT>(cameraData + 0xB0);
        FLOAT cameraPitch   = EQ_ReadMemory<FLOAT>(cameraData + 0xB4);

        if (cameraHeading < 0.0f)
        {
            cameraHeading = cameraHeading + 512.0f;
        }

        float cameraHeadingRadians = cameraHeading * EQ_PI / 256.0f;

        cameraPitch = (float)(cameraPitch + fabs(EQ_CAMERA_PITCH_DEFAULT)); // remove negative offset from camera pitch

        float cameraPitchRadians = cameraPitch * EQ_PI / 256.0f;

        float addY = sinf(cameraHeadingRadians);
        float addX = cosf(cameraHeadingRadians);
        float addZ = sinf(cameraPitchRadians);

        if (fabs(cameraPitch) < 90.0f)
        {
            cameraY -= addY * g_freeCameraMultiplier;
            cameraX += addX * g_freeCameraMultiplier;
        }

        cameraZ += addZ * g_freeCameraMultiplier;

        EQ_WriteMemory<FLOAT>(cameraData + 0xBC, cameraY);
        EQ_WriteMemory<FLOAT>(cameraData + 0xC0, cameraX);
        EQ_WriteMemory<FLOAT>(cameraData + 0xC4, cameraZ);
    }

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        FLOAT cameraY = EQ_ReadMemory<FLOAT>(cameraData + 0xBC);
        FLOAT cameraX = EQ_ReadMemory<FLOAT>(cameraData + 0xC0);
        FLOAT cameraZ = EQ_ReadMemory<FLOAT>(cameraData + 0xC4);

        FLOAT cameraHeading = EQ_ReadMemory<FLOAT>(cameraData + 0xB0);
        FLOAT cameraPitch   = EQ_ReadMemory<FLOAT>(cameraData + 0xB4);

        cameraHeading = cameraHeading + 256.0f;

        if (cameraHeading > 512.0f)
        {
            cameraHeading = cameraHeading - 512.0f;
        }

        float cameraHeadingRadians = cameraHeading * EQ_PI / 256.0f;

        cameraPitch = (float)(cameraPitch + fabs(EQ_CAMERA_PITCH_DEFAULT)); // remove negative offset from camera pitch

        float cameraPitchRadians = cameraPitch * EQ_PI / 256.0f;

        float addY = sinf(cameraHeadingRadians);
        float addX = cosf(cameraHeadingRadians);
        float addZ = sinf(cameraPitchRadians);

        if (fabs(cameraPitch) < 90.0f)
        {
            cameraY -= addY * g_freeCameraMultiplier;
            cameraX += addX * g_freeCameraMultiplier;
        }

        cameraZ += addZ * g_freeCameraMultiplier;

        EQ_WriteMemory<FLOAT>(cameraData + 0xBC, cameraY);
        EQ_WriteMemory<FLOAT>(cameraData + 0xC0, cameraX);
        EQ_WriteMemory<FLOAT>(cameraData + 0xC4, cameraZ);
    }
}

void EQAPP_DoInterpretCommand(const char* command)
{
    // help
    if (strcmp(command, "//help") == 0)
    {
        for (auto& slashCommand : g_slashCommandsList)
        {
            std::cout << slashCommand << std::endl;
        }

        return;
    }

    // unload
    if (strcmp(command, "//unload") == 0)
    {
        EQAPP_DoUnload();
        return;
    }

    // toggle debug
    if (strcmp(command, "//debug") == 0)
    {
        EQ_ToggleBool(g_debugIsEnabled);

        std::cout << "Debug: " << std::boolalpha << g_debugIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // test
    if (strcmp(command, "//test") == 0)
    {
        std::cout << "test" << std::endl;

        //DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);
        //if (playerSpawn == NULL)
        //{
            //return;
        //}

        //FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
        //FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
        //FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

        //DWORD mapViewMap = EQ_GetMapViewMap();
        //if (mapViewMap == NULL)
        //{
            //return;
        //}

        //((MapViewMap*)mapViewMap)->AddLabel(-playerX, -playerY, playerZ, 0xFFFF0000, 2, "TESTING123");

        return;
    }

    // loot debug
    if (strcmp(command, "//lootdebug") == 0)
    {
        std::cout << "Loot Debug:" << std::endl;

        DWORD lootWindow = EQ_ReadMemory<DWORD>(EQ_POINTER_CLootWnd);
        if (lootWindow == NULL)
        {
            std::cout << "Loot Window is NULL." << std::endl;
            return;
        }

        DWORD lootWindowIsVisible = EQ_ReadMemory<BYTE>(lootWindow + 0x124);
        if (lootWindowIsVisible == 0)
        {
            std::cout << "Loot Window is not open." << std::endl;
            return;
        }

        for (size_t i = 0; i < EQ_NUM_LOOT_WINDOW_SLOTS; i++)
        {
            DWORD itemInfo = EQ_ReadMemory<DWORD>(lootWindow + (0x228 + (i * 4)));
            if (itemInfo == NULL)
            {
                std::cout << i << ": itemInfo == NULL" << std::endl;
                continue;
            }

            PCHAR itemName = EQ_ReadMemory<PCHAR>(itemInfo + 0xB8);
            if (itemName == NULL)
            {
                std::cout << i << ": itemName == NULL" << std::endl;
                continue;
            }

            std::cout << i << ": " << itemName << std::endl;
        }
    }

    // loot
    if (strncmp(command, "//loot ", 7) == 0)
    {
        char commandEx[128];

        char itemName[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), itemName, sizeof(itemName));

        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(itemName);

            bool lootResult = EQ_LootItemByName(itemName);

            std::cout << "Loot Result: " << itemName << ", " << std::boolalpha << lootResult << std::noboolalpha << std::endl;
        }
    }

    // toggle auto loot
    if (strcmp(command, "//autoloot") == 0 || strcmp(command, "//al") == 0)
    {
        EQ_ToggleBool(g_autoLootIsEnabled);

        std::cout << "Auto Loot: " << std::boolalpha << g_autoLootIsEnabled << std::noboolalpha << std::endl;

        if (g_autoLootIsEnabled == false)
        {
            g_autoLootList.clear();
        }

        return;
    }

    // reset auto loot
    if (strcmp(command, "//resetautoloot") == 0 || strcmp(command, "//resetal") == 0 || strcmp(command, "//ral") == 0)
    {
        std::cout << "Auto Loot reset." << std::endl;

        g_autoLootList.clear();

        return;
    }

    // add to auto loot list
    if (strncmp(command, "//autoloot ", 11) == 0 || strncmp(command, "//al ", 5) == 0)
    {
        char commandEx[128];

        char itemName[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), itemName, sizeof(itemName));

        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(itemName);

            g_autoLootList.push_back(itemName);

            std::cout << "Auto Loot added: " << itemName << std::endl;

            g_autoLootIsEnabled = true;
        }
    }

    // cast ray
    if (strcmp(command, "//castray") == 0 || strcmp(command, "//cr") == 0)
    {
        DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);
        DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);

        if (targetSpawn != NULL)
        {
            FLOAT targetY = EQ_ReadMemory<FLOAT>(targetSpawn + 0x64);
            FLOAT targetX = EQ_ReadMemory<FLOAT>(targetSpawn + 0x68);
            FLOAT targetZ = EQ_ReadMemory<FLOAT>(targetSpawn + 0x6C);

            int result = EQ_CastRay(playerSpawn, targetY, targetX, targetZ);

            std::cout << "Cast Ray result: " << result << std::endl;
        }

        return;
    }

    // get melee range
    if (strcmp(command, "//getmeleerange") == 0 || strcmp(command, "//getmr") == 0 || strcmp(command, "//gmr") == 0)
    {
        DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);
        DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);

        if (targetSpawn != NULL)
        {
            FLOAT meleeRange = EQ_get_melee_range(playerSpawn, targetSpawn);

            std::cout << "Melee Range: " << meleeRange << std::endl;
        }

        return;
    }

    // get spell name
    if (strncmp(command, "//getspellname ", 15) == 0 || strncmp(command, "//getsn ", 8) == 0 || strncmp(command, "//gsn ", 6) == 0)
    {
        char commandEx[128];

        int spellId = 0;

        int result = sscanf_s(command, "%s %d", commandEx, sizeof(commandEx), &spellId);

        if (result == 2)
        {
            std::string spellName = EQ_GetSpellNameById(spellId);

            std::cout << "Spell Name By ID: " << spellId << " == " << spellName << std::endl;
        }

        return;
    }

    // get target info
    if (strcmp(command, "//gettargetinfo") == 0 || strcmp(command, "//getti") == 0 || strcmp(command, "//gti") == 0)
    {
        DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);

        if (targetSpawn == NULL)
        {
            std::cout << "No target." << std::endl;
            return;
        }

        std::cout << "Target Info:" << std::endl;

        //

        char spawnName[0x40] = {0};
        memcpy(spawnName, (LPVOID)(targetSpawn + 0xA4), sizeof(spawnName));

        std::cout << "NAME:  " << spawnName;

        int spawnType = EQ_ReadMemory<BYTE>(targetSpawn + 0x125);
        if (spawnType != EQ_SPAWN_TYPE_PLAYER)
        {
            char spawnNameEx[0x40] = {0};
            memcpy(spawnNameEx, (LPVOID)(targetSpawn + 0xE4), sizeof(spawnNameEx));

            std::cout << " (" << spawnNameEx << ")";
        }

        std::cout << std::endl;

        //

        int spawnLevel = EQ_ReadMemory<BYTE>(targetSpawn + 0x315);

        std::cout << "LEVEL: " << spawnLevel << std::endl;

        //

        int spawnGuildId = EQ_ReadMemory<DWORD>(targetSpawn + 0x30C);

        const char* spawnGuildName = EQ_EQ_Guilds.GetGuildNameById(spawnGuildId);

        std::cout << "GUILD: " << spawnGuildName << " (" << spawnGuildId << ")" << std::endl;

        //

        int spawnRace = EQ_ReadMemory<DWORD>(targetSpawn + 0xE64);

        const char* spawnRaceDescription = EQ_CEverQuest->GetRaceDesc(spawnRace);

        std::cout << "RACE:  " << spawnRaceDescription << " (" << spawnRace << ")" << std::endl;

        //

        int spawnClass = EQ_ReadMemory<BYTE>(targetSpawn + 0xE68);

        const char* spawnClassDescription = EQ_CEverQuest->GetClassDesc(spawnClass);

        std::cout << "CLASS: " << spawnClassDescription << " (" << spawnClass << ")" << std::endl;

        //

        int spawnDeity = EQ_ReadMemory<DWORD>(targetSpawn + 0xD4D);

        const char* spawnDeityDescription = EQ_CEverQuest->GetDeityDesc(spawnDeity);

        std::cout << "DEITY: " << spawnDeityDescription << " (" << spawnDeity << ")" << std::endl;

        //

        const char* spawnBodyTypeDescription = EQ_CEverQuest->GetBodyTypeDesc(targetSpawn + 0x128);

        std::cout << "BANE:  " << spawnBodyTypeDescription << std::endl;

        return;
    }

    // bank or money
    if (strcmp(command, "//bank") == 0 || strcmp(command, "//money") == 0)
    {
        DWORD charInfo = EQ_ReadMemory<DWORD>(EQ_POINTER_CHAR_INFO);
        if (charInfo == NULL)
        {
            return;
        }

        DWORD platinum = EQ_ReadMemory<DWORD>(charInfo + 0xF5A4);
        DWORD gold     = EQ_ReadMemory<DWORD>(charInfo + 0xF5A8);
        DWORD silver   = EQ_ReadMemory<DWORD>(charInfo + 0xF5AC);
        DWORD copper   = EQ_ReadMemory<DWORD>(charInfo + 0xF5B0);

        std::cout << "You have " << platinum << "p " << gold << "g " << silver << "s " << copper << "c in the bank." << std::endl;

        return;
    }

    // bank list
    if (strcmp(command, "//banklist") == 0 || strcmp(command, "//bl") == 0)
    {
        EQAPP_DoBankList(NULL);

        return;
    }

    // search bank list
    if (strncmp(command, "//banklist ", 11) == 0 || strncmp(command, "//bl ", 5) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));

        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            EQAPP_DoBankList(name);
        }

        return;
    }

    // inventory list
    if (strcmp(command, "//inventorylist") == 0 || strcmp(command, "//invlist") == 0 || strcmp(command, "//il") == 0)
    {
        EQAPP_DoInventoryList(NULL);

        return;
    }

    // search inventory list
    if (strncmp(command, "//inventorylist ", 16) == 0 || strncmp(command, "//invlist ", 10) == 0 || strncmp(command, "//il ", 5) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));

        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            EQAPP_DoInventoryList(name);
        }

        return;
    }

    // spawn list
    if (strcmp(command, "//spawnlist") == 0 || strcmp(command, "//sl") == 0)
    {
        EQAPP_DoSpawnList(NULL);

        return;
    }

    // search spawn list
    if (strncmp(command, "//spawnlist ", 12) == 0 || strncmp(command, "//sl ", 5) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));

        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            EQAPP_DoSpawnList(name);
        }

        return;
    }

    // toggle esp
    if (strcmp(command, "//esp") == 0)
    {
        EQ_ToggleBool(g_espIsEnabled);

        std::cout << "ESP: " << std::boolalpha << g_espIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // toggle esp spawn
    if (strcmp(command, "//espspawn") == 0 || strcmp(command, "//esps") == 0)
    {
        EQ_ToggleBool(g_espSpawnIsEnabled);

        std::cout << "ESP Spawn: " << std::boolalpha << g_espSpawnIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // toggle esp ground spawn
    if (strcmp(command, "//espgroundspawn") == 0 || strcmp(command, "//espg") == 0)
    {
        EQ_ToggleBool(g_espGroundSpawnIsEnabled);

        std::cout << "ESP Ground Spawn: " << std::boolalpha << g_espGroundSpawnIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // toggle esp door
    if (strcmp(command, "//espdoor") == 0 || strcmp(command, "//espd") == 0)
    {
        EQ_ToggleBool(g_espDoorIsEnabled);

        std::cout << "ESP Door: " << std::boolalpha << g_espDoorIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // toggle esp skeleton
    if (strcmp(command, "//espskeleton") == 0 || strcmp(command, "//espsk") == 0)
    {
        EQ_ToggleBool(g_espSkeletonIsEnabled);

        std::cout << "ESP Skeleton: " << std::boolalpha << g_espSkeletonIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // toggle esp zone object
    if (strcmp(command, "//espzoneobject") == 0 || strcmp(command, "//espzo") == 0)
    {
        EQ_ToggleBool(g_espZoneObjectIsEnabled);

        std::cout << "ESP Zone Object: " << std::boolalpha << g_espZoneObjectIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set esp spawn distance
    if (strncmp(command, "//setespspawndistance ", 22) == 0 || strncmp(command, "//setesps ", 10) == 0 || strncmp(command, "//setesp ", 9) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);

        if (result == 2)
        {
            g_espSpawnDistance = distance;

            std::cout << "ESP Spawn Distance: " << distance << std::endl;
        }

        return;
    }

    // set esp ground spawn distance
    if (strncmp(command, "//setespgroundspawndistance ", 28) == 0 || strncmp(command, "//setespg ", 10) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);

        if (result == 2)
        {
            g_espGroundSpawnDistance = distance;

            std::cout << "ESP Ground Spawn Distance: " << distance << std::endl;
        }

        return;
    }

    // set esp door distance
    if (strncmp(command, "//setespdoordistance ", 21) == 0 || strncmp(command, "//setespd ", 10) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);

        if (result == 2)
        {
            g_espDoorDistance = distance;

            std::cout << "ESP Door Distance: " << distance << std::endl;
        }

        return;
    }

    // set esp skeleton distance
    if (strncmp(command, "//setespskeletondistance ", 25) == 0 || strncmp(command, "//setespsk ", 11) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);

        if (result == 2)
        {
            g_espSkeletonDistance = distance;

            std::cout << "ESP Skeleton Distance: " << distance << std::endl;
        }

        return;
    }

    // set esp zone object distance
    if (strncmp(command, "//setespzoneobjectdistance ", 27) == 0 || strncmp(command, "//setespzo ", 11) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);

        if (result == 2)
        {
            g_espZoneObjectDistance = distance;

            std::cout << "ESP Zone Object Distance: " << distance << std::endl;
        }

        return;
    }

    // toggle speed
    if (strcmp(command, "//speed") == 0)
    {
        EQ_ToggleBool(g_speedHackIsEnabled);

        std::cout << "Speed Hack: " << std::boolalpha << g_speedHackIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set speed sow
    if (strcmp(command, "//setspeed sow") == 0)
    {
        g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_SPIRIT_OF_WOLF;

        std::cout << "Speed Modifier:  Spirit of Wolf" << std::endl;

        return;
    }

    // set speed run1
    if (strcmp(command, "//setspeed run1") == 0)
    {
        g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN1;

        std::cout << "Speed Modifier: AA Run 1" << std::endl;

        return;
    }

    // set speed run2
    if (strcmp(command, "//setspeed run2") == 0)
    {
        g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN2;

        std::cout << "Speed Modifier: AA Run 2" << std::endl;

        return;
    }

    // set speed run3
    if (strcmp(command, "//setspeed run3") == 0)
    {
        g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN3;

        std::cout << "Speed Modifier: AA Run 3" << std::endl;

        return;
    }

    // set speed
    if (strncmp(command, "//setspeed ", 11) == 0 || strncmp(command, "//sets ", 7) == 0 || strncmp(command, "//ss ", 5) == 0)
    {
        char commandEx[128];

        float modifier = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &modifier);

        if (result == 2)
        {
            g_speedHackModifier = modifier;

            std::cout << "Speed Modifier: " << g_speedHackModifier << std::endl;
        }

        return;
    }

    // toggle locator
    if (strcmp(command, "//locator") == 0 || strcmp(command, "//loc") == 0)
    {
        EQ_ToggleBool(g_locatorIsEnabled);

        std::cout << "Locator: " << std::boolalpha << g_locatorIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set locator
    if (strncmp(command, "//setlocator ", 13) == 0 || strncmp(command, "//setloc ", 9) == 0)
    {
        char commandEx[128];

        float y = 0.0f;
        float x = 0.0f;
        float z = 0.0f;

        int result = sscanf_s(command, "%s %f %f %f", commandEx, sizeof(commandEx), &y, &x, &z);

        if (result == 4)
        {
            g_locatorY = y;
            g_locatorX = x;
            g_locatorZ = z;

            std::cout << "Locator: " << y << ", " << x << ", " << z << std::endl;

            g_locatorIsEnabled = true;
        }

        return;
    }

    // toggle find
    if (strcmp(command, "//find") == 0)
    {
        EQ_ToggleBool(g_findIsEnabled);

        std::cout << "Find: " << std::boolalpha << g_findIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set find
    if (strncmp(command, "//setfind ", 10) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));

        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            g_findSpawnName = name;

            std::cout << "Find: " << name << std::endl;

            g_findIsEnabled = true;
        }

        return;
    }

    // toggle line to target
    if (strcmp(command, "//linetotarget") == 0 || strcmp(command, "//ltt") == 0)
    {
        EQ_ToggleBool(g_lineToTargetIsEnabled);

        std::cout << "Line To Target: " << std::boolalpha << g_lineToTargetIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // memory list
    if (strcmp(command, "//memorylist") == 0 || strcmp(command, "//memlist") == 0)
    {
        std::cout << "Memory List:" << std::endl;

        for (auto& memory : g_memoryList)
        {
            std::cout << memory.index << " : " << memory.isEnabled << " : " << memory.name << " (" << memory.description << ")" << std::endl;
        }

        return;
    }

    // toggle memory
    if (strncmp(command, "//memory ", 9) == 0 || strncmp(command, "//mem ", 6) == 0)
    {
        char commandEx[128];

        unsigned int index = 0;

        int result = sscanf_s(command, "%s %d", commandEx, sizeof(commandEx), &index);

        if (result == 2)
        {
            if (index > (g_memoryList.size() - 1))
            {
                std::cout << "No memory found at specified index: " << index << std::endl;
                return;
            }

            PEQAPPMEMORY pMemory = &g_memoryList.at(index);

            EQ_ToggleBool(pMemory->isEnabled);

            EQAPP_ToggleMemory(pMemory, pMemory->isEnabled);

            std::cout << pMemory->name << ": " << std::boolalpha << pMemory->isEnabled << std::endl;
        }

        return;
    }

    // always attack
    if (strcmp(command, "//alwaysattack") == 0 || strcmp(command, "//aa") == 0)
    {
        EQ_ToggleBool(g_alwaysAttackIsEnabled);

        std::cout << "Always Attack: " << std::boolalpha << g_alwaysAttackIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // toggle combat hotbutton
    if (strcmp(command, "//combathotbutton") == 0 || strcmp(command, "//chb") == 0)
    {
        EQ_ToggleBool(g_combatHotbuttonIsEnabled);

        std::cout << "Combat Hotbutton: " << std::boolalpha << g_combatHotbuttonIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set combat hotbutton
    if (strncmp(command, "//setcombathotbutton ", 21) == 0 || strncmp(command, "//setchb ", 9) == 0 || strncmp(command, "//schb ", 7) == 0)
    {
        char commandEx[128];

        unsigned int buttonIndex = 0;

        unsigned int delay = 1;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &buttonIndex, &delay);

        if (result == 3)
        {
            if (buttonIndex < 1 || buttonIndex > 10)
            {
                std::cout << "No hotbutton exists at specified index: " << buttonIndex << std::endl;
                return;
            }

            g_combatHotbuttonIndex = buttonIndex - 1;

            g_combatHotbuttonDelay = delay * 1000;

            std::cout << "Combat Hotbutton: " << buttonIndex << " (" << delay << " second(s))" << std::endl;

            g_combatHotbuttonIsEnabled = true;
        }

        return;
    }

    // toggle always hotbutton
    if (strcmp(command, "//alwayshotbutton") == 0 || strcmp(command, "//ahb") == 0)
    {
        EQ_ToggleBool(g_alwaysHotbuttonIsEnabled);

        std::cout << "Always Hotbutton: " << std::boolalpha << g_alwaysHotbuttonIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set always hotbutton
    if (strncmp(command, "//setalwayshotbutton ", 21) == 0 || strncmp(command, "//setahb ", 9) == 0 || strncmp(command, "//sahb ", 7) == 0)
    {
        char commandEx[128];

        unsigned int buttonIndex = 0;

        unsigned int delay = 1;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &buttonIndex, &delay);

        if (result == 3)
        {
            if (buttonIndex < 1 || buttonIndex > 10)
            {
                std::cout << "No hotbutton exists at specified index: " << buttonIndex << std::endl;
                return;
            }

            g_alwaysHotbuttonIndex = buttonIndex - 1;

            g_alwaysHotbuttonDelay = delay * 1000;

            std::cout << "Always Hotbutton: " << buttonIndex << " (" << delay << " second(s))" << std::endl;

            g_alwaysHotbuttonIsEnabled = true;
        }

        return;
    }

    // set collision radius
    if (strncmp(command, "//setcollisionradius ", 21) == 0 || strncmp(command, "//setcr ", 8) == 0 || strncmp(command, "//scr ", 6) == 0)
    {
        char commandEx[128];

        float radius = 1.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &radius);

        if (result == 2)
        {
            if (radius < 0.1f)
            {
                std::cout << "Specified radius too low: " << radius << std::endl;
                return;
            }

            DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);

            if (playerSpawn == NULL)
            {
                return;
            }

            EQ_SetSpawnCollisionRadius(playerSpawn, radius);

            std::cout << "Collision Radius: " << radius << std::endl;
        }

        return;
    }

    // toggle draw distance
    if (strcmp(command, "//drawdistance") == 0 || strcmp(command, "//dd") == 0)
    {
        EQ_ToggleBool(g_drawDistanceIsEnabled);

        std::cout << "Draw Distance: " << std::boolalpha << g_drawDistanceIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set draw distance
    if (strncmp(command, "//setdrawdistance ", 18) == 0 || strncmp(command, "//setdd ", 8) == 0)
    {
        char commandEx[128];

        float distance = EQ_DRAW_DISTANCE_DEFAULT;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);

        if (result == 2)
        {
            if (distance < EQ_DRAW_DISTANCE_MINIMUM)
            {
                std::cout << "Specified draw distance too low: " << distance << std::endl;
                return;
            }

            g_drawDistance = distance;

            g_drawDistanceIsEnabled = true;

            std::cout << "Draw Distance: " << distance << std::endl;
        }

        return;
    }

    // set field of view
    if (strncmp(command, "//setfieldofview ", 17) == 0 || strncmp(command, "//setfov ", 9) == 0)
    {
        char commandEx[128];

        float fov = EQ_FIELD_OF_VIEW_DEFAULT;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &fov);

        if (result == 2)
        {
            if (fov <= 0.0f)
            {
                std::cout << "Specified field of view too low: " << fov << std::endl;
                return;
            }

            EQ_SetFieldOfView(fov);

            std::cout << "Field Of View: " << fov << std::endl;
        }

        return;
    }

    // toggle target beep
    if (strcmp(command, "//targetbeep") == 0 || strcmp(command, "//tbeep") == 0)
    {
        EQ_ToggleBool(g_targetBeepIsEnabled);

        std::cout << "Target Beep: " << std::boolalpha << g_targetBeepIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set target beep
    if (strncmp(command, "//settargetbeep ", 16) == 0 || strncmp(command, "//settbeep ", 11) == 0 || strncmp(command, "//stb ", 6) == 0)
    {
        char commandEx[128];

        char name[1024];

        unsigned int delay = 1;

        int result = sscanf_s(command, "%s %s %d", commandEx, sizeof(commandEx), name, sizeof(name), &delay);

        if (result == 3)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            g_targetBeepName = name;

            g_targetBeepDelay = delay * 1000;

            std::cout << "Target Beep: " << name << " (" << delay << " second(s))" << std::endl;

            g_targetBeepIsEnabled = true;
        }

        return;
    }

    // toggle spawn beep
    if (strcmp(command, "//spawnbeep") == 0 || strcmp(command, "//sbeep") == 0)
    {
        EQ_ToggleBool(g_spawnBeepIsEnabled);

        std::cout << "Spawn Beep: " << std::boolalpha << g_spawnBeepIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set spawn beep
    if (strncmp(command, "//setspawnbeep ", 15) == 0 || strncmp(command, "//setsbeep ", 11) == 0 || strncmp(command, "//ssb ", 6) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));

        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            g_spawnBeepName = name;

            std::cout << "Spawn Beep: " << name << std::endl;

            g_spawnBeepIsEnabled = true;
        }

        return;
    }

    // toggle height
    if (strcmp(command, "//height") == 0)
    {
        EQ_ToggleBool(g_heightIsEnabled);

        std::cout << "Height: " << std::boolalpha << g_heightIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set height
    if (strncmp(command, "//setheight ", 12) == 0 || strncmp(command, "//seth ", 7) == 0 || strncmp(command, "//sh ", 5) == 0)
    {
        DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);

        if (targetSpawn == NULL)
        {
            std::cout << "Target not found!" << std::endl;
            return;
        }

        char commandEx[128];

        float height = g_heightMaximum;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &height);

        if (result == 2)
        {
            ((EQPlayer*)targetSpawn)->ChangeHeight(height);

            std::cout << "Set Target Height: " << height << std::endl;

            g_targetBeepIsEnabled = true;
        }

        return;
    }

    // toggle health bars
    if (strcmp(command, "//healthbars") == 0 || strcmp(command, "//hb") == 0)
    {
        EQ_ToggleBool(g_healthBarsIsEnabled);

        std::cout << "Health Bars: " << std::boolalpha << g_healthBarsIsEnabled << std::noboolalpha << std::endl;
    }

    // reset health bars
    if (strcmp(command, "//resethealthbars") == 0 || strcmp(command, "//resethb") == 0 || strcmp(command, "//rhb") == 0)
    {
        g_healthBarsSpawnNamesList.clear();

        std::cout << "Health Bars reset." << std::endl;

        return;
    }

    // add to health bars list
    if (strncmp(command, "//healthbars ", 13) == 0 || strncmp(command, "//hb ", 5) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));

        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            g_healthBarsSpawnNamesList.push_back(name);

            std::cout << "Health Bars added: " << name << std::endl;

            g_healthBarsIsEnabled = true;
        }

        return;
    }

    // toggle map labels
    if (strcmp(command, "//maplabels") == 0 || strcmp(command, "//ml") == 0)
    {
        EQ_ToggleBool(g_mapLabelsIsEnabled);

        std::cout << "Map Labels: " << std::boolalpha << g_mapLabelsIsEnabled << std::noboolalpha << std::endl;

        if (g_mapLabelsIsEnabled == false)
        {
            EQAPP_RemoveMapLabels();
        }

        return;
    }

    // add map labels
    if (strcmp(command, "//addmaplabels") == 0 || strcmp(command, "//addml") == 0 || strcmp(command, "//aml") == 0)
    {
        EQAPP_DoMapLabels();

        std::cout << "Adding map labels..." << std::endl;

        return;
    }

    // remove map labels
    if (strcmp(command, "//removemaplabels") == 0 || strcmp(command, "//removeml") == 0 || strcmp(command, "//rml") == 0)
    {
        EQAPP_RemoveMapLabels();

        std::cout << "Removing map labels..." << std::endl;

        return;
    }

    // get map labels
    if (strcmp(command, "//getmaplabels") == 0 || strcmp(command, "//getml") == 0 || strcmp(command, "//gml") == 0)
    {
        std::cout << "Map Labels:" << std::endl;

        DWORD mapViewWnd = EQ_ReadMemory<DWORD>(EQ_POINTER_CMapViewWnd);
        if (mapViewWnd == NULL)
        {
            return;
        }

        struct _EQMAPLABEL* mapLabel = EQ_ReadMemory<struct _EQMAPLABEL*>(mapViewWnd + 0x2D0);
        if (mapLabel == NULL)
        {
            return;
        }

        while (mapLabel != NULL)
        {
            std::cout << mapLabel->Label << std::endl;

            mapLabel = mapLabel->Next;
        }

        return;
    }

    // toggle filter map labels
    if (strcmp(command, "//filtermaplabels") == 0 || strcmp(command, "//filterml") == 0 || strcmp(command, "//fml") == 0)
    {
        EQ_ToggleBool(g_mapLabelsFilterIsEnabled);

        std::cout << "Map Labels Filter: " << std::boolalpha << g_mapLabelsFilterIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // filter map labels
    if (strncmp(command, "//filtermaplabels ", 18) == 0 || strncmp(command, "//filterml ", 11) == 0 || strncmp(command, "//fml ", 6) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));

        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            g_mapLabelsFilterName = name;

            std::cout << "Map Labels Filter: " << name << std::endl;

            g_mapLabelsFilterIsEnabled = true;
        }

        return;
    }

    // get zone info
    if (strcmp(command, "//getzoneinfo") == 0 || strcmp(command, "//getzone") == 0 || strcmp(command, "//getzi") == 0 || strcmp(command, "//gzi") == 0)
    {
        DWORD zoneId = EQ_GetZoneId();

        char zoneLongName[0x80] = {0};
        memcpy(zoneLongName, (LPVOID)(EQ_ZONEINFO_LONG_NAME), sizeof(zoneLongName));

        char zoneShortName[0x20] = {0};
        memcpy(zoneShortName, (LPVOID)(EQ_ZONEINFO_SHORT_NAME), sizeof(zoneShortName));

        std::cout << "Zone Info: " << zoneLongName << " (" << zoneShortName << ") " << "(ID: " << zoneId << ")" << std::endl;

        return;
    }

    // load memory
    if (strcmp(command, "//loadmemory") == 0 || strcmp(command, "//loadm") == 0 || strcmp(command, "//lm") == 0)
    {
        EQAPP_UnloadMemory();
        EQAPP_LoadMemory();

        std::cout << "Loading memories..." << std::endl;

        return;
    }

    // toggle sounds
    if (strcmp(command, "//sounds") == 0)
    {
        EQ_ToggleBool(g_soundsIsEnabled);

        std::cout << "Sounds: " << std::boolalpha << g_soundsIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // load sounds
    if (strcmp(command, "//loadsounds") == 0 || strcmp(command, "//loads") == 0 || strcmp(command, "//ls") == 0)
    {
        EQAPP_LoadSounds();

        std::cout << "Loading sounds..." << std::endl;

        return;
    }

    // toggle named spawns
    if (strcmp(command, "//namedspawns") == 0 || strcmp(command, "//ns") == 0)
    {
        EQ_ToggleBool(g_namedSpawnsIsEnabled);

        std::cout << "Named Spawns: " << std::boolalpha << g_namedSpawnsIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // load named spawns
    if (strcmp(command, "//loadnamedspawns") == 0 || strcmp(command, "//loadns") == 0 || strcmp(command, "//lns") == 0)
    {
        EQAPP_LoadNamedSpawns();

        std::cout << "Loading named spawns..." << std::endl;

        return;
    }

    // get named spawns
    if (strcmp(command, "//getnamedspawns") == 0 || strcmp(command, "//getns") == 0 || strcmp(command, "//gns") == 0)
    {
        std::cout << "Named Spawns:" << std::endl;

        size_t index = 1;

        for (auto& namedSpawn : g_namedSpawnsList)
        {
            std::cout << index << ": " << namedSpawn << std::endl;

            index++;
        }

        return;
    }

    // write character file
    if (strcmp(command, "//writecharacterfile") == 0 || strcmp(command, "//writechar") == 0 || strcmp(command, "//writecf") == 0 || strcmp(command, "//wcf") == 0)
    {
        EQAPP_WriteCharacterFile();
        EQAPP_WriteCharacterFileJson();

        return;
    }

    // load text overlay chat text
    if (strcmp(command, "//loadtextoverlaychattext") == 0 || strcmp(command, "//loadtoct") == 0 || strcmp(command, "//ltoct") == 0)
    {
        EQAPP_LoadTextOverlayChatText();

        std::cout << "Loading text overlay chat text..." << std::endl;

        return;
    }

    // load short zone names
    if (strcmp(command, "//loadshortzonenames") == 0 || strcmp(command, "//loadszn") == 0 || strcmp(command, "//lszn") == 0)
    {
        EQAPP_LoadShortZoneNames();

        std::cout << "Loading short zone names..." << std::endl;

        return;
    }

    // toggle hide corpse looted
    if (strcmp(command, "//hidecorpselooted") == 0 || strcmp(command, "//hcl") == 0)
    {
        EQ_ToggleBool(g_hideCorpseLootedIsEnabled);

        std::cout << "Hide Corpse Looted: " << std::boolalpha << g_hideCorpseLootedIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // open all doors
    if (strcmp(command, "//openalldoors") == 0 || strcmp(command, "//opendoors") == 0 || strcmp(command, "//oad") == 0)
    {
        EQAPP_DoOpenAllDoors();

        std::cout << "Opening all doors..." << std::endl;

        return;
    }

    // census
    if (strcmp(command, "//census") == 0)
    {
        EQAPP_DoCensus();

        return;
    }

    // toggle free camera
    if (strcmp(command, "//freecamera") == 0 || strcmp(command, "//freecam") == 0 || strcmp(command, "//fc") == 0)
    {
        if (g_freeCameraIsEnabled == false)
        {
            EQAPP_ToggleFreeCamera(true);
        }
        else
        {
            EQAPP_ToggleFreeCamera(false);
        }

        std::cout << "Free Camera: " << std::boolalpha << g_freeCameraIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set view actor by name
    if (strncmp(command, "//setviewactor ", 15) == 0 || strncmp(command, "//setview ", 10) == 0 || strncmp(command, "//setva ", 8) == 0 || strncmp(command, "//sva ", 6) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));

        if (result == 2)
        {
            EQ_CDisplay->SetViewActorByName(name);

            std::cout << "Set View Actor: " << name << std::endl;
        }

        return;
    }

    // set view actor to target
    if (strcmp(command, "//setviewtarget") == 0 || strcmp(command, "//setvt") == 0 || strcmp(command, "//svt") == 0)
    {
        DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);
        if (targetSpawn == NULL)
        {
            std::cout << "Target not found!" << std::endl;
            return;
        }

        EQ_SetViewActorBySpawn(targetSpawn);

        std::cout << "Set View Actor to Target." << std::endl;

        return;
    }

    // reset view actor
    if (strcmp(command, "//resetviewactor") == 0 || strcmp(command, "//resetview") == 0 || strcmp(command, "//resetva") == 0 || strcmp(command, "//rva") == 0)
    {
        EQ_ResetViewActor();

        std::cout << "Reset View Actor." << std::endl;

        return;
    }

    // map locations
    if (strcmp(command, "//maplocations") == 0 || strcmp(command, "//maplocs") == 0)
    {
        EQAPP_DoMapLocations();

        std::cout << "Saving map locations to file..." << std::endl;

        return;
    }

    // get map location
    if (strcmp(command, "//getmaplocation") == 0 || strcmp(command, "//getmaploc") == 0)
    {
        EQ_CopyTargetMapLocationToClipboard();

        std::cout << "Copying target's map location to the clipboard..." << std::endl;

        return;
    }

    // open map file
    if (strcmp(command, "//openmapfile") == 0 || strcmp(command, "//omf") == 0)
    {
        char zoneShortName[0x20] = {0};
        memcpy(zoneShortName, (LPVOID)(EQ_ZONEINFO_SHORT_NAME), sizeof(zoneShortName));

        if (strlen(zoneShortName) == 0)
        {
            return;
        }

        std::stringstream filePath;
        filePath << "maps\\" << zoneShortName << ".txt";

        ShellExecuteA(0, "open", filePath.str().c_str(), 0, 0, SW_SHOW);

        std::cout << "Opening map file: " << filePath.str() << std::endl;

        for (size_t i = 1; i < 4; i++)
        {
            std::stringstream filePath;
            filePath << "maps\\" << zoneShortName << "_" << i << ".txt";

            ShellExecuteA(0, "open", filePath.str().c_str(), 0, 0, SW_SHOW);

            std::cout << "Opening map file: " << filePath.str() << std::endl;
        }

        return;
    }
}

int __fastcall EQAPP_DETOUR_CEverQuest__SetGameState(void* pThis, void* not_used, int a1)
{
    // a1 = game state

    if (g_bExit == 1)
    {
        return EQAPP_REAL_CEverQuest__SetGameState(pThis, a1);
    }

    return EQAPP_REAL_CEverQuest__SetGameState(pThis, a1);
}

int __fastcall EQAPP_DETOUR_CEverQuest__StartCasting(void* pThis, void* not_used, int a1)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_CEverQuest__StartCasting(pThis, a1);
    }

    //EQAPP_Log("---------- CEverQuest::StartCasting ----------", 0);
    //EQAPP_Log("a1: ", a1);

    DWORD spawnId = EQ_ReadMemory<WORD>(a1);
    //EQAPP_Log("(WORD)(a1) Spawn ID: ", spawnId);

    DWORD spellId = EQ_ReadMemory<WORD>(a1 + 2);
    //EQAPP_Log("(WORD)(a1 + 2) Spell ID: ", spellId);

    DWORD spellCastTime = EQ_ReadMemory<DWORD>(a1 + 4);
    //EQAPP_Log("(DWORD)(a1 + 4) Spell Cast Time: ", spellCastTime);

    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);

    DWORD spawnInfo = EQ_EQPlayerManager->GetSpawnByID(spawnId);

    if (spawnInfo != NULL && spawnInfo != playerSpawn)
    {
        char spawnName[0x40] = {0};
        memcpy(spawnName, (LPVOID)(spawnInfo + 0xE4), sizeof(spawnName));
        //EQAPP_Log("Spawn Name:", 0);
        //EQAPP_Log(spawnName, 0);

        std::string spellName = EQ_GetSpellNameById(spellId);
        //EQAPP_Log("Spell Name:", 0);
        //EQAPP_Log(spellName, 0);

        if (spawnName != NULL && spellName.size() != 0)
        {
            if (g_debugIsEnabled == true)
            {
                std::cout <<  __FUNCTION__ << ": " << spawnName << " begins to cast " << spellName << std::endl;
            }

            std::stringstream ss;
            ss << spawnName << " (" << spellName << ")";
            EQAPP_DoOnScreenTextAddToList(ss.str());

            EQAPP_DoSpawnCastSpellAddToList(spawnInfo, spellId, spellCastTime);
        }
    }

    return EQAPP_REAL_CEverQuest__StartCasting(pThis, a1);
}

int __cdecl EQAPP_DETOUR_Exit()
{
    EQAPP_DoUnload();

    return EQAPP_REAL_Exit();
}

int __cdecl EQAPP_DETOUR_DrawNetStatus(int a1, unsigned short a2, unsigned short a3, unsigned short a4, unsigned short a5, int a6, unsigned short a7, unsigned long a8, long a9, unsigned long a10)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }

    if (g_bLoaded == 1)
    {
        EQAPP_DoLoad();
    }

    // kill switch
    if (GetAsyncKeyState(VK_PAUSE))
    {
        EQAPP_DoUnload();
    }

    if (EQ_IsInGame() == false)
    {
        return EQAPP_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }

    if (EQ_IsScreenshotBeingTaken() == true)
    {
        return EQAPP_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }

    EQAPP_DoWindowTitles();

    EQAPP_DoFreeCameraKeys();

    EQAPP_DoAutoLoot();
    EQAPP_DoSpawnCastSpell();
    EQAPP_DoTargetBeep();
    EQAPP_DoCombatHotbutton();
    EQAPP_DoAlwaysHotbutton();
    EQAPP_DoAlwaysAttack();
    EQAPP_DoMaxSwimming();
    EQAPP_DoHeight();
    EQAPP_DoDrawDistance();
    EQAPP_DoHideCorpseLooted();

    if (EQAPP_AreImportantWindowsOpen() == false && EQ_IsKeyShiftPressed() == false)
    {
        EQAPP_DoLineToTarget();
        EQAPP_DoEsp();

        EQAPP_DoHealthBars();
        EQAPP_DoNamedSpawns();
        EQAPP_DoOnScreenText();
    }

    EQAPP_DoHud();

    return EQAPP_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

int __cdecl EQAPP_DETOUR_ExecuteCmd(DWORD a1, BOOL a2, PVOID a3)
{
    // a1 = command index

    if (g_bExit == 1)
    {
        return EQAPP_REAL_ExecuteCmd(a1, a2, a3);
    }

    if (g_freeCameraIsEnabled == true)
    {
        // prevent movement and changing the camera view
        if
        (
            (a1 >= EQ_CMD_JUMP && a1 <= EQ_CMD_STRAFE_RIGHT)                                      ||
            (a1 >= EQ_CMD_SET_CAMERA_VIEW_OVERHEAD && a1 <= EQ_CMD_SET_CAMERA_VIEW_USER_DEFINED2) ||
            a1 == EQ_CMD_TOGGLE_CAMERA_VIEW
        )
        {
            return EQAPP_REAL_ExecuteCmd(NULL, 0, 0);
        }
    }

    if (g_debugIsEnabled == true)
    {
        std::string commandName = EQ_GetExecuteCmdName(a1);
        if (a1 != NULL && a2 != NULL && a3 != NULL && commandName.size() != 0)
        {
            std::cout << "ExecuteCmd: " << a1 << ", " << a2 << ", " << a3 << " (" << commandName << ")" << std::endl;
        }
    }

    return EQAPP_REAL_ExecuteCmd(a1, a2, a3);
}

int __fastcall EQAPP_DETOUR_CMapViewWnd__dCMapViewWnd(void* pThis, void* not_used)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_CMapViewWnd__dCMapViewWnd(pThis);
    }

    //std::cout << "CMapViewWnd::~CMapViewWnd()" << std::endl;

    return EQAPP_REAL_CMapViewWnd__dCMapViewWnd(pThis);
}

int __fastcall EQAPP_DETOUR_CMapViewWnd__DrawMap(void* pThis, void* not_used, int a1, int a2, int a3, int a4)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_CMapViewWnd__DrawMap(pThis, a1, a2, a3, a4);
    }

    if (g_mapLabelsIsEnabled == true)
    {
        EQAPP_DoMapLabels();
    }

    return EQAPP_REAL_CMapViewWnd__DrawMap(pThis, a1, a2, a3, a4);
}

int __fastcall EQAPP_DETOUR_CDisplay__CreatePlayerActor(void* pThis, void* not_used, int a1, int a2, int a3, int a4, int a5)
{
    // a1 = spawnInfo

    if (g_bExit == 1)
    {
        return EQAPP_REAL_CDisplay__CreatePlayerActor(pThis, a1, a2, a3, a4, a5);
    }

    if (a1 != NULL)
    {
        if (g_replaceRacesIsEnabled == true)
        {
            DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);
            if (a1 == playerSpawn)
            {
                int spawnRace = EQ_ReadMemory<DWORD>(playerSpawn + 0xE64);
                if (spawnRace == EQ_RACE_CHOKADAI)
                {
                    EQ_WriteMemory<DWORD>(playerSpawn + 0xE64, EQ_RACE_HUMAN);
                }
            }
        }

        char spawnName[0x40] = {0};
        memcpy(spawnName, (LPVOID)(a1 + 0xA4), sizeof(spawnName));

        if (g_debugIsEnabled == true && spawnName != NULL)
        {
            std::cout << "CDisplay::CreatePlayerActor(): " << spawnName << std::endl;
            std::cout << "a1: " << a1 << std::endl;
            std::cout << "a2: " << a2 << std::endl; // 0
            std::cout << "a3: " << a3 << std::endl; // 1
            std::cout << "a4: " << a4 << std::endl; // 2
            std::cout << "a5: " << a5 << std::endl; // 1
        }

        if (EQ_DoesSpawnExist(a1) == false)
        {
            EQAPP_DoSpawnMessage(a1);
        }

        if (EQ_IsInGame() == true)
        {
            EQAPP_DoSpawnBeep(a1);
        }
    }

    return EQAPP_REAL_CDisplay__CreatePlayerActor(pThis, a1, a2, a3, a4, a5);
}

int __fastcall EQAPP_DETOUR_CDisplay__DeleteActor(void* pThis, void* not_used, DWORD a1)
{
    // a1 = actorInstance

    if (g_bExit == 1)
    {
        return EQAPP_REAL_CDisplay__DeleteActor(pThis, a1);
    }

    if (a1 != NULL)
    {
        DWORD spawnInfo = EQ_ReadMemory<DWORD>(a1 + 0x70);

        if (spawnInfo != NULL)
        {
            char spawnName[0x40] = {0};
            memcpy(spawnName, (LPVOID)(spawnInfo + 0xA4), sizeof(spawnName));

            if (g_debugIsEnabled == true && spawnName != NULL)
            {
                std::cout << "CDisplay::DeleteActor(): " << spawnName << std::endl;
            }

            EQAPP_DoDespawnMessage(spawnInfo);
        }
    }

    return EQAPP_REAL_CDisplay__DeleteActor(pThis, a1);
}

int __fastcall EQAPP_DETOUR_CEverQuest__dsp_chat(void* pThis, void* not_used, const char* a1, int a2, bool a3)
{
    // a1 = text
    // a2 = textColor

    if (g_bExit == 1)
    {
        return EQAPP_REAL_CEverQuest__dsp_chat(pThis, a1, a2, a3);
    }

    if (a1 == NULL)
    {
        return EQAPP_REAL_CEverQuest__dsp_chat(pThis, a1, a2, a3);
    }

    if (strlen(a1) == 0)
    {
        return EQAPP_REAL_CEverQuest__dsp_chat(pThis, a1, a2, a3);
    }

    if (g_soundsIsEnabled == true)
    {
        for (auto& sound : g_soundsList)
        {
            if (strstr(a1, sound.text.c_str()) != NULL)
            {
                PlaySoundA(sound.filename.c_str(), 0, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
                break;
            }
        }
    }

    if (g_textOverlayOnChatTextIsEnabled == true)
    {
        for (auto& text : g_textOverlayChatTextList)
        {
            if (strstr(a1, text.c_str()) != NULL)
            {
                DWORD windowWidth  = EQ_ReadMemory<DWORD>(EQ_WINDOW_WIDTH);
                DWORD windowHeight = EQ_ReadMemory<DWORD>(EQ_WINDOW_HEIGHT);

                EQ_CTextOverlay->DisplayText(a1, EQ_TEXT_COLOR_WHITE, 15, 192, (int)(windowWidth / 2), (int)(windowHeight / 3), g_textOverlayChatTextDuration);

                EQAPP_DoOnScreenTextAddToList(a1);

                break;
            }
        }
    }

    if (g_censusIsActive == true)
    {
        if
        (
            strstr(a1, "[") != NULL &&
            strstr(a1, "]") != NULL
        )
        {
            std::cout << a1 << std::endl;
        }
    }

    return EQAPP_REAL_CEverQuest__dsp_chat(pThis, a1, a2, a3);
}

int __fastcall EQAPP_DETOUR_CEverQuest__MoveToZone(void* pThis, void* not_used, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_CEverQuest__MoveToZone(pThis, a1, a2, a3, a4, a5, a6, a7, a8);//, a9, a10, a11, a12, a13, a14);
    }

    EQAPP_RemoveMapLabels();

    return EQAPP_REAL_CEverQuest__MoveToZone(pThis, a1, a2, a3, a4, a5, a6, a7, a8);//, a9, a10, a11, a12, a13, a14);
}

int __fastcall EQAPP_DETOUR_CEverQuest__EnterZone(void* pThis, void* not_used, struct HWND__* a1)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_CEverQuest__EnterZone(pThis, a1);
    }

    int result = EQAPP_REAL_CEverQuest__EnterZone(pThis, a1);

    EQAPP_WriteCharacterFile();
    EQAPP_WriteCharacterFileJson();

    EQAPP_LoadNamedSpawns();

    g_alwaysAttackIsEnabled = false;

    g_censusIsActive = false;

    g_spawnCastSpellList.clear();

    g_onScreenTextList.clear();

    return result;
}

int __fastcall EQAPP_DETOUR_CEverQuest__InterpretCmd(void* pThis, void* not_used, DWORD a1, const char* a2)
{
    // a1 = spawnInfo
    // a2 = command text

    if (g_bExit == 1)
    {
        return EQAPP_REAL_CEverQuest__InterpretCmd(pThis, NULL, NULL);
    }

    if (a1 == NULL || a2 == NULL)
    {
        return EQAPP_REAL_CEverQuest__InterpretCmd(pThis, NULL, NULL);
    }

    if (strlen(a2) == 0)
    {
        return EQAPP_REAL_CEverQuest__InterpretCmd(pThis, NULL, NULL);
    }

    if (strcmp(a2, "/exit") == 0)
    {
        EQAPP_DoUnload();
    }

    if (strlen(a2) > 2 && strncmp(a2, "//", 2) == 0)
    {
        EQAPP_DoInterpretCommand(a2);

        return EQAPP_REAL_CEverQuest__InterpretCmd(pThis, NULL, NULL);
    }

    return EQAPP_REAL_CEverQuest__InterpretCmd(pThis, a1, a2);
}

int __fastcall EQAPP_DETOUR_EQ_Character__eqspa_movement_rate(void* pThis, void* not_used, int a1)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_EQ_Character__eqspa_movement_rate(pThis, a1);
    }

    int result = EQAPP_REAL_EQ_Character__eqspa_movement_rate(pThis, a1);

    if (g_speedHackIsEnabled == true)
    {
        DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);

        if (playerSpawn != NULL)
        {
            FLOAT speedModifier = EQ_ReadMemory<FLOAT>(playerSpawn + 0x18);

            if (speedModifier < g_speedHackModifier)
            {
                EQ_WriteMemory<FLOAT>(playerSpawn + 0x18, g_speedHackModifier);
            }
        }
    }

    return result;
}

int __fastcall EQAPP_DETOUR_EQPlayer__ChangePosition(void* pThis, void* not_used, BYTE a1)
{
    // a1 = standing state

    if (g_bExit == 1)
    {
        return EQAPP_REAL_EQPlayer__ChangePosition(pThis, a1);
    }

    if (g_freeCameraIsEnabled == true)
    {
        EQAPP_ToggleFreeCamera(false);
    }

    if (a1 == EQ_STANDING_STATE_DUCKING)
    {
        g_censusIsActive = false;
    }

    if (g_neverFrozenIsEnabled == true)
    {
        if (a1 == EQ_STANDING_STATE_FROZEN)
        {
            a1 = EQ_STANDING_STATE_STANDING;
        }
    }

    return EQAPP_REAL_EQPlayer__ChangePosition(pThis, a1);
}

int __fastcall EQAPP_DETOUR_EQPlayer__do_change_form(void* pThis, void* not_used, int a1)
{
    // a1 = change form structure

    if (g_bExit == 1)
    {
        return EQAPP_REAL_EQPlayer__do_change_form(pThis, a1);
    }

    //std::cout << "EQPlayer::do_change_form():" << std::endl;
    //std::cout << "a1: " << std::hex << a1 << std::dec << std::endl;

    //std::cout << "a: " << ((EQCHANGEFORM*)a1)->a << std::endl;
    //std::cout << "b: " << ((EQCHANGEFORM*)a1)->b << std::endl;
    //std::cout << "c: " << ((EQCHANGEFORM*)a1)->c << std::endl;
    //std::cout << "d: " << ((EQCHANGEFORM*)a1)->d << std::endl;
    //std::cout << "e: " << ((EQCHANGEFORM*)a1)->e << std::endl;
    //std::cout << "f: " << ((EQCHANGEFORM*)a1)->f << std::endl;
    //std::cout << "g: " << ((EQCHANGEFORM*)a1)->g << std::endl;

    return EQAPP_REAL_EQPlayer__do_change_form(pThis, a1);
}

int __fastcall EQAPP_DETOUR_EQPlayer__SetRace(void* pThis, void* not_used, int a1)
{
    // a1 = race id

    if (g_bExit == 1)
    {
        return EQAPP_REAL_EQPlayer__SetRace(pThis, a1);
    }

    if (g_replaceRacesIsEnabled == true)
    {
        if (a1 == EQ_RACE_INVISIBLE_MAN)
        {
            a1 = EQ_RACE_HUMAN;
        }
        else if (a1 == EQ_RACE_SKELETON)
        {
            a1 = EQ_RACE_SKELETON2;
        }

        DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);
        if ((DWORD)pThis == playerSpawn)
        {
            if (a1 == EQ_RACE_CHOKADAI)
            {
                a1 = EQ_RACE_HUMAN;
            }
        }
    }

    return EQAPP_REAL_EQPlayer__SetRace(pThis, a1);
}

int __fastcall EQAPP_DETOUR_MapViewMap__Save(void* pThis, void* not_used, char* a1)
{
    // a1 = short zone name of map

    if (g_bExit == 1)
    {
        return EQAPP_REAL_MapViewMap__Save(pThis, a1);
    }

    std::cout << "The game is saving the map..." << std::endl;

    EQAPP_RemoveMapLabels();

    return EQAPP_REAL_MapViewMap__Save(pThis, a1);
}

int __fastcall EQAPP_DETOUR_MapViewMap__SaveEx(void* pThis, void* not_used, char* a1, int a2)
{
    // a1 = short zone name of map
    // a2 = map layer

    if (g_bExit == 1)
    {
        return EQAPP_REAL_MapViewMap__SaveEx(pThis, a1, a2);
    }

    std::cout << "The game is saving the map by layer..." << a2 << std::endl;

    EQAPP_RemoveMapLabels();

    return EQAPP_REAL_MapViewMap__SaveEx(pThis, a1, a2);
}

void EQAPP_AddDetours()
{
    EQAPP_REAL_Exit = (EQ_FUNCTION_TYPE_Exit)DetourFunction((PBYTE)EQ_FUNCTION_Exit, (PBYTE)EQAPP_DETOUR_Exit);
    EQAPP_REAL_DrawNetStatus = (EQ_FUNCTION_TYPE_DrawNetStatus)DetourFunction((PBYTE)EQ_FUNCTION_DrawNetStatus, (PBYTE)EQAPP_DETOUR_DrawNetStatus);
    EQAPP_REAL_ExecuteCmd = (EQ_FUNCTION_TYPE_ExecuteCmd)DetourFunction((PBYTE)EQ_FUNCTION_ExecuteCmd, (PBYTE)EQAPP_DETOUR_ExecuteCmd);

    EQAPP_REAL_CDisplay__CreatePlayerActor = (EQ_FUNCTION_TYPE_CDisplay__CreatePlayerActor)DetourFunction((PBYTE)EQ_FUNCTION_CDisplay__CreatePlayerActor, (PBYTE)EQAPP_DETOUR_CDisplay__CreatePlayerActor);
    EQAPP_REAL_CDisplay__DeleteActor = (EQ_FUNCTION_TYPE_CDisplay__DeleteActor)DetourFunction((PBYTE)EQ_FUNCTION_CDisplay__DeleteActor, (PBYTE)EQAPP_DETOUR_CDisplay__DeleteActor);

    EQAPP_REAL_CEverQuest__dsp_chat = (EQ_FUNCTION_TYPE_CEverQuest__dsp_chat)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__dsp_chat, (PBYTE)EQAPP_DETOUR_CEverQuest__dsp_chat);
    EQAPP_REAL_CEverQuest__EnterZone = (EQ_FUNCTION_TYPE_CEverQuest__EnterZone)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__EnterZone, (PBYTE)EQAPP_DETOUR_CEverQuest__EnterZone);
    //EQAPP_REAL_CEverQuest__InterpretCmd = (EQ_FUNCTION_TYPE_CEverQuest__InterpretCmd)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__InterpretCmd, (PBYTE)EQAPP_DETOUR_CEverQuest__InterpretCmd);
    EQAPP_REAL_CEverQuest__MoveToZone = (EQ_FUNCTION_TYPE_CEverQuest__MoveToZone)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__MoveToZone, (PBYTE)EQAPP_DETOUR_CEverQuest__MoveToZone);
    EQAPP_REAL_CEverQuest__SetGameState = (EQ_FUNCTION_TYPE_CEverQuest__SetGameState)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__SetGameState, (PBYTE)EQAPP_DETOUR_CEverQuest__SetGameState);
    EQAPP_REAL_CEverQuest__StartCasting = (EQ_FUNCTION_TYPE_CEverQuest__StartCasting)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__StartCasting, (PBYTE)EQAPP_DETOUR_CEverQuest__StartCasting);

    //EQAPP_REAL_CMapViewWnd__dCMapViewWnd = (EQ_FUNCTION_TYPE_CMapViewWnd__dCMapViewWnd)DetourFunction((PBYTE)EQ_FUNCTION_CMapViewWnd__dCMapViewWnd, (PBYTE)EQAPP_DETOUR_CMapViewWnd__dCMapViewWnd);
    EQAPP_REAL_CMapViewWnd__DrawMap = (EQ_FUNCTION_TYPE_CMapViewWnd__DrawMap)DetourFunction((PBYTE)EQ_FUNCTION_CMapViewWnd__DrawMap, (PBYTE)EQAPP_DETOUR_CMapViewWnd__DrawMap);

    EQAPP_REAL_EQ_Character__eqspa_movement_rate = (EQ_FUNCTION_TYPE_EQ_Character__eqspa_movement_rate)DetourFunction((PBYTE)EQ_FUNCTION_EQ_Character__eqspa_movement_rate, (PBYTE)EQAPP_DETOUR_EQ_Character__eqspa_movement_rate);

    EQAPP_REAL_EQPlayer__ChangePosition = (EQ_FUNCTION_TYPE_EQPlayer__ChangePosition)DetourFunction((PBYTE)EQ_FUNCTION_EQPlayer__ChangePosition, (PBYTE)EQAPP_DETOUR_EQPlayer__ChangePosition);
    //EQAPP_REAL_EQPlayer__do_change_form = (EQ_FUNCTION_TYPE_EQPlayer__do_change_form)DetourFunction((PBYTE)EQ_FUNCTION_EQPlayer__do_change_form, (PBYTE)EQAPP_DETOUR_EQPlayer__do_change_form);
    EQAPP_REAL_EQPlayer__SetRace = (EQ_FUNCTION_TYPE_EQPlayer__SetRace)DetourFunction((PBYTE)EQ_FUNCTION_EQPlayer__SetRace, (PBYTE)EQAPP_DETOUR_EQPlayer__SetRace);

    EQAPP_REAL_MapViewMap__Save = (EQ_FUNCTION_TYPE_MapViewMap__Save)DetourFunction((PBYTE)EQ_FUNCTION_MapViewMap__Save, (PBYTE)EQAPP_DETOUR_MapViewMap__Save);
    EQAPP_REAL_MapViewMap__SaveEx = (EQ_FUNCTION_TYPE_MapViewMap__SaveEx)DetourFunction((PBYTE)EQ_FUNCTION_MapViewMap__SaveEx, (PBYTE)EQAPP_DETOUR_MapViewMap__SaveEx);
}

void EQAPP_RemoveDetours()
{
    DetourRemove((PBYTE)EQAPP_REAL_Exit, (PBYTE)EQAPP_DETOUR_Exit);
    DetourRemove((PBYTE)EQAPP_REAL_DrawNetStatus, (PBYTE)EQAPP_DETOUR_DrawNetStatus);
    DetourRemove((PBYTE)EQAPP_REAL_ExecuteCmd, (PBYTE)EQAPP_DETOUR_ExecuteCmd);

    DetourRemove((PBYTE)EQAPP_REAL_CDisplay__CreatePlayerActor, (PBYTE)EQAPP_DETOUR_CDisplay__CreatePlayerActor);
    DetourRemove((PBYTE)EQAPP_REAL_CDisplay__DeleteActor, (PBYTE)EQAPP_DETOUR_CDisplay__DeleteActor);

    DetourRemove((PBYTE)EQAPP_REAL_CEverQuest__dsp_chat, (PBYTE)EQAPP_DETOUR_CEverQuest__dsp_chat);
    DetourRemove((PBYTE)EQAPP_REAL_CEverQuest__EnterZone, (PBYTE)EQAPP_DETOUR_CEverQuest__EnterZone);
    //DetourRemove((PBYTE)EQAPP_REAL_CEverQuest__InterpretCmd, (PBYTE)EQAPP_DETOUR_CEverQuest__InterpretCmd);
    DetourRemove((PBYTE)EQAPP_REAL_CEverQuest__MoveToZone, (PBYTE)EQAPP_DETOUR_CEverQuest__MoveToZone);
    DetourRemove((PBYTE)EQAPP_REAL_CEverQuest__SetGameState, (PBYTE)EQAPP_DETOUR_CEverQuest__SetGameState);
    DetourRemove((PBYTE)EQAPP_REAL_CEverQuest__StartCasting, (PBYTE)EQAPP_DETOUR_CEverQuest__StartCasting);

    //DetourRemove((PBYTE)EQAPP_REAL_CMapViewWnd__dCMapViewWnd, (PBYTE)EQAPP_DETOUR_CMapViewWnd__dCMapViewWnd);
    DetourRemove((PBYTE)EQAPP_REAL_CMapViewWnd__DrawMap, (PBYTE)EQAPP_DETOUR_CMapViewWnd__DrawMap);

    DetourRemove((PBYTE)EQAPP_REAL_EQ_Character__eqspa_movement_rate, (PBYTE)EQAPP_DETOUR_EQ_Character__eqspa_movement_rate);

    DetourRemove((PBYTE)EQAPP_REAL_EQPlayer__ChangePosition, (PBYTE)EQAPP_DETOUR_EQPlayer__ChangePosition);
    //DetourRemove((PBYTE)EQAPP_REAL_EQPlayer__do_change_form, (PBYTE)EQAPP_DETOUR_EQPlayer__do_change_form);
    DetourRemove((PBYTE)EQAPP_REAL_EQPlayer__SetRace, (PBYTE)EQAPP_DETOUR_EQPlayer__SetRace);

    DetourRemove((PBYTE)EQAPP_REAL_MapViewMap__Save, (PBYTE)EQAPP_DETOUR_MapViewMap__Save);
    DetourRemove((PBYTE)EQAPP_REAL_MapViewMap__SaveEx, (PBYTE)EQAPP_DETOUR_MapViewMap__SaveEx);
}

void EQAPP_LoadGraphicsDllFunctions()
{
    DWORD baseAddress = EQ_ReadMemory<DWORD>(EQ_POINTER_GRAPHICS_DLL_BASE_ADDRESS);

    DWORD addressDrawLine = baseAddress + EQ_GRAPHICS_DLL_OFFSET_DrawLine;
    EQGraphicsDLL__DrawLine = (EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawLine)addressDrawLine;

    DWORD addressDrawQuad = baseAddress + EQ_GRAPHICS_DLL_OFFSET_DrawQuad;
    EQGraphicsDLL__DrawQuad = (EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawQuad)addressDrawQuad;
}

DWORD WINAPI EQAPP_ThreadLoop(LPVOID param)
{
    while (g_bExit == 0)
    {
        Sleep(100);
    }

    Sleep(100);

    EQAPP_RemoveDetours();

    FreeConsole();

    Sleep(1000);

    FreeLibraryAndExitThread(g_module, 0);
    return 0;
}

DWORD WINAPI EQAPP_ThreadConsole(LPVOID param)
{
    EQAPP_LoadConsole();

    while (g_bExit == 0)
    {
        std::string command;
        std::getline(std::cin, command);

        EQAPP_DoInterpretCommand(command.c_str());
    }

    FreeConsole();

    return 0;
}

DWORD WINAPI EQAPP_ThreadLoad(LPVOID param)
{
    EQAPP_EnableDebugPrivileges();

    EQAPP_LoadGraphicsDllFunctions();

    g_handleThreadConsole = CreateThread(NULL, 0, &EQAPP_ThreadConsole, NULL, 0, NULL);

    // wait for the console to load
    while (g_bConsole == 0);

    g_handleThreadLoop = CreateThread(NULL, 0, &EQAPP_ThreadLoop, NULL, 0, NULL);

    EQAPP_AddDetours();

    g_bLoaded = 1;

    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    g_module = module;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            g_handleThreadLoad = CreateThread(NULL, 0, &EQAPP_ThreadLoad, NULL, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
