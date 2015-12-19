#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <psapi.h>

#define APPLICATION_NAME "EverQuest Inject DLL"

#define EQ_EXECUTABLE_NAME "eqgame.exe"
#define EQ_PROCESS_NAME    "eqgame.exe"

#define EQ_WINDOW_TITLE     "EverQuest"
#define EQ_WINDOW_TITLE_EQW "EQW beta 2.32"

int file_exists(char *filename)
{
    FILE *file = fopen(filename, "r");

    int result = (file == NULL ? 0 : 1);

    fclose(file);

    return result;
}

void enable_debug_privileges()
{
    HANDLE token;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
    {
        TOKEN_PRIVILEGES tp;
        TOKEN_PRIVILEGES tp_previous;

        DWORD cb_previous = sizeof(TOKEN_PRIVILEGES);

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
                &tp_previous,
                &cb_previous
            );

            tp_previous.PrivilegeCount            = 1;
            tp_previous.Privileges[0].Luid        = luid;
            tp_previous.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
    
            AdjustTokenPrivileges
            (
                token,
                FALSE,
                &tp_previous,
                cb_previous,
                NULL,
                NULL
            );
        }
    }

    CloseHandle(token);
}

int main(int argc, char *argv[])
{
    printf("%s\n", APPLICATION_NAME);

    if (argc < 2)
    {
        printf("Usage: %s DLLNAME.dll\n", argv[0]);
        return 0;
    }

    char *dll_name = argv[1];

    if (strlen(dll_name) == 0)
    {
        printf("Error: Injected DLL filename must not be blank.\n");
        printf("Usage: %s DLLNAME.dll\n", argv[0]);
        return 0;
    }

    if ((strstr(dll_name, ".dll") == NULL) && (strstr(dll_name, ".DLL") == NULL))
    {
        printf("Error: Injected DLL filename must have a *.dll extension.\n");
        return 0;
    }

    if (file_exists(dll_name) == 0)
    {
        printf("Error: Injected DLL filename '%s' file not found.\nThe DLL needs to be in the same folder as this program or the full path to the DLL must be specified.", dll_name);
        return 0;
    }

    printf("DLL Name: %s\n", argv[1]);

    if (file_exists(EQ_EXECUTABLE_NAME) == 0)
    {
        printf("Error: EverQuest executable '%s' file not found.\n", EQ_EXECUTABLE_NAME);
    }

    HWND eq_window = FindWindowA(NULL, EQ_WINDOW_TITLE);
    if (eq_window == NULL)
    {
        eq_window = FindWindowA(NULL, EQ_WINDOW_TITLE_EQW);
        if (eq_window == NULL)
        {
            printf("Error: EverQuest window not found.\n");
        }
    }

    printf("Enabling debug privileges.\n");

    enable_debug_privileges();

    DWORD processes[1024];
    DWORD needed;
    if (!EnumProcesses(processes, sizeof(processes), &needed))
    {
        return 0;
    }

    DWORD num_processes = needed / sizeof(DWORD);

    size_t i = 0;
    for (i = 0; i < num_processes; i++)
    {
        if (processes[i] != 0)
        {
            HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processes[i]);

            if (process_handle != NULL)
            {
                HMODULE module;
                DWORD needed2;
                if (EnumProcessModules(process_handle, &module, sizeof(module), &needed2))
                {
                    char module_name[1024] = {0};
                    GetModuleBaseNameA(process_handle, module, module_name, sizeof(module_name));

                    if (strcmp(module_name, EQ_PROCESS_NAME) == 0)
                    {
                        int is_dll_already_injected = 0;

                        HMODULE modules[1024];
                        DWORD needed3;
                        if (EnumProcessModules(process_handle, modules, sizeof(modules), &needed3))
                        {
                            DWORD num_modules = needed3 / sizeof(HMODULE);

                            size_t j = 0;
                            for (j = 0; j < num_modules; j++)
                            {
                                char module_name[1024] = {0};
                                GetModuleBaseNameA(process_handle, modules[j], module_name, sizeof(module_name));

                                //printf("Module Name: %s\n", module_name);

                                if (strcmp(module_name, dll_name) == 0)
                                {
                                    printf("DLL is already injected in EverQuest. (Name: %s, ID: %d | 0x%08X)\nSkipping...\n", module_name, (int)processes[i], (int)processes[i]);

                                    is_dll_already_injected = 1;

                                    break;
                                }
                            }
                        }

                        if (is_dll_already_injected == 0)
                        {

                        printf("EverQuest process found. (Name: %s, ID: %d | 0x%08X)\nInjecting DLL...\n", module_name, (int)processes[i], (int)processes[i]);

                        char dll_path_name[1024] = {0};
                        GetFullPathNameA(dll_name, 1024, dll_path_name, NULL);

                        LPVOID remote_memory = VirtualAllocEx(process_handle, NULL, strlen(dll_path_name), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

                        WriteProcessMemory(process_handle, remote_memory, dll_path_name, strlen(dll_path_name), NULL);

                        HANDLE remote_thread = CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32"), "LoadLibraryA"), remote_memory, 0, NULL);

                        //WaitForSingleObject(remote_thread, INFINITE);

                        //VirtualFreeEx(process_handle, remote_memory, strlen(dll_path_name), MEM_RELEASE);

                        CloseHandle(remote_thread);

                        }
                    }
                }
            }

            CloseHandle(process_handle);
        }
    }

    printf("Done.\n");

    return EXIT_SUCCESS;
}

/*
    DWORD process_id;
    GetWindowThreadProcessId(everquest_window, &process_id);

    HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);

    char dll_path_name[MAX_PATH] = {0};
    GetFullPathName(APPLICATION_DLL_NAME, MAX_PATH, dll_path_name, NULL);

    LPVOID remote_memory = VirtualAllocEx(process_handle, NULL, strlen(dll_path_name), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    WriteProcessMemory(process_handle, remote_memory, dll_path_name, strlen(dll_path_name), NULL);

    HANDLE remote_thread = CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA"), remote_memory, 0, NULL);

    WaitForSingleObject(remote_thread, INFINITE);

    VirtualFreeEx(process_handle, remote_memory, strlen(dll_path_name), MEM_RELEASE);

    CloseHandle(remote_thread);
    CloseHandle(process_handle);
*/
