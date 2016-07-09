#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <random>

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

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "jansson.h"
#pragma comment(lib, "jansson.lib")

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui.h>
#include "imgui_impl_glfw.h"

#include "eqsod.h"
#include "eqsod_functions.h"

#include "eqapp.h"
#include "eqapp_functions.h"

#include "eqapp_detours.h"
#include "eqapp_console.h"
#include "eqapp_memory.h"
#include "eqapp_sounds.h"
#include "eqapp_waypoint.h"
#include "eqapp_zoneshortnames.h"
#include "eqapp_freecamera.h"
#include "eqapp_alwaysattack.h"
#include "eqapp_hidecorpselooted.h"
#include "eqapp_maxswimmingskill.h"
#include "eqapp_changeheight.h"
#include "eqapp_linetotarget.h"
#include "eqapp_namedspawns.h"
#include "eqapp_census.h"
#include "eqapp_nobeep.h"
#include "eqapp_spawnbeep.h"
#include "eqapp_targetbeep.h"
#include "eqapp_characterfile.h"
#include "eqapp_textoverlaychattext.h"
#include "eqapp_doors.h"
#include "eqapp_maplocations.h"
#include "eqapp_onscreentext.h"
#include "eqapp_autoloot.h"
#include "eqapp_maplabels.h"
#include "eqapp_drawdistance.h"
#include "eqapp_combathotbutton.h"
#include "eqapp_alwayshotbutton.h"
#include "eqapp_spawncastspell.h"
#include "eqapp_spawnlist.h"
#include "eqapp_banklist.h"
#include "eqapp_inventorylist.h"
#include "eqapp_esp.h"                 // needs to be included last
#include "eqapp_esp_functions.h"       // needs to be included last
#include "eqapp_hud.h"                 // needs to be included last
#include "eqapp_interpretcommand.h"    // needs to be included last

#include "eqapp_imgui.h"
#include "eqapp_imgui_functions.h"

#include "eqapp_imgui_consolewindow.h"

void EQAPP_Load()
{
    std::cout << "Loading..." << std::endl;

    EQAPP_Memory_Load();
    EQAPP_Sounds_Load();
    EQAPP_NamedSpawns_Load();
    EQAPP_ESP_Custom_Load();
    EQAPP_TextOverlayChatText_Load();
    EQAPP_NoBeep_Load();
    EQAPP_ZoneShortNames_Load();

    EQAPP_CharacterFile_Write();

    std::cout << "Loaded." << std::endl;

    g_bLoaded = 1;
}

void EQAPP_Unload()
{
    std::cout << "Unloading..." << std::endl;

    g_spawnCastSpellList.clear();

    EQ_ResetViewActor();

    EQAPP_MapLabels_Remove();

    EQAPP_Memory_Unload();

    EQAPP_FreeCamera_Set(false);

    EQ_SetWindowTitle(EQ_STRING_WINDOW_TITLE);

    std::cout << "Unloaded." << std::endl;

    g_bExit = 1;
}

void EQAPP_SetWindowTitles()
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

    DWORD playerSpawn = EQ_GetPlayerSpawn();
    if (playerSpawn == NULL)
    {
        SetWindowTextA(window, EQ_STRING_WINDOW_TITLE);

        if (g_consoleWindowHwnd != NULL)
        {
            SetWindowTextA(g_consoleWindowHwnd, g_consoleWindowTitle);
        }
    }
    else
    {
        char playerName[EQ_SIZE_SPAWN_INFO_NAME] = {0};
        memcpy(playerName, (LPVOID)(playerSpawn + EQ_OFFSET_SPAWN_INFO_NAME), sizeof(playerName));

        std::stringstream ss;
        ss << EQ_STRING_WINDOW_TITLE << ": " << playerName;

        SetWindowTextA(window, ss.str().c_str());

        if (g_consoleWindowHwnd != NULL)
        {
            std::stringstream ss;
            ss << g_applicationName << ": " << playerName;

            SetWindowTextA(g_consoleWindowHwnd, ss.str().c_str());
        }
    }
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
            std::stringstream ss;
            ss << "ExecuteCmd: " << a1 << ", " << a2 << ", " << a3 << " (" << commandName << ")";

            EQAPP_PrintDebugMessage(__FUNCTION__, ss.str());
        }
    }

    return EQAPP_REAL_ExecuteCmd(a1, a2, a3);
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
    // a1 = spawn casting spell structure

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

    DWORD playerSpawn = EQ_GetPlayerSpawn();

    DWORD spawnInfo = EQ_EQPlayerManager->GetSpawnByID(spawnId);

    if (spawnInfo != NULL && spawnInfo != playerSpawn)
    {
        char spawnName[EQ_SIZE_SPAWN_INFO_NAME] = {0};
        memcpy(spawnName, (LPVOID)(spawnInfo + EQ_OFFSET_SPAWN_INFO_NAME), sizeof(spawnName));
        //EQAPP_Log("Spawn Name:", 0);
        //EQAPP_Log(spawnName, 0);

        std::string spellName = EQ_GetSpellNameById(spellId);
        //EQAPP_Log("Spell Name:", 0);
        //EQAPP_Log(spellName, 0);

        if (spawnName != NULL && spellName.size() != 0)
        {
            if (g_debugIsEnabled == true)
            {
                std::stringstream ss;
                ss << spawnName << " begins to cast " << spellName;

                EQAPP_PrintDebugMessage(__FUNCTION__, ss.str());
            }

            std::stringstream ss;
            ss << spawnName << " (" << spellName << ")";

            EQAPP_OnScreenText_Add(ss.str());

            EQAPP_SpawnCastSpell_Add(spawnInfo, spellId, spellCastTime);
        }
    }

    return EQAPP_REAL_CEverQuest__StartCasting(pThis, a1);
}

