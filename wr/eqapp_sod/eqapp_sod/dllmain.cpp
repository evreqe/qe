#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>

// console
#include <fcntl.h>
#include <io.h>

#include <windows.h>
#include <tlhelp32.h>

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

//#define DIRECTINPUT_VERSION 0x0800
//#include "dinput.h"
//#pragma comment(lib, "dinput.lib")

#include "detours.h"
#pragma comment(lib, "detours.lib")

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
EQ_FUNCTION_TYPE_CDisplay__CreatePlayerActor EQAPP_REAL_CDisplay__CreatePlayerActor = NULL;
EQ_FUNCTION_TYPE_CDisplay__DeleteActor EQAPP_REAL_CDisplay__DeleteActor = NULL;
//EQ_FUNCTION_TYPE_CDisplay__SetNameSpriteState EQAPP_REAL_CDisplay__SetNameSpriteState = NULL;
//EQ_FUNCTION_TYPE_CDisplay__SetNameSpriteTint EQAPP_REAL_CDisplay__SetNameSpriteTint = NULL;
EQ_FUNCTION_TYPE_CEverQuest__dsp_chat EQAPP_REAL_CEverQuest__dsp_chat = NULL;
EQ_FUNCTION_TYPE_CEverQuest__EnterZone EQAPP_REAL_CEverQuest__EnterZone = NULL;
EQ_FUNCTION_TYPE_CEverQuest__InterpretCmd EQAPP_REAL_CEverQuest__InterpretCmd = NULL;
EQ_FUNCTION_TYPE_CEverQuest__MoveToZone EQAPP_REAL_CEverQuest__MoveToZone = NULL;
EQ_FUNCTION_TYPE_CEverQuest__SetGameState EQAPP_REAL_CEverQuest__SetGameState = NULL;
EQ_FUNCTION_TYPE_CMapViewWnd__DrawMap EQAPP_REAL_CMapViewWnd__DrawMap = NULL;
EQ_FUNCTION_TYPE_EQ_Character__eqspa_movement_rate EQAPP_REAL_EQ_Character__eqspa_movement_rate = NULL;
EQ_FUNCTION_TYPE_EQPlayer__ChangePosition EQAPP_REAL_EQPlayer__ChangePosition = NULL;

// types
typedef struct _EQAPPMEMORY
{
    size_t index;
    std::string name;
    std::string filename;
    std::string description;
    bool isEnabled;
} EQAPPMEMORY, *PEQAPPMEMORY;

// globals
std::ios g_coutFlags(NULL);

bool g_debugIsEnabled = false;

bool g_memoryIsEnabled = true;
std::vector<EQAPPMEMORY> g_memoryList;

bool g_espIsEnabled = true;
float g_espSpawnDistance = 400.0f;
float g_espGroundSpawnDistance = 200.0f;

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

bool g_clipIsEnabled = true;
float g_clipMinimum = 10000.0f;

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
DWORD g_mapLabelsData = 255;
DWORD g_mapLabelsTimer = 0;
DWORD g_mapLabelsDelay = 5000;

bool g_neverFrozenIsEnabled = true;

// slash commands
const std::vector<std::string> g_slashCommands
{
    "//help",
    "//unload",
    "//debug",
    "//test",
    "//castray",
    "//getmeleerange",
    "//getspellname",
    "//spawnlist",
    "//esp",
    "//espspawndistance",
    "//espgroundspawndistance",
    "//speed",
    "//loc",
    "//find",
    "//linetotarget",
    "//memorylist",
    "//memory",
    "//alwaysattack",
    "//combathotbutton",
    "//alwayshotbutton",
    "//setcollisionradius",
    "//targetbeep",
    "//spawnbeep",
    "//hidecorpselooted",
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
void EQAPP_LoadMemory();
void EQAPP_UnloadMemory();
void EQAPP_ToggleMemory(PEQAPPMEMORY pMemory, bool bEnabled);
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
void EQAPP_DoEsp();
void EQAPP_DoMaxSwimming();
void EQAPP_DoHeight();
void EQAPP_DoClip();
void EQAPP_DoHideCorpseLooted();
void EQAPP_DoMapLabels();
void EQAPP_RemoveMapLabels();
void EQAPP_DoInterpretCommand(char* command);
int __cdecl EQAPP_DETOUR_DrawNetStatus(int a1, unsigned short a2, unsigned short a3, unsigned short a4, unsigned short a5, int a6, unsigned short a7, unsigned long a8, long a9, unsigned long a10);
int __fastcall EQAPP_DETOUR_CEverQuest__dsp_chat(void* pThis, void* not_used, const char* a1, int a2, bool a3);
int __fastcall EQAPP_DETOUR_CEverQuest__EnterZone(void* pThis, void* not_used, struct HWND__* a1);
int __fastcall EQAPP_DETOUR_CEverQuest__InterpretCmd(void* pThis, void* not_used, class EQPlayer* a1, char* a2);
int __fastcall EQAPP_DETOUR_EQ_Character__eqspa_movement_rate(void* pThis, void* not_used, int a1);
void EQAPP_AddDetours();
void EQAPP_RemoveDetours();
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
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;

    // allocate a console for this app
    AllocConsole();

    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );

    // redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );

    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );

    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    std::ios::sync_with_stdio();

    // disable the close button
    HWND hwnd = GetConsoleWindow();
    if (hwnd != NULL)
    {
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

    file << "[" << __TIME__ << "] " << text << " (" << number << ")" << " Hex(" << std::hex << number << ")" << std::endl;

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
    EQAPP_LoadNamedSpawns();
    EQAPP_LoadTextOverlayChatText();

    EQ_WriteToChat("Loaded.");

    g_bLoaded = 0;
}

