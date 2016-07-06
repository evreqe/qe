#ifndef EQAPP_FUNCTIONS_H
#define EQAPP_FUNCTIONS_H

#include "eqapp.h"

template <class T>
void EQAPP_Log(const char* text, T number);

void EQAPP_EnableDebugPrivileges();
DWORD EQAPP_GetModuleBaseAddress(const wchar_t* moduleName);

void EQAPP_CenterWindow(HWND hwnd);
bool EQAPP_IsForegroundWindowCurrentProcessId();

void EQAPP_PrintBool(const char* text, bool& b);
void EQAPP_IMGUI_TextBool(bool& b);

void EQAPP_PrintFunctionError(const char* functionName, const char* errorText);

void EQAPP_COUT_SaveFlags();
void EQAPP_COUT_RestoreFlags();

int EQAPP_INI_ReadInt(const char* filename, const char* section, const char* key, int defaultValue);
bool EQAPP_INI_ReadBool(const char* filename, const char* section, const char* key, int defaultValue);
float EQAPP_INI_ReadFloat(const char* filename, const char* section, const char* key, float defaultValue);
std::string EQAPP_INI_ReadString(const char* filename, const char* section, const char* key, const char* defaultValue);

void EQAPP_LoadGraphicsDllFunctions();
bool EQAPP_IsAnImportantWindowOpen();

void EQAPP_LootDebugInformation_Print();
void EQAPP_SpawnInformation_Print(DWORD spawnInfo);
void EQAPP_BankCurrency_Print();
void EQAPP_CastRayToTarget_Print();
void EQAPP_MeleeRangeToTarget_Print();
void EQAPP_ZoneInformation_Print();

void EQAPP_OpenZoneMapFile();

//****************************************************************************************************//

void EQAPP_Load();
void EQAPP_Unload();

void EQAPP_SetWindowTitles();

//****************************************************************************************************//

template <class T>
void EQAPP_Log(const char* text, T number)
{
    std::fstream file;
    file.open("eqapp/log.txt", std::ios::out | std::ios::app);
    file << "[" << __TIME__ << "] " << text << " (" << number << ")" << " Hex(" << std::hex << number << std::dec << ")" << std::endl;
    file.close();
}

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

            AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &tpPrevious, &cbPrevious);

            tpPrevious.PrivilegeCount            = 1;
            tpPrevious.Privileges[0].Luid        = luid;
            tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);

            AdjustTokenPrivileges(token, FALSE, &tpPrevious, cbPrevious, NULL, NULL);
        }
    }

    CloseHandle(token);
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