int __fastcall EQAPP_DETOUR_CDisplay__CreatePlayerActor(void* pThis, void* not_used, int a1, int a2, int a3, int a4, int a5)
{
    // a1 = spawnInfo
    // a2 = 0
    // a3 = 1
    // a4 = 2
    // a5 = 1

    if (g_bExit == 1)
    {
        return EQAPP_REAL_CDisplay__CreatePlayerActor(pThis, a1, a2, a3, a4, a5);
    }

    if (a1 != NULL)
    {
        if (g_replaceRacesIsEnabled == true)
        {
            DWORD playerSpawn = EQ_GetPlayerSpawn();
            if (a1 == playerSpawn)
            {
                int spawnRace = EQ_ReadMemory<DWORD>(playerSpawn + EQ_OFFSET_SPAWN_INFO_RACE);
                if (spawnRace == EQ_RACE_CHOKADAI)
                {
                    EQ_WriteMemory<DWORD>(playerSpawn + EQ_OFFSET_SPAWN_INFO_RACE, EQ_RACE_HUMAN);
                }
            }
        }

        char spawnNumberedName[EQ_SIZE_SPAWN_INFO_NUMBERED_NAME] = {0};
        memcpy(spawnNumberedName, (LPVOID)(a1 + EQ_OFFSET_SPAWN_INFO_NUMBERED_NAME), sizeof(spawnNumberedName));

        if (g_debugIsEnabled == true && spawnNumberedName != NULL)
        {
            std::cout << "[debug] CDisplay::CreatePlayerActor(): " << spawnNumberedName << std::endl;
            std::cout << "[debug] a1: " << a1 << std::endl; // spawnInfo
            std::cout << "[debug] a2: " << a2 << std::endl; // 0
            std::cout << "[debug] a3: " << a3 << std::endl; // 1
            std::cout << "[debug] a4: " << a4 << std::endl; // 2
            std::cout << "[debug] a5: " << a5 << std::endl; // 1
        }

        if (EQ_DoesSpawnExist(a1) == false)
        {
            EQAPP_OnScreenText_AddSpawnMessage(a1, false);
        }

        if (EQ_IsInGame() == true)
        {
            EQAPP_SpawnBeep_Execute(a1);
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
        DWORD spawnInfo = EQ_ReadMemory<DWORD>(a1 + EQ_OFFSET_ACTOR_INSTANCE_INFO_SPAWN_INFO);
        if (spawnInfo != NULL)
        {
            char spawnNumberedName[EQ_SIZE_SPAWN_INFO_NUMBERED_NAME] = {0};
            memcpy(spawnNumberedName, (LPVOID)(spawnInfo + EQ_OFFSET_SPAWN_INFO_NUMBERED_NAME), sizeof(spawnNumberedName));

            if (g_debugIsEnabled == true && spawnNumberedName != NULL)
            {
                std::cout << "[debug] CDisplay::DeleteActor(): " << spawnNumberedName << std::endl;
            }

            EQAPP_OnScreenText_AddSpawnMessage(spawnInfo, true);
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
                EQAPP_PlaySound(sound.filename.c_str());
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
                EQAPP_TextOverlayChatText_DisplayText(a1);
                EQAPP_OnScreenText_Add(a1);
                break;
            }
        }
    }

    if (g_censusIsEnabled == true)
    {
        if (strstr(a1, "[") != NULL && strstr(a1, "]") != NULL)
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
        return EQAPP_REAL_CEverQuest__MoveToZone(pThis, a1, a2, a3, a4, a5, a6, a7, a8);
    }

    EQAPP_MapLabels_Remove();

    return EQAPP_REAL_CEverQuest__MoveToZone(pThis, a1, a2, a3, a4, a5, a6, a7, a8);
}