void EQAPP_DoUnload()
{
    EQAPP_RemoveMapLabels();

    EQAPP_UnloadMemory();

    EQ_WriteToChat("Unloaded.");

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

        std::stringstream ssEnabled;
        ssEnabled << "bEnabled" << i;

        EQAPPMEMORY memory;
        memory.isEnabled = EQAPP_IniReadBool("eqapp/memory.ini", "Memory", ssEnabled.str().c_str(), 0);

        std::cout << __FUNCTION__ << ": " << filename << " isEnabled: " << std::boolalpha << memory.isEnabled << std::endl;

        std::stringstream filepath;
        filepath << "eqapp/memory/" << filename;

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

void EQAPP_LoadNamedSpawns()
{
    g_namedSpawnsList.clear();

    char zoneShortName[0x20] = {0};
    memcpy(zoneShortName, (LPVOID)(EQ_ZONEINFO_SHORT_NAME), sizeof(zoneShortName));

    if (strlen(zoneShortName) == 0)
    {
        return;
    }

    std::stringstream filePath;
    filePath << "eqapp/namedspawns/" << zoneShortName << ".txt";

    std::ifstream file;
    file.open(filePath.str().c_str(), std::ios::in);

    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": Failed to open file: " << filePath.str() << std::endl;
    }

    std::string line;
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

void EQAPP_DoNamedSpawns()
{
    if (g_namedSpawnsIsEnabled == false)
    {
        return;
    }

    unsigned int numSpawns = 0;

    unsigned int x = g_namedSpawnsX;
    unsigned int y = g_namedSpawnsY;

    EQ_DrawQuad((float)x, (float)y, g_namedSpawnsWidth, g_namedSpawnsHeight, 0xC8000040);

    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
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

                EQ_DrawText(spawnName, x, y, 0xFFFFFFFF, 2);
                y += 13;

                break;
            }
        }

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    if (numSpawns > 0)
    {
        g_namedSpawnsHeight = (float)(numSpawns * 13);
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

void EQAPP_DoHud()
{
    unsigned int x = 5;
    unsigned int y = 28;

    float size = 127.0f;

    EQ_DrawQuad((float)x, (float)y, size, size, 0xC8000040);

    EQ_DrawText(g_applicationName, x, y, 0xFF808080, 2);
    y += 12;

    DWORD numPlayersInZone = EQ_GetNumPlayersInZone();

    if (numPlayersInZone != 0)
    {
        std::stringstream ss;
        ss << "Players in zone: " << numPlayersInZone;

        EQ_DrawText(ss.str().c_str(), x, y, 0xFFFFFFFF, 2);
        y += 12;
    }

    if (g_speedHackIsEnabled == true)
    {
        std::stringstream ss;
        ss << "Speed: " << g_speedHackModifier;

        EQ_DrawText(ss.str().c_str(), x, y, 0xFFFFFFFF, 2);
        y += 12;
    }

    if (EQ_IsAutoAttackEnabled() == true)
    {
        EQ_DrawText("Auto attack is on.", x, y, 0xFFFF0000, 2);
        y += 12;
    }

    if (EQ_IsAutoFireEnabled() == true)
    {
        EQ_DrawText("Auto fire is on.", x, y, 0xFFFF0000, 2);
        y += 12;
    }

    if (g_alwaysAttackIsEnabled == true)
    {
        EQ_DrawText("Always attack is on.", x, y, 0xFFFF0000, 2);
        y += 12;
    }

    if (g_alwaysHotbuttonIsEnabled == true)
    {
        std::stringstream ss;
        ss << "Always Hotbutton: " << (g_alwaysHotbuttonIndex + 1) << " (" << (g_alwaysHotbuttonDelay / 1000) << "s)";

        EQ_DrawText(ss.str().c_str(), x, y, 0xFF00FF00, 2);
        y += 12;
    }

    if (g_combatHotbuttonIsEnabled == true)
    {
        std::stringstream ss;
        ss << "Combat Hotbutton: " << (g_combatHotbuttonIndex + 1) << " (" << (g_combatHotbuttonDelay / 1000) << "s)";

        EQ_DrawText(ss.str().c_str(), x, y, 0xFF00FF00, 2);
        y += 12;
    }

    if (g_targetBeepIsEnabled == true)
    {
        std::stringstream ss;
        ss << "Target Beep: " << g_targetBeepName << " (" << (g_targetBeepDelay / 1000) << "s)";

        EQ_DrawText(ss.str().c_str(), x, y, 0xFF00FF00, 2);
        y += 12;
    }

    if (g_spawnBeepIsEnabled == true)
    {
        std::stringstream ss;
        ss << "Spawn Beep: " << g_spawnBeepName;

        EQ_DrawText(ss.str().c_str(), x, y, 0xFF00FF00, 2);
        y += 12;
    }
}

void EQAPP_DrawHealthBar(int x, int y, char* spawnName, int spawnHealth)
{
    if (strlen(spawnName) == 0)
    {
        return;
    }

    float healthBarWidth = g_healthBarsWidth;

    if (spawnHealth != 0)
    {
        float healthPercent = 100.0f / spawnHealth;

        healthBarWidth = g_healthBarsWidth / healthPercent;
    }

    EQ_DrawQuad((float)x, (float)y, healthBarWidth, 13.0f, 0xC8FF0000);

    EQ_DrawText(spawnName, x, y, 0xFFFFFFFF, 2);

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

        EQ_DrawText(ss.str().c_str(), (x + (int)(g_healthBarsWidth * 0.5f)), y, 0xFFFFFFFF, 2);
    }
}

