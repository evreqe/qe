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

void EQAPP_COUT_SaveFlags();
void EQAPP_COUT_RestoreFlags();

int EQAPP_INI_ReadInt(const char* filename, const char* section, const char* key, int defaultValue);
bool EQAPP_INI_ReadBool(const char* filename, const char* section, const char* key, int defaultValue);
float EQAPP_INI_ReadFloat(const char* filename, const char* section, const char* key, float defaultValue);
std::string EQAPP_INI_ReadString(const char* filename, const char* section, const char* key, const char* defaultValue);

void EQAPP_LoadGraphicsDllFunctions();
bool EQAPP_IsAnImportantWindowOpen();

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

#endif // EQAPP_FUNCTIONS_H