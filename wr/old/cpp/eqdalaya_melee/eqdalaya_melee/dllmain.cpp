#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>

#include <boost/algorithm/string.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <boost/range/algorithm.hpp>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <windows.h>

#include "vk_keys.h"

#include "memory.hpp"

#include "eqdalaya.hpp"

const std::string ini_file = "eqdalaya_melee.ini";

#define APPLICATION_NAME "EQDalaya Melee"

bool exit_dll = false;

memory mem;

HANDLE thread_handle_load;
HANDLE thread_handle_keys;
HANDLE thread_handle_melee;

bool melee_activated = false;

bool melee_enabled = true;

int melee_hot_button = 1;

void toggle_bool(bool &b)
{
    b = !b;
}

bool check_if_sitting()
{
    int player_spawn_info = everquest_get_player_spawn_info(mem);

    BYTE player_standing_state = mem.read_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE);

    if (player_standing_state == EVERQUEST_STANDING_STATE_SITTING)
    {
        return true;
    }

    return false;
}

int fix_hot_button_number(int hot_button)
{
    if (hot_button > 0 && hot_button < 10)
    {
        return hot_button - 1;
    }
    else
    {
        return 9;
    }
}

void load_config()
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_file, pt);

    melee_enabled = pt.get<bool>("Melee.bEnabled", melee_enabled);

    melee_hot_button = pt.get<int>("Melee.iHotButton", melee_hot_button);

    melee_hot_button = fix_hot_button_number(melee_hot_button);
}

void wait_for_thread(HANDLE thread)
{
    if (thread == NULL)
    {
        return;
    }

    DWORD exit_code;
    GetExitCodeThread(thread, &exit_code);

    if (exit_code == STILL_ACTIVE)
    {
        TerminateThread(thread, 0);
        CloseHandle(thread);
    }
}

void thread_keys(HMODULE module)
{
    while (1)
    {

        if (exit_dll == true)
        {
            break;
        }

        if (mem.is_foreground_window_current_process_id() == false)
        {
            Sleep(100);
            continue;
        }

        if
        (
            (KEYDOWN(VK_CONTROL)) &&
            (KEYDOWN(VK_ALT)) &&
            (KEYDOWN(VK_BACK))
        )
        {
            if (melee_activated == false)
            {
                exit_dll = true;
                break;
            }
        }

        if
        (
            (KEYDOWN(VK_CONTROL)) &&
            (KEYDOWN(VK_ALT)) &&
            (KEYDOWN(VK_W))
        )
        {
            toggle_bool(melee_activated);

            //if (melee_activated == true)
            //{
                //everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " activated.");
            //}
            //else
            //{
                //everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " deactivated.");
            //}

            Sleep(1000);
        }

        Sleep(10);
    }

    if (exit_dll == true)
    {
        //everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " unloaded.");

        Sleep(1000);

        wait_for_thread(thread_handle_melee);

        FreeLibraryAndExitThread(module, 0);

        return;
    }
}

void thread_melee(HMODULE module)
{
    while (1)
    {

        if (exit_dll == true)
        {
            return;
        }

        if (melee_enabled == false)
        {
            Sleep(100);
            continue;
        }

        if (melee_activated == false)
        {
            Sleep(100);
            continue;
        }

        if (check_if_sitting() == true)
        {
            Sleep(100);
            continue;
        }

        bool is_auto_attack_enabled = everquest_is_auto_attack_enabled(mem);

        if (is_auto_attack_enabled == false)
        {
            Sleep(100);
            continue;
        }

        int target_spawn_info = everquest_get_target_spawn_info(mem);

        if (target_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            Sleep(100);
            continue;
        }

        int player_spawn_info = everquest_get_player_spawn_info(mem);

        float player_y = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        float player_x = mem.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);

        float target_y = mem.read_any<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        float target_x = mem.read_any<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);

        float target_distance = everquest_calculate_distance(player_x, player_y, target_x, target_y);

        if (target_distance > 15)
        {
            Sleep(100);
            continue;
        }

        everquest_function_CHotButtonWnd__DoHotButton(melee_hot_button, 1);

        Sleep(100);
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

    load_config();

    thread_handle_keys  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_keys,  module, 0, NULL);
    thread_handle_melee = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_melee, module, 0, NULL);

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