void EQAPP_DoHealthBars()
{
    if (g_healthBarsIsEnabled == false)
    {
        return;
    }

    unsigned int numHealthBars = 0;

    unsigned int x = g_healthBarsX;
    unsigned int y = g_healthBarsY;

    EQ_DrawQuad((float)x, (float)y, g_healthBarsWidth, g_healthBarsHeight, 0xC8000040);

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
                y += 13;

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
                y += 13;

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
            y += 13;
        }

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    if (numHealthBars > 0)
    {
        g_healthBarsHeight = (float)(numHealthBars * 13);
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

    DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);

    if (targetSpawn == NULL)
    {
        return;
    }

    DWORD currentTime = EQ_GetTimer();

    if ((currentTime - g_targetBeepTimer) < g_targetBeepDelay)
    {
        return;
    }

    g_targetBeepTimer = currentTime;

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

    char spawnName[0x40] = {0};
    memcpy(spawnName, (LPVOID)(spawnInfo + 0xE4), sizeof(spawnName));

    int spawnType = EQ_ReadMemory<BYTE>(spawnInfo + 0x125);

    if (spawnType == EQ_SPAWN_TYPE_PLAYER)
    {
        std::stringstream ss;
        ss << spawnName << " has entered the zone.";

        EQ_WriteToChat(ss.str().c_str(), EQ_TEXT_COLOR_YELLOW);
    }
    else if (spawnType == EQ_SPAWN_TYPE_NPC)
    {
        for (auto& namedSpawn : g_namedSpawnsList)
        {
            if (strstr(spawnName, namedSpawn.c_str()) != NULL)
            {
                std::stringstream ss;
                ss << spawnName << " has spawned.";

                EQ_WriteToChat(ss.str().c_str(), EQ_TEXT_COLOR_YELLOW);

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

    char spawnName[0x40] = {0};
    memcpy(spawnName, (LPVOID)(spawnInfo + 0xE4), sizeof(spawnName));

    int spawnType = EQ_ReadMemory<BYTE>(spawnInfo + 0x125);

    if (spawnType == EQ_SPAWN_TYPE_PLAYER)
    {
        if (EQ_IsSpawnInGroup(spawnInfo) == false)
        {
            std::stringstream ss;
            ss << spawnName << " has left the zone.";

            EQ_WriteToChat(ss.str().c_str(), EQ_TEXT_COLOR_YELLOW);
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

                EQ_WriteToChat(ss.str().c_str(), EQ_TEXT_COLOR_YELLOW);

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

    if (EQ_IsCastingSpell() == true)
    {
        return;
    }

    DWORD currentTime = EQ_GetTimer();

    if ((currentTime - g_combatHotbuttonTimer) < g_combatHotbuttonDelay)
    {
        return;
    }

    g_combatHotbuttonTimer = currentTime;

    EQ_CLASS_CHotButtonWnd->DoHotButton(g_combatHotbuttonIndex, 0);
}

void EQAPP_DoAlwaysHotbutton()
{
    if (g_alwaysHotbuttonIsEnabled == false)
    {
        return;
    }

    DWORD currentTime = EQ_GetTimer();

    if ((currentTime - g_alwaysHotbuttonTimer) < g_alwaysHotbuttonDelay)
    {
        return;
    }

    g_alwaysHotbuttonTimer = currentTime;

    EQ_CLASS_CHotButtonWnd->DoHotButton(g_alwaysHotbuttonIndex, 0);
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

void EQAPP_DoClip()
{
    if (g_clipIsEnabled == false)
    {
        return;
    }

    float clipMin = EQ_ReadMemory<FLOAT>(EQ_ZONEINFO_MIN_CLIP);
    float clipMax = EQ_ReadMemory<FLOAT>(EQ_ZONEINFO_MAX_CLIP);

    if (clipMin < g_clipMinimum || clipMax < g_clipMinimum)
    {
        EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_MIN_CLIP, g_clipMinimum);
        EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_MAX_CLIP, g_clipMinimum);

        EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP1, g_clipMinimum);
        EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP2, g_clipMinimum);
        EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP3, g_clipMinimum);
        EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP4, g_clipMinimum);
        EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP5, g_clipMinimum);
        EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP6, g_clipMinimum);
    }
}

void EQAPP_RemoveMapLabels()
{
    EQ_MapWindow_RemoveLabel(g_mapLabelsData);
}

void EQAPP_DoMapLabels()
{
    DWORD currentTime = EQ_GetTimer();

    if ((currentTime - g_mapLabelsTimer) < g_mapLabelsDelay)
    {
        return;
    }

    g_mapLabelsTimer = currentTime;

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

        int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125); 

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

        mapLabel.Layer = 0;
        mapLabel.Width = 1;
        mapLabel.Height = 12;

        mapLabel.Data = g_mapLabelsData;

        EQ_MapWindow_AddLabel(&mapLabel);

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }
}

