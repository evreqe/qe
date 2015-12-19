#include <windows.h>

#include "vk_keys.h"

#include "memory.hpp"

#include "eqdalaya.hpp"

#define APPLICATION_NAME "EQDalaya Keys"

bool exit_dll = false;

memory mem;

HANDLE thread_handle_load;
HANDLE thread_handle_keys;

void thread_keys(HMODULE module)
{
    while (1)
    {

        if (exit_dll == true)
        {
            break;
        }

        if (mem.is_foreground_window_current_process_id() == true)
        {
            if
            (
                (KEYDOWN(VK_CONTROL)) &&
                (KEYDOWN(VK_ALT)) &&
                (KEYDOWN(VK_BACK))
            )
            {
                exit_dll = true;
                break;
            }

            Sleep(100);
            continue;
        }

        if (KEYDOWN(VK_CONTROL))
        {
            if (KEYDOWN(VK_1))
            {
                everquest_function_CHotButtonWnd__DoHotButton(0, 1);
                Sleep(1000);
            }

            if (KEYDOWN(VK_2))
            {
                everquest_function_CHotButtonWnd__DoHotButton(1, 1);
                Sleep(1000);
            }

            if (KEYDOWN(VK_3))
            {
                everquest_function_CHotButtonWnd__DoHotButton(2, 1);
                Sleep(1000);
            }

            if (KEYDOWN(VK_4))
            {
                everquest_function_CHotButtonWnd__DoHotButton(3, 1);
                Sleep(1000);
            }

            if (KEYDOWN(VK_5))
            {
                everquest_function_CHotButtonWnd__DoHotButton(4, 1);
                Sleep(1000);
            }

            if (KEYDOWN(VK_6))
            {
                everquest_function_CHotButtonWnd__DoHotButton(5, 1);
                Sleep(1000);
            }

            if (KEYDOWN(VK_7))
            {
                everquest_function_CHotButtonWnd__DoHotButton(6, 1);
                Sleep(1000);
            }

            if (KEYDOWN(VK_8))
            {
                everquest_function_CHotButtonWnd__DoHotButton(7, 1);
                Sleep(1000);
            }

            if (KEYDOWN(VK_9))
            {
                everquest_function_CHotButtonWnd__DoHotButton(8, 1);
                Sleep(1000);
            }

            if (KEYDOWN(VK_0))
            {
                everquest_function_CHotButtonWnd__DoHotButton(9, 1);
                Sleep(1000);
            }
        }

        Sleep(10);
    }

    if (exit_dll == true)
    {
        //everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " unloaded.");

        Sleep(1000);

        FreeLibraryAndExitThread(module, 0);

        return;
    }
}

void thread_load(HMODULE module)
{
    mem.enable_debug_privileges();
    mem.set_process_by_id(GetCurrentProcessId());

    if (mem.get_process_id() == 0)
    {
        return;
    }

    thread_handle_keys = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_keys, module, 0, NULL);

    //everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " loaded.");

    return;
}

extern "C" BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(module);
            thread_handle_load = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_load, module, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