void EQAPP_CenterWindow(HWND hwnd)
{
    if (hwnd == NULL)
    {
        return;
    }

    HWND desktopWindow = GetDesktopWindow();
    if (desktopWindow == NULL)
    {
        return;
    }

    RECT desktopRect;
    GetWindowRect(desktopWindow, &desktopRect);

    RECT hwndRect;
    GetWindowRect(hwnd, &hwndRect);

    hwndRect.left = (LONG)(desktopRect.left + ((desktopRect.right  - desktopRect.left) - (hwndRect.right  - hwndRect.left)) * 0.5f);
    hwndRect.top  = (LONG)(desktopRect.top  + ((desktopRect.bottom - desktopRect.top)  - (hwndRect.bottom - hwndRect.top))  * 0.5f);

    HWND taskbarWindow = FindWindowA("Shell_TrayWnd", NULL);
    if (taskbarWindow == NULL)
    {
        return;
    }

    RECT taskbarRect;
    GetWindowRect(taskbarWindow, &taskbarRect);

    hwndRect.top = (LONG)(hwndRect.top - (taskbarRect.bottom - taskbarRect.top) * 0.5f);

    SetWindowPos(hwnd, HWND_DESKTOP, hwndRect.left, hwndRect.top, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
    UpdateWindow(hwnd);
}

bool EQAPP_IsForegroundWindowCurrentProcessId()
{
    HWND foregroundHwnd = GetForegroundWindow();

    DWORD foregroundProcessId;
    GetWindowThreadProcessId(foregroundHwnd, &foregroundProcessId);

    return (foregroundProcessId == GetCurrentProcessId());
}

void EQAPP_PrintBool(const char* text, bool& b)
{
    std::cout << text << ": ";

    if (b == true)
    {
        std::cout << "On";
    }
    else
    {
        std::cout << "Off";
    }

    std::cout << std::endl;
}

void EQAPP_IMGUI_TextBool(bool& b)
{
    if (b == false)
    {
        ImGui::Text("Off");
    }
    else
    {
        ImGui::Text("On");
    }
}

void EQAPP_PrintFunctionError(const char* functionName, const char* errorText)
{
    std::cout << "[error] " << functionName << ": " << errorText << std::endl;
}

void EQAPP_COUT_SaveFlags()
{
    g_coutFlags.copyfmt(std::cout);
}

void EQAPP_COUT_RestoreFlags()
{
    std::cout.copyfmt(g_coutFlags);
}

int EQAPP_INI_ReadInt(const char* filename, const char* section, const char* key, int defaultValue)
{
    return GetPrivateProfileIntA(section, key, defaultValue, filename);
}

bool EQAPP_INI_ReadBool(const char* filename, const char* section, const char* key, int defaultValue)
{
    int result = GetPrivateProfileIntA(section, key, defaultValue, filename);

    return (result != 0);
}

float EQAPP_INI_ReadFloat(const char* filename, const char* section, const char* key, float defaultValue)
{
    char bufferDefault[128];
    _snprintf_s(bufferDefault, sizeof(bufferDefault), _TRUNCATE, "%f", defaultValue);

    char bufferResult[128];
    GetPrivateProfileStringA(section, key, bufferDefault, bufferResult, sizeof(bufferResult), filename);

    float result = (float)atof(bufferResult);

    return result;
}

std::string EQAPP_INI_ReadString(const char* filename, const char* section, const char* key, const char* defaultValue)
{
    char result[1024];
    GetPrivateProfileStringA(section, key, defaultValue, result, 1024, filename);
    return result;
}

void EQAPP_LoadGraphicsDllFunctions()
{
    DWORD baseAddress = EQ_ReadMemory<DWORD>(EQ_POINTER_GRAPHICS_DLL_BASE_ADDRESS);

    DWORD addressDrawLine = baseAddress + EQ_GRAPHICS_DLL_OFFSET_DrawLine;
    EQGraphicsDLL__DrawLine = (EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawLine)addressDrawLine;

    DWORD addressDrawQuad = baseAddress + EQ_GRAPHICS_DLL_OFFSET_DrawQuad;
    EQGraphicsDLL__DrawQuad = (EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawQuad)addressDrawQuad;
}

bool EQAPP_IsAnImportantWindowOpen()
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

void EQAPP_LootDebugInformation_Print()
{
    std::cout << "Loot Debug Information:" << std::endl;

    DWORD lootWindow = EQ_ReadMemory<DWORD>(EQ_POINTER_CLootWnd);
    if (lootWindow == NULL)
    {
        std::cout << "Loot Window is NULL." << std::endl;
        return;
    }

    DWORD lootWindowIsVisible = EQ_ReadMemory<BYTE>(lootWindow + 0x124);
    if (lootWindowIsVisible == 0)
    {
        std::cout << "Loot Window is NOT open." << std::endl;
        return;
    }

    for (size_t i = 0; i < EQ_NUM_LOOT_WINDOW_SLOTS; i++)
    {
        DWORD itemInfo = EQ_ReadMemory<DWORD>(lootWindow + (0x228 + (i * 4)));
        if (itemInfo == NULL)
        {
            std::cout << "#" << i << ": itemInfo == NULL" << std::endl;
            continue;
        }

        PCHAR itemName = EQ_ReadMemory<PCHAR>(itemInfo + 0xB8);
        if (itemName == NULL)
        {
            std::cout << "#" << i << ": itemName == NULL" << std::endl;
            continue;
        }

        std::cout << "#" << i << ": " << itemName << std::endl;
    }
}

void EQAPP_SpawnInformation_Print(DWORD spawnInfo)
{
    if (spawnInfo == NULL)
    {
        EQAPP_PrintFunctionError(__FUNCTION__ , " spawn is NULL");
        return;
    }

    std::cout << "Spawn Information:" << std::endl;

    // name

    char spawnName[0x40] = {0};
    memcpy(spawnName, (LPVOID)(spawnInfo + 0xA4), sizeof(spawnName));

    std::cout << "NAME:  " << spawnName;

    int spawnType = EQ_ReadMemory<BYTE>(spawnInfo + 0x125);
    if (spawnType != EQ_SPAWN_TYPE_PLAYER)
    {
        char spawnNameEx[0x40] = {0};
        memcpy(spawnNameEx, (LPVOID)(spawnInfo + 0xE4), sizeof(spawnNameEx));

        std::cout << " (" << spawnNameEx << ")";
    }

    std::cout << std::endl;

    // level

    int spawnLevel = EQ_ReadMemory<BYTE>(spawnInfo + 0x315);

    std::cout << "LEVEL: " << spawnLevel << std::endl;

    // guild

    int spawnGuildId = EQ_ReadMemory<DWORD>(spawnInfo + 0x30C);

    const char* spawnGuildName = EQ_EQ_Guilds.GetGuildNameById(spawnGuildId);

    std::cout << "GUILD: " << spawnGuildName << " (" << spawnGuildId << ")" << std::endl;

    // race

    int spawnRace = EQ_ReadMemory<DWORD>(spawnInfo + 0xE64);

    const char* spawnRaceDescription = EQ_CEverQuest->GetRaceDesc(spawnRace);

    std::cout << "RACE:  " << spawnRaceDescription << " (" << spawnRace << ")" << std::endl;

    // class

    int spawnClass = EQ_ReadMemory<BYTE>(spawnInfo + 0xE68);

    const char* spawnClassDescription = EQ_CEverQuest->GetClassDesc(spawnClass);

    std::cout << "CLASS: " << spawnClassDescription << " (" << spawnClass << ")" << std::endl;

    // deity

    int spawnDeity = EQ_ReadMemory<DWORD>(spawnInfo + 0xD4D);

    const char* spawnDeityDescription = EQ_CEverQuest->GetDeityDesc(spawnDeity);

    std::cout << "DEITY: " << spawnDeityDescription << " (" << spawnDeity << ")" << std::endl;

    // body type description

    const char* spawnBodyTypeDescription = EQ_CEverQuest->GetBodyTypeDesc(spawnInfo + 0x128); // this function requires an offset!

    std::cout << "BANE:  " << spawnBodyTypeDescription << std::endl;
}

void EQAPP_BankCurrency_Print()
{
    DWORD charInfo = EQ_ReadMemory<DWORD>(EQ_POINTER_CHAR_INFO);
    if (charInfo == NULL)
    {
        return;
    }

    DWORD bankPlatinum = EQ_ReadMemory<DWORD>(charInfo + 0xF5A4);
    DWORD bankGold     = EQ_ReadMemory<DWORD>(charInfo + 0xF5A8);
    DWORD bankSilver   = EQ_ReadMemory<DWORD>(charInfo + 0xF5AC);
    DWORD bankCopper   = EQ_ReadMemory<DWORD>(charInfo + 0xF5B0);

    std::cout << "You have "
              << bankPlatinum << "p "
              << bankGold     << "g "
              << bankSilver   << "s "
              << bankCopper   << "c in the bank."
              << std::endl;
}

void EQAPP_CastRayToTarget_Print()
{
    DWORD playerSpawn = EQ_GetPlayerSpawn();
    if (playerSpawn != NULL)
    {
        DWORD targetSpawn = EQ_GetTargetSpawn();
        if (targetSpawn == NULL)
        {
            std::cout << "[error] " << __FUNCTION__ ": target not found" << std::endl;
            return;
        }

        FLOAT targetY = EQ_ReadMemory<FLOAT>(targetSpawn + 0x64);
        FLOAT targetX = EQ_ReadMemory<FLOAT>(targetSpawn + 0x68);
        FLOAT targetZ = EQ_ReadMemory<FLOAT>(targetSpawn + 0x6C);

        int result = EQ_CastRay(playerSpawn, targetY, targetX, targetZ);

        std::string resultString;
        if (result == 0)
        {
            resultString = "Failed";
        }
        else
        {
            resultString = "Succeeded";
        }

        std::cout << "Cast Ray to Target result: " << resultString << std::endl;
    }
}

void EQAPP_MeleeRangeToTarget_Print()
{
    DWORD playerSpawn = EQ_GetPlayerSpawn();
    if (playerSpawn != NULL)
    {
        DWORD targetSpawn = EQ_GetTargetSpawn();
        if (targetSpawn == NULL)
        {
            std::cout << "[error] " << __FUNCTION__ << ": target not found" << std::endl;
            return;
        }

        FLOAT meleeRange = EQ_get_melee_range(playerSpawn, targetSpawn);

        std::cout << "Melee Range to Target: " << meleeRange << std::endl;
    }
}

void EQAPP_ZoneInformation_Print()
{
    DWORD zoneId = EQ_GetZoneId();

    std::string zoneLongName = EQ_GetZoneLongName();
    if (zoneLongName.size() == 0)
    {
        EQAPP_PrintFunctionError(__FUNCTION__, "zone long name is NULL");
        return;
    }

    std::string zoneShortName = EQ_GetZoneShortName();
    if (zoneShortName.size() == 0)
    {
        EQAPP_PrintFunctionError(__FUNCTION__, "zone short name is NULL");
        return;
    }

    std::cout << "Zone Information: " << zoneLongName << " (" << zoneShortName << ") " << "(ID: " << zoneId << ")" << std::endl;
}

void EQAPP_OpenZoneMapFile()
{
    std::string zoneShortName = EQ_GetZoneShortName();
    if (zoneShortName.size() == 0)
    {
        EQAPP_PrintFunctionError(__FUNCTION__, "zone short name is NULL");
        return;
    }

    std::stringstream filePath;
    filePath << "maps\\" << zoneShortName << ".txt";

    ShellExecuteA(0, "open", filePath.str().c_str(), 0, 0, SW_SHOW);

    std::cout << "Opening zone map file: " << filePath.str() << std::endl;

    for (size_t i = 1; i < (EQ_NUM_MAP_LAYERS + 1); i++)
    {
        std::stringstream filePath;
        filePath << "maps\\" << zoneShortName << "_" << i << ".txt";

        ShellExecuteA(0, "open", filePath.str().c_str(), 0, 0, SW_SHOW);

        std::cout << "Opening zone map file: " << filePath.str() << std::endl;
    }
}

#endif // EQAPP_FUNCTIONS_H