void EQAPP_DoEsp()
{
    if (g_espIsEnabled == false)
    {
        return;
    }

    if (EQ_IsInGame() == false)
    {
        return;
    }

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

        char spawnName[0x40] = {0};
        memcpy(spawnName, (LPVOID)(spawn + 0xE4), sizeof(spawnName));

        FLOAT spawnY = EQ_ReadMemory<FLOAT>(spawn + 0x64);
        FLOAT spawnX = EQ_ReadMemory<FLOAT>(spawn + 0x68);
        FLOAT spawnZ = EQ_ReadMemory<FLOAT>(spawn + 0x6C);

        int spawnLevel = EQ_ReadMemory<BYTE>(spawn + 0x315);

        if (spawnLevel < 1 || spawnLevel > 100)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125);

        int spawnIsPet = EQ_ReadMemory<DWORD>(spawn + 0x260);
        int spawnIsLfg = EQ_ReadMemory<BYTE>(spawn + 0x1F1);
        int spawnIsPvp = EQ_ReadMemory<BYTE>(spawn + 0x2BC);
        int spawnIsTrader = EQ_ReadMemory<DWORD>(spawn + 0x228);
        int spawnIsAfk = EQ_ReadMemory<DWORD>(spawn + 0x284);
        int spawnIsGm = EQ_ReadMemory<BYTE>(spawn + 0x4DA);

        float spawnDistance = EQ_CalculateDistance3d(playerX, playerY, playerZ, spawnX, spawnY, spawnZ);

        int fontSize = 3;

        if (spawnDistance > (g_espSpawnDistance / 2.0f))
        {
            fontSize = 2;
        }

        bool showAtAnyDistance = false;

        if (g_findIsEnabled == true)
        {
            if (strstr(spawnName, g_findSpawnName.c_str()) != NULL && g_findSpawnName.size() != 0)
            {
                fontSize = 5;
                showAtAnyDistance = true;
            }
        }

        if (spawnIsPet == 0)
        {
            for (auto& namedSpawn : g_namedSpawnsList)
            {
                if (strstr(spawnName, namedSpawn.c_str()) != NULL)
                {
                    fontSize = 5;
                    showAtAnyDistance = true;
                    break;
                }
            }
        }

        if (spawnType == EQ_SPAWN_TYPE_NPC && spawnDistance > g_espSpawnDistance && showAtAnyDistance == false)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        int screenX = -1;
        int screenY = -1;
        bool result = EQ_WorldSpaceToScreenSpace(spawnX, spawnY, spawnZ, screenX, screenY);
        if (result == false)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        int textColor = 0xFFFFFFFF; // white

        if (spawnType == EQ_SPAWN_TYPE_PLAYER)
        {
            textColor = 0xFFFF0000; // red
        }
        else if (spawnType == EQ_SPAWN_TYPE_NPC)
        {
            textColor = 0xFF00FFFF; // cyan
        }
        else if (spawnType == EQ_SPAWN_TYPE_PLAYER_CORPSE)
        {
            textColor = 0xFFFF8000; // orange
        }
        else if (spawnType == EQ_SPAWN_TYPE_NPC_CORPSE)
        {
            textColor = 0xFFFFFF00; // yellow
        }

        if (EQ_IsSpawnInGroup(spawn) == true)
        {
            textColor = 0xFF00FF00; // green
        }

        if (spawn == targetSpawn || spawnIsGm == 1)
        {
            textColor = 0xFFFF00FF; // pink
        }

        std::stringstream ss;
        ss << "+ " << spawnName;

        if (spawnType == EQ_SPAWN_TYPE_PLAYER_CORPSE || spawnType == EQ_SPAWN_TYPE_NPC_CORPSE)
        {
            ss << " corpse";
        }

        ss << " L" << spawnLevel << " (" << (int)spawnDistance << ")";

        if (spawnIsLfg == 1)
        {
            ss << "  LFG";
        }

        if (spawnIsPvp == 1)
        {
            ss << "  PVP";
        }

        if (spawnIsTrader == 1)
        {
            ss << "  TRADER";
        }

        if (spawnIsAfk == 1)
        {
            ss << "  AFK";
        }

        if (spawnIsGm == 1)
        {
            ss << "  *GM*";
        }

        EQ_DrawText(ss.str().c_str(), screenX, screenY, textColor, fontSize);

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    // ground spawn
    spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_GROUND_SPAWN_INFO);

    while (spawn)
    {
        char spawnName[0x18] = {0};
        memcpy(spawnName, (LPVOID)(spawn + 0x38), sizeof(spawnName));

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

        std::string spawnNameEx = spawnName;

        auto find = EQ_KEYVALUE_ACTOR_DEFINITIONS.find(spawnName);
        if (find != EQ_KEYVALUE_ACTOR_DEFINITIONS.end())
        {
            spawnNameEx = find->second;
        }

        std::stringstream ss;
        ss << "+ " << spawnNameEx << " (" << (int)spawnDistance << ")";

        EQ_DrawText(ss.str().c_str(), screenX, screenY, 0xFFFFFFFF, 2);

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x04); // next
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
        }
    }
}