int __fastcall EQAPP_DETOUR_CEverQuest__EnterZone(void* pThis, void* not_used, struct HWND__* a1)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_CEverQuest__EnterZone(pThis, a1);
    }

    int result = EQAPP_REAL_CEverQuest__EnterZone(pThis, a1);

    EQAPP_FreeCamera_Set(false);

    EQAPP_CharacterFile_Write();

    EQAPP_TextOverlayChatText_Load();
    EQAPP_NoBeep_Load();
    EQAPP_NamedSpawns_Load();
    EQAPP_ESP_Custom_Load();

    g_alwaysAttackIsEnabled = false;

    g_censusIsEnabled = false;

    g_spawnCastSpellList.clear();

    g_onScreenTextList.clear();

    return result;
}

/*
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
        EQAPP_Unload();
    }

    if (strlen(a2) > 2 && strncmp(a2, "//", 2) == 0)
    {
        EQAPP_InterpretCommand(a2);

        return EQAPP_REAL_CEverQuest__InterpretCmd(pThis, NULL, NULL);
    }

    return EQAPP_REAL_CEverQuest__InterpretCmd(pThis, a1, a2);
}
*/

int __fastcall EQAPP_DETOUR_EQ_Character__eqspa_movement_rate(void* pThis, void* not_used, int a1)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_EQ_Character__eqspa_movement_rate(pThis, a1);
    }

    int result = EQAPP_REAL_EQ_Character__eqspa_movement_rate(pThis, a1);

    if (g_speedHackIsEnabled == true)
    {
        DWORD playerSpawn = EQ_GetPlayerSpawn();
        if (playerSpawn != NULL)
        {
            FLOAT speedModifier = EQ_ReadMemory<FLOAT>(playerSpawn + EQ_OFFSET_SPAWN_INFO_SPEED_MODIFIER);
            if (speedModifier < g_speedHackModifier)
            {
                EQ_WriteMemory<FLOAT>(playerSpawn + EQ_OFFSET_SPAWN_INFO_SPEED_MODIFIER, g_speedHackModifier);
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

    DWORD playerSpawn = EQ_GetPlayerSpawn();
    if (playerSpawn != NULL && (DWORD)pThis == playerSpawn)
    {
        if (a1 == EQ_STANDING_STATE_DUCKING)
        {
            if (g_freeCameraIsEnabled == true)
            {
                EQAPP_FreeCamera_Set(false);
            }

            g_censusIsEnabled = false;
        }
    }

    return EQAPP_REAL_EQPlayer__ChangePosition(pThis, a1);
}

/*
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
*/

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

        DWORD playerSpawn = EQ_GetPlayerSpawn();
        if (playerSpawn != NULL && (DWORD)pThis == playerSpawn)
        {
            if (a1 == EQ_RACE_CHOKADAI)
            {
                a1 = EQ_RACE_HUMAN;
            }
        }
    }

    return EQAPP_REAL_EQPlayer__SetRace(pThis, a1);
}

/*
int __fastcall EQAPP_DETOUR_CMapViewWnd__dCMapViewWnd(void* pThis, void* not_used)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_CMapViewWnd__dCMapViewWnd(pThis);
    }

    //std::cout << "CMapViewWnd::~CMapViewWnd()" << std::endl;

    return EQAPP_REAL_CMapViewWnd__dCMapViewWnd(pThis);
}
*/

int __fastcall EQAPP_DETOUR_CMapViewWnd__DrawMap(void* pThis, void* not_used, int a1, int a2, int a3, int a4)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_CMapViewWnd__DrawMap(pThis, a1, a2, a3, a4);
    }

    if (g_mapLabelsIsEnabled == true)
    {
        EQAPP_MapLabels_Execute();
    }

    return EQAPP_REAL_CMapViewWnd__DrawMap(pThis, a1, a2, a3, a4);
}

int __fastcall EQAPP_DETOUR_MapViewMap__Save(void* pThis, void* not_used, char* a1)
{
    // a1 = short zone name of map

    if (g_bExit == 1)
    {
        return EQAPP_REAL_MapViewMap__Save(pThis, a1);
    }

    std::cout << "The game is saving the map..." << std::endl;

    EQAPP_MapLabels_Remove();

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

    std::cout << "The game is saving map layer #" << a2 << std::endl;

    EQAPP_MapLabels_Remove();

    return EQAPP_REAL_MapViewMap__SaveEx(pThis, a1, a2);
}

