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

const std::string ini_file = "eqdalaya_warrior_auto.ini";

#define APPLICATION_NAME "EQDalaya Warrior Automatic"

bool exit_dll = false;

memory memory;

HANDLE thread_handle_load;
HANDLE thread_handle_keys;
HANDLE thread_handle_warrior_auto;

bool warrior_auto_activated = false;

bool warrior_auto_enabled = true;

int warrior_hot_button = 1;

void toggle_bool(bool &b)
{
    b = !b;
}

bool check_if_sitting()
{
    int player_spawn_info = everquest_get_player_spawn_info(memory);

    BYTE player_standing_state = memory.read_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE);

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

    warrior_auto_enabled = pt.get<bool>("WarriorAuto.bEnabled", warrior_auto_enabled);

    warrior_hot_button = pt.get<int>("WarriorAuto.iWarriorHotButton", warrior_hot_button);

    warrior_hot_button = fix_hot_button_number(warrior_hot_button);
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

        if (memory.get_process_id() == 0)
        {
            exit_dll == true;
            break;
        }

        if (memory.is_foreground_window_current_process_id() == false)
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
            if (warrior_auto_activated == false)
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
            int player_spawn_info = everquest_get_player_spawn_info(memory);

            BYTE player_class = memory.read_any<BYTE>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_CLASS);

            if (player_class != EVERQUEST_CLASS_WARRIOR)
            {
                //everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " Error: You are not a Warrior.");

                exit_dll = true;
                break;
            }

            toggle_bool(warrior_auto_activated);

            if (warrior_auto_activated == true)
            {
                //everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " activated.");
            }
            else
            {
                //everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " deactivated.");
            }

            Sleep(1000);
        }

        Sleep(10);
    }

    if (exit_dll == true)
    {
        //everquest_function_CEverQuest__dsp_chat(APPLICATION_NAME " unloaded.");

        Sleep(1000);

        wait_for_thread(thread_handle_warrior_auto);

        FreeLibraryAndExitThread(module, 0);

        return;
    }
}

void thread_warrior_auto(HMODULE module)
{
    while (1)
    {

        if (exit_dll == true)
        {
            return;
        }

        if (memory.get_process_id() == 0)
        {
            exit_dll == true;
            return;
        }

        if (warrior_auto_enabled == false)
        {
            Sleep(100);
            continue;
        }

        if (warrior_auto_activated == false)
        {
            Sleep(100);
            continue;
        }

        if (check_if_sitting() == true)
        {
            Sleep(100);
            continue;
        }

        bool is_auto_attack_enabled = everquest_is_auto_attack_enabled(memory);

        if (is_auto_attack_enabled == false)
        {
            Sleep(100);
            continue;
        }

        int target_spawn_info = everquest_get_target_spawn_info(memory);

        if (target_spawn_info == EVERQUEST_SPAWN_INFO_NULL)
        {
            Sleep(100);
            continue;
        }

        int player_spawn_info = everquest_get_player_spawn_info(memory);

        float player_y = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        float player_x = memory.read_any<float>(player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);

        float target_y = memory.read_any<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y);
        float target_x = memory.read_any<float>(target_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X);

        float target_distance = everquest_calculate_distance(player_x, player_y, target_x, target_y);

        if (target_distance > 15)
        {
            Sleep(100);
            continue;
        }

        everquest_function_CHotButtonWnd__DoHotButton(warrior_hot_button, 1);

        Sleep(100);
    }
}

void thread_load(HMODULE module)
{
    memory.enable_debug_privileges();
    memory.set_process_by_id(GetCurrentProcessId());

    load_config();

    thread_handle_keys         = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_keys,         module, 0, NULL);
    thread_handle_warrior_auto = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_warrior_auto, module, 0, NULL);

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