void EQAPP_DoInterpretCommand(const char* command)
{
    // help
    if (strcmp(command, "//help") == 0)
    {
        for (auto& slashCommand : g_slashCommands)
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

    // debug
    if (strcmp(command, "//debug") == 0)
    {
        EQ_ToggleBool(g_debugIsEnabled);

        std::cout << "Debug: " << std::boolalpha << g_debugIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // test
    if (strcmp(command, "//test") == 0)
    {
        //EQ_CLASS_CTextOverlay->DisplayText("Your spell was resisted!", EQ_TEXT_COLOR_RED, 15, 192, 100, 100, 5000); //192, 800, 100, 5000);
        //EQ_WriteToChat("test");

        std::cout << "test" << std::endl;

        return;
    }

    // cast ray
    if (strcmp(command, "//castray") == 0)
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
    if (strcmp(command, "//getmeleerange") == 0)
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
    if (strncmp(command, "//getspellname ", 15) == 0)
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

    // spawn list
    if (strcmp(command, "//spawnlist") == 0)
    {
        EQAPP_CoutSaveFlags();

        std::cout << "Spawn List:" << std::endl;

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

            int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125);

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

            std::cout << "L" << std::setfill('0') << std::setw(2) << spawnLevel << "    " << spawnName << " ";

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

            std::cout << std::endl;

            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
        }

        EQAPP_CoutRestoreFlags();

        return;
    }

    // esp
    if (strncmp(command, "//esp", 5) == 0)
    {
        if (strcmp(command, "//esp") == 0)
        {
            EQ_ToggleBool(g_espIsEnabled);

            std::cout << "ESP: " << std::boolalpha << g_espIsEnabled << std::noboolalpha << std::endl;
        }
        else if (strncmp(command, "//esp ", 6) == 0 || strncmp(command, "//espspawndistance ", 19) == 0)
        {
            char commandEx[128];

            float distance = 0.0f;

            int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);

            if (result == 2)
            {
                g_espSpawnDistance = distance;

                std::cout << "ESP Spawn Distance: " << distance << std::endl;
            }
        }
        else if (strncmp(command, "//espgroundspawndistance ", 25) == 0)
        {
            char commandEx[128];

            float distance = 0.0f;

            int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);

            if (result == 2)
            {
                g_espGroundSpawnDistance = distance;

                std::cout << "ESP Ground Spawn Distance: " << distance << std::endl;
            }
        }

        return;
    }

    // speed hack
    if (strncmp(command, "//speed", 7) == 0)
    {
        if (strcmp(command, "//speed") == 0)
        {
            EQ_ToggleBool(g_speedHackIsEnabled);

            std::cout << "Speed Hack: " << std::boolalpha << g_speedHackIsEnabled << std::noboolalpha << std::endl;
        }
        else if (strcmp(command, "//speed sow") == 0)
        {
            g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_SPIRIT_OF_WOLF;

            std::cout << "Speed Modifier:  Spirit of Wolf" << std::endl;
        }
        else if (strcmp(command, "//speed run1") == 0)
        {
            g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN1;

            std::cout << "Speed Modifier: AA Run 1" << std::endl;
        }
        else if (strcmp(command, "//speed run2") == 0)
        {
            g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN2;

            std::cout << "Speed Modifier: AA Run 2" << std::endl;
        }
        else if (strcmp(command, "//speed run3") == 0)
        {
            g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN3;

            std::cout << "Speed Modifier: AA Run 3" << std::endl;
        }
        else if (strncmp(command, "//speed ", 8) == 0)
        {
            char commandEx[128];

            float modifier = 0.0f;

            int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &modifier);

            if (result == 2)
            {
                g_speedHackModifier = modifier;

                std::cout << "Speed Modifier: " << g_speedHackModifier << std::endl;
            }
        }

        return;
    }

    // locator
    if (strncmp(command, "//loc", 5) == 0)
    {
        if (strcmp(command, "//loc") == 0)
        {
            EQ_ToggleBool(g_locatorIsEnabled);

            std::cout << "Locator: " << std::boolalpha << g_locatorIsEnabled << std::noboolalpha << std::endl;
        }
        else if (strncmp(command, "//loc ", 6) == 0)
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
        }

        return;
    }

    // find
    if (strncmp(command, "//find", 6) == 0)
    {
        if (strcmp(command, "//find") == 0)
        {
            EQ_ToggleBool(g_findIsEnabled);

            std::cout << "Find: " << std::boolalpha << g_findIsEnabled << std::noboolalpha << std::endl;
        }
        else if (strncmp(command, "//find ", 7) == 0)
        {
            char commandEx[128];

            char name[1024];

            int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));

            if (result == 2)
            {
                g_findSpawnName = name;

                std::cout << "Find: " << name << std::endl;

                g_findIsEnabled = true;
            }
        }

        return;
    }

    // line to target
    if (strcmp(command, "//linetotarget") == 0)
    {
        EQ_ToggleBool(g_lineToTargetIsEnabled);

        std::cout << "Line To Target: " << std::boolalpha << g_lineToTargetIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // memory list
    if (strcmp(command, "//memorylist") == 0)
    {
        std::cout << "Memory List:" << std::endl;

        for (auto& memory : g_memoryList)
        {
            std::cout << memory.index << " : " << memory.isEnabled << " : " << memory.name << " (" << memory.description << ")" << std::endl;
        }

        return;
    }

    // memory
    if (strncmp(command, "//memory ", 9) == 0)
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
    if (strcmp(command, "//alwaysattack") == 0)
    {
        EQ_ToggleBool(g_alwaysAttackIsEnabled);

        std::cout << "Always Attack: " << std::boolalpha << g_alwaysAttackIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // combat hotbutton
    if (strncmp(command, "//combathotbutton", 17) == 0)
    {
        if (strcmp(command, "//combathotbutton") == 0)
        {
            EQ_ToggleBool(g_combatHotbuttonIsEnabled);

            std::cout << "Combat Hotbutton: " << std::boolalpha << g_combatHotbuttonIsEnabled << std::noboolalpha << std::endl;
        }
        else if (strncmp(command, "//combathotbutton ", 18) == 0)
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
        }

        return;
    }

    // always hotbutton
    if (strncmp(command, "//alwayshotbutton", 17) == 0)
    {
        if (strcmp(command, "//alwayshotbutton") == 0)
        {
            EQ_ToggleBool(g_alwaysHotbuttonIsEnabled);

            std::cout << "Always Hotbutton: " << std::boolalpha << g_alwaysHotbuttonIsEnabled << std::noboolalpha << std::endl;
        }
        else if (strncmp(command, "//alwayshotbutton ", 18) == 0)
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
        }

        return;
    }

    // set collision radius
    if (strncmp(command, "//setcollisionradius ", 21) == 0)
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

    // clip
    if (strcmp(command, "//clip") == 0)
    {
        EQ_ToggleBool(g_clipIsEnabled);

        std::cout << "Clip: " << std::boolalpha << g_clipIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set clip
    if (strncmp(command, "//setclip ", 10) == 0)
    {
        char commandEx[128];

        float clip = 1000.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &clip);

        if (result == 2)
        {
            if (clip < 100.0f)
            {
                std::cout << "Specified clip too low: " << clip << std::endl;
                return;
            }

            EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_MIN_CLIP, clip);
            EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_MAX_CLIP, clip);

            EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP1, clip);
            EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP2, clip);
            EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP3, clip);
            EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP4, clip);
            EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP5, clip);
            EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_CLIP6, clip);

            std::cout << "Clip: " << clip << std::endl;
        }

        return;
    }

    // target beep
    if (strncmp(command, "//targetbeep", 12) == 0)
    {
        if (strcmp(command, "//targetbeep") == 0)
        {
            EQ_ToggleBool(g_targetBeepIsEnabled);

            std::cout << "Target Beep: " << std::boolalpha << g_targetBeepIsEnabled << std::noboolalpha << std::endl;
        }
        else if (strncmp(command, "//targetbeep ", 13) == 0)
        {
            char commandEx[128];

            char name[1024];

            unsigned int delay = 1;

            int result = sscanf_s(command, "%s %s %d", commandEx, sizeof(commandEx), name, sizeof(name), &delay);

            if (result == 3)
            {
                g_targetBeepName = name;

                g_targetBeepDelay = delay * 1000;

                std::cout << "Target Beep: " << name << " (" << delay << " second(s))" << std::endl;

                g_targetBeepIsEnabled = true;
            }
        }

        return;
    }

    // spawn beep
    if (strncmp(command, "//spawnbeep", 11) == 0)
    {
        if (strcmp(command, "//spawnbeep") == 0)
        {
            EQ_ToggleBool(g_spawnBeepIsEnabled);

            std::cout << "Spawn Beep: " << std::boolalpha << g_spawnBeepIsEnabled << std::noboolalpha << std::endl;
        }
        else if (strncmp(command, "//spawnbeep ", 12) == 0)
        {
            char commandEx[128];

            char name[1024];

            int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));

            if (result == 2)
            {
                g_spawnBeepName = name;

                std::cout << "Spawn Beep: " << name << std::endl;

                g_spawnBeepIsEnabled = true;
            }
        }

        return;
    }

    // height
    if (strcmp(command, "//height") == 0)
    {
        EQ_ToggleBool(g_heightIsEnabled);

        std::cout << "Height: " << std::boolalpha << g_heightIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // set height
    if (strncmp(command, "//setheight ", 12) == 0)
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

    // health bars
    if (strncmp(command, "//healthbars", 12) == 0)
    {
        if (strcmp(command, "//healthbars") == 0)
        {
            EQ_ToggleBool(g_healthBarsIsEnabled);

            std::cout << "Health Bars: " << std::boolalpha << g_healthBarsIsEnabled << std::noboolalpha << std::endl;
        }
        else if (strcmp(command, "//healthbarsreset") == 0)
        {
            g_healthBarsSpawnNamesList.clear();

            std::cout << "Health Bars spawn names reset." << std::endl;
        }
        else if (strncmp(command, "//healthbars ", 13) == 0)
        {
            char commandEx[128];

            char name[1024];

            int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));

            if (result == 2)
            {
                g_healthBarsSpawnNamesList.push_back(name);

                std::cout << "Health Bars spawn name added: " << name << std::endl;
            }
        }

        return;
    }

    // map labels
    if (strcmp(command, "//maplabels") == 0)
    {
        EQ_ToggleBool(g_mapLabelsIsEnabled);

        std::cout << "Map Labels: " << std::boolalpha << g_mapLabelsIsEnabled << std::noboolalpha << std::endl;

        if (g_mapLabelsIsEnabled == false)
        {
            EQAPP_RemoveMapLabels();
        }

        return;
    }

    // get map labels
    if (strcmp(command, "//getmaplabels") == 0)
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

    // get zone id
    if (strcmp(command, "//getzoneid") == 0)
    {
        DWORD zoneId = EQ_GetZoneId();

        std::cout << "Zone ID: " << zoneId << std::endl;

        return;
    }

    // get zone name
    if (strcmp(command, "//getzonename") == 0)
    {
        char zoneLongName[0x80] = {0};
        memcpy(zoneLongName, (LPVOID)(EQ_ZONEINFO_LONG_NAME), sizeof(zoneLongName));

        char zoneShortName[0x20] = {0};
        memcpy(zoneShortName, (LPVOID)(EQ_ZONEINFO_SHORT_NAME), sizeof(zoneShortName));

        std::cout << "Zone Name: " << zoneLongName << " (" << zoneShortName << ")" << std::endl;

        return;
    }

    // load memory
    if (strcmp(command, "//loadmemory") == 0)
    {
        EQAPP_UnloadMemory();
        EQAPP_LoadMemory();

        std::cout << "Loading memories..." << std::endl;

        return;
    }

    // load named spawns
    if (strcmp(command, "//loadnamedspawns") == 0)
    {
        EQAPP_LoadNamedSpawns();

        std::cout << "Loading named spawns..." << std::endl;

        return;
    }

    // named spawns
    if (strcmp(command, "//namedspawns") == 0)
    {
        EQ_ToggleBool(g_namedSpawnsIsEnabled);

        std::cout << "Named Spawns: " << std::boolalpha << g_namedSpawnsIsEnabled << std::noboolalpha << std::endl;

        return;
    }

    // load text overlay chat text
    if (strcmp(command, "//loadtextoverlaychattext") == 0)
    {
        EQAPP_LoadTextOverlayChatText();

        std::cout << "Loading text overlay chat text..." << std::endl;

        return;
    }

    // hide corpse looted
    if (strcmp(command, "//hidecorpselooted") == 0)
    {
        EQ_ToggleBool(g_hideCorpseLootedIsEnabled);

        std::cout << "Hide Corpse Looted: " << std::boolalpha << g_hideCorpseLootedIsEnabled << std::noboolalpha << std::endl;

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

    //

    return EQAPP_REAL_CEverQuest__SetGameState(pThis, a1);
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

    EQAPP_DoWindowTitles();

    EQAPP_DoTargetBeep();
    EQAPP_DoCombatHotbutton();
    EQAPP_DoAlwaysHotbutton();
    EQAPP_DoAlwaysAttack();
    EQAPP_DoMaxSwimming();
    EQAPP_DoHeight();
    EQAPP_DoClip();
    EQAPP_DoHideCorpseLooted();
    EQAPP_DoLineToTarget();
    EQAPP_DoEsp();
    EQAPP_DoHealthBars();
    EQAPP_DoNamedSpawns();

    EQAPP_DoHud();

    return EQAPP_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
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
        char spawnName[0x40] = {0};
        memcpy(spawnName, (LPVOID)(a1 + 0xE4), sizeof(spawnName));

        if (g_debugIsEnabled == true)
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

        EQAPP_DoSpawnBeep(a1);
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
            memcpy(spawnName, (LPVOID)(spawnInfo + 0xE4), sizeof(spawnName));

            if (g_debugIsEnabled == true)
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

    if (g_textOverlayOnChatTextIsEnabled == true)
    {
        for (auto& text : g_textOverlayChatTextList)
        {
            if (strstr(a1, text.c_str()) != NULL)
            {
                DWORD windowWidth  = EQ_ReadMemory<DWORD>(EQ_WINDOW_WIDTH);
                DWORD windowHeight = EQ_ReadMemory<DWORD>(EQ_WINDOW_HEIGHT);

                EQ_CLASS_CTextOverlay->DisplayText(a1, EQ_TEXT_COLOR_WHITE, 15, 192, (int)(windowWidth / 2), (int)(windowHeight / 3), 5000);
                break;
            }
        }
    }

    return EQAPP_REAL_CEverQuest__dsp_chat(pThis, a1, a2, a3);
}