int __cdecl EQAPP_DETOUR_DrawNetStatus(int a1, unsigned short a2, unsigned short a3, unsigned short a4, unsigned short a5, int a6, unsigned short a7, unsigned long a8, long a9, unsigned long a10)
{
    if (g_bExit == 1)
    {
        return EQAPP_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }

    if (g_bLoaded == 0)
    {
        EQAPP_Load();
    }

    // kill switch
    if (GetAsyncKeyState(g_killSwitchKey))
    {
        EQAPP_Unload();
    }

    if (EQ_IsInGame() == false)
    {
        return EQAPP_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }

    if (EQ_IsScreenshotBeingTaken() == true)
    {
        return EQAPP_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    }

    EQAPP_SetWindowTitles();

    EQAPP_FreeCamera_Keys();

    EQAPP_HideCorpseLooted_Execute();
    EQAPP_AlwaysAttack_Execute();
    EQAPP_MaxSwimmingSkill_Execute();
    EQAPP_AutoLoot_Execute();
    EQAPP_SpawnCastSpell_Execute();
    EQAPP_TargetBeep_Execute();
    EQAPP_CombatHotbutton_Execute();
    EQAPP_AlwaysHotbutton_Execute();
    EQAPP_ChangeHeight_Execute();
    EQAPP_DrawDistance_Execute();
    EQAPP_Census_Execute();

    if (EQAPP_IsAnImportantWindowOpen() == false && EQ_IsKeyShiftPressed() == false)
    {
        // these functions draw on the screen
        EQAPP_LineToTarget_Execute();
        EQAPP_ESP_Execute();
        EQAPP_NamedSpawns_Execute();
        EQAPP_OnScreenText_Execute();
    }

    EQAPP_Hud_Execute();

    return EQAPP_REAL_DrawNetStatus(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

int __cdecl EQAPP_DETOUR_Exit()
{
    EQAPP_Unload();

    return EQAPP_REAL_Exit();
}

DWORD WINAPI EQAPP_ThreadConsole(LPVOID param)
{
    if (!glfwInit())
    {
        return 0;
    }

    g_consoleWindow = glfwCreateWindow(g_consoleWindowWidth, g_consoleWindowHeight, g_consoleWindowTitle, NULL, NULL);
    if (!g_consoleWindow)
    {
        glfwTerminate();
        ImGui_ImplGlfw_Shutdown();

        g_bConsole = 0;

        return 0;
    }

    glfwMakeContextCurrent(g_consoleWindow);
    ImGui_ImplGlfw_Init(g_consoleWindow, true);

    g_consoleWindowHwnd = glfwGetWin32Window(g_consoleWindow);

    EQAPP_CenterWindow(g_consoleWindowHwnd);

    // disable close button
    EnableMenuItem(GetSystemMenu(g_consoleWindowHwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

    const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    g_videoModeWidth  = videoMode->width;
    g_videoModeHeight = videoMode->height;

    EQAPP_Console_Load();

    while (g_bExit == 0)
    {
        if (!glfwWindowShouldClose(g_consoleWindow))
        {
            glfwPollEvents();
            ImGui_ImplGlfw_NewFrame();

            EQAPP_IMGUI_Draw();

            int displayWidth, displayHeight;
            glfwGetFramebufferSize(g_consoleWindow, &displayWidth, &displayHeight);
            glViewport(0, 0, displayWidth, displayHeight);
            glClearColor(g_consoleWindowClearColor.x, g_consoleWindowClearColor.y, g_consoleWindowClearColor.z, g_consoleWindowClearColor.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui::Render();
            glfwSwapBuffers(g_consoleWindow);
        }

        // print redirected std::cout to the console
        for (std::string text; std::getline(g_consoleStringStream, text, '\n');)
        {
            g_imguiConsoleWindow.AddLog("%s", text.c_str());
        }
        g_consoleStringStream.str(std::string());
        g_consoleStringStream.clear();

        // interpet commands sent to the console
        for (auto& command : g_imguiConsoleWindow.m_interpretCommands)
        {
            EQAPP_InterpretCommand(command.c_str());
        }
        g_imguiConsoleWindow.m_interpretCommands.clear();
    }

    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();

    EQAPP_Console_Unload();

    return 0;
}

DWORD WINAPI EQAPP_ThreadLoop(LPVOID param)
{
    while (g_bExit == 0)
    {
        Sleep(100);
    }

    // wait for the console to unload
    while (g_bConsole == 1);

    Sleep(100);

    EQAPP_Detours_Remove();

    Sleep(1000);

    FreeLibraryAndExitThread(g_module, 0);
    return 0;
}

DWORD WINAPI EQAPP_ThreadLoad(LPVOID param)
{
    EQAPP_EnableDebugPrivileges();

    EQ_LoadGraphicsDllFunctions();

    g_handleThreadConsole = CreateThread(NULL, 0, &EQAPP_ThreadConsole, NULL, 0, NULL);

    // wait for the console to load
    while (g_bConsole == 0);

    g_handleThreadLoop = CreateThread(NULL, 0, &EQAPP_ThreadLoop, NULL, 0, NULL);

    EQAPP_Detours_Add();

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