int __fastcall EQAPP_DETOUR_CEverQuest__MoveToZone(void* pThis, void* not_used, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)//, int a9, int a10, int a11, int a12, int a13, int a14)
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

    EQAPP_LoadNamedSpawns();

    return result;
}

int __fastcall EQAPP_DETOUR_CEverQuest__InterpretCmd(void* pThis, void* not_used, class EQPlayer* a1, char* a2)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_CEverQuest__InterpretCmd(pThis, NULL, a2);
    }

    if (a1 == NULL || a2 == NULL)
    {
        return EQAPP_REAL_CEverQuest__InterpretCmd(pThis, NULL, a2);
    }

    if (strlen(a2) == 0)
    {
        return EQAPP_REAL_CEverQuest__InterpretCmd(pThis, NULL, a2);
    }

    std::string command = a2;
    const char* commandEx = command.c_str();

    if (strcmp(commandEx, "/exit") == 0)
    {
        EQAPP_DoUnload();
    }

    if (strlen(commandEx) > 2 && strncmp(commandEx, "//", 2) == 0)
    {
        EQAPP_DoInterpretCommand(commandEx);

        return EQAPP_REAL_CEverQuest__InterpretCmd(pThis, NULL, a2);
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

    if (g_neverFrozenIsEnabled == true)
    {
        if (a1 == EQ_STANDING_STATE_FROZEN)
        {
            a1 = EQ_STANDING_STATE_STANDING;
        }
    }

    return EQAPP_REAL_EQPlayer__ChangePosition(pThis, a1);
}

void EQAPP_AddDetours()
{
    EQAPP_REAL_Exit = (EQ_FUNCTION_TYPE_Exit)DetourFunction((PBYTE)EQ_FUNCTION_Exit, (PBYTE)EQAPP_DETOUR_Exit);

    EQAPP_REAL_DrawNetStatus = (EQ_FUNCTION_TYPE_DrawNetStatus)DetourFunction((PBYTE)EQ_FUNCTION_DrawNetStatus, (PBYTE)EQAPP_DETOUR_DrawNetStatus);

    EQAPP_REAL_CDisplay__CreatePlayerActor = (EQ_FUNCTION_TYPE_CDisplay__CreatePlayerActor)DetourFunction((PBYTE)EQ_FUNCTION_CDisplay__CreatePlayerActor, (PBYTE)EQAPP_DETOUR_CDisplay__CreatePlayerActor);
    EQAPP_REAL_CDisplay__DeleteActor = (EQ_FUNCTION_TYPE_CDisplay__DeleteActor)DetourFunction((PBYTE)EQ_FUNCTION_CDisplay__DeleteActor, (PBYTE)EQAPP_DETOUR_CDisplay__DeleteActor);

    EQAPP_REAL_CEverQuest__dsp_chat = (EQ_FUNCTION_TYPE_CEverQuest__dsp_chat)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__dsp_chat, (PBYTE)EQAPP_DETOUR_CEverQuest__dsp_chat);
    EQAPP_REAL_CEverQuest__EnterZone = (EQ_FUNCTION_TYPE_CEverQuest__EnterZone)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__EnterZone, (PBYTE)EQAPP_DETOUR_CEverQuest__EnterZone);
    //EQAPP_REAL_CEverQuest__InterpretCmd = (EQ_FUNCTION_TYPE_CEverQuest__InterpretCmd)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__InterpretCmd, (PBYTE)EQAPP_DETOUR_CEverQuest__InterpretCmd);
    EQAPP_REAL_CEverQuest__MoveToZone = (EQ_FUNCTION_TYPE_CEverQuest__MoveToZone)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__MoveToZone, (PBYTE)EQAPP_DETOUR_CEverQuest__MoveToZone);
    EQAPP_REAL_CEverQuest__SetGameState = (EQ_FUNCTION_TYPE_CEverQuest__SetGameState)DetourFunction((PBYTE)EQ_FUNCTION_CEverQuest__SetGameState, (PBYTE)EQAPP_DETOUR_CEverQuest__SetGameState);

    EQAPP_REAL_CMapViewWnd__DrawMap = (EQ_FUNCTION_TYPE_CMapViewWnd__DrawMap)DetourFunction((PBYTE)EQ_FUNCTION_CMapViewWnd__DrawMap, (PBYTE)EQAPP_DETOUR_CMapViewWnd__DrawMap);

    EQAPP_REAL_EQ_Character__eqspa_movement_rate = (EQ_FUNCTION_TYPE_EQ_Character__eqspa_movement_rate)DetourFunction((PBYTE)EQ_FUNCTION_EQ_Character__eqspa_movement_rate, (PBYTE)EQAPP_DETOUR_EQ_Character__eqspa_movement_rate);

    EQAPP_REAL_EQPlayer__ChangePosition = (EQ_FUNCTION_TYPE_EQPlayer__ChangePosition)DetourFunction((PBYTE)EQ_FUNCTION_EQPlayer__ChangePosition, (PBYTE)EQAPP_DETOUR_EQPlayer__ChangePosition);
}

void EQAPP_RemoveDetours()
{
    DetourRemove((PBYTE)EQAPP_REAL_Exit, (PBYTE)EQAPP_DETOUR_Exit);

    DetourRemove((PBYTE)EQAPP_REAL_DrawNetStatus, (PBYTE)EQAPP_DETOUR_DrawNetStatus);

    DetourRemove((PBYTE)EQAPP_REAL_CDisplay__CreatePlayerActor, (PBYTE)EQAPP_DETOUR_CDisplay__CreatePlayerActor);
    DetourRemove((PBYTE)EQAPP_REAL_CDisplay__DeleteActor, (PBYTE)EQAPP_DETOUR_CDisplay__DeleteActor);

    DetourRemove((PBYTE)EQAPP_REAL_CEverQuest__dsp_chat, (PBYTE)EQAPP_DETOUR_CEverQuest__dsp_chat);
    DetourRemove((PBYTE)EQAPP_REAL_CEverQuest__EnterZone, (PBYTE)EQAPP_DETOUR_CEverQuest__EnterZone);
    //DetourRemove((PBYTE)EQAPP_REAL_CEverQuest__InterpretCmd, (PBYTE)EQAPP_DETOUR_CEverQuest__InterpretCmd);
    DetourRemove((PBYTE)EQAPP_REAL_CEverQuest__MoveToZone, (PBYTE)EQAPP_DETOUR_CEverQuest__MoveToZone);
    DetourRemove((PBYTE)EQAPP_REAL_CEverQuest__SetGameState, (PBYTE)EQAPP_DETOUR_CEverQuest__SetGameState);

    DetourRemove((PBYTE)EQAPP_REAL_CMapViewWnd__DrawMap, (PBYTE)EQAPP_DETOUR_CMapViewWnd__DrawMap);

    DetourRemove((PBYTE)EQAPP_REAL_EQ_Character__eqspa_movement_rate, (PBYTE)EQAPP_DETOUR_EQ_Character__eqspa_movement_rate);

    DetourRemove((PBYTE)EQAPP_REAL_EQPlayer__ChangePosition, (PBYTE)EQAPP_DETOUR_EQPlayer__ChangePosition);
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
