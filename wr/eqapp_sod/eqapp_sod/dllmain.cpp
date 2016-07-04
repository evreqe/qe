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
#include "eqapp_memory.h"
#include "eqapp_sounds.h"
#include "eqapp_waypoint.h"
#include "eqapp_freecamera.h"
#include "eqapp_linetotarget.h"
#include "eqapp_namedspawns.h"
#include "eqapp_census.h"
#include "eqapp_nobeep.h"
#include "eqapp_characterfile.h"
#include "eqapp_textoverlaychattext.h"
#include "eqapp_doors.h"

#include "eqapp_imgui.h"
#include "eqapp_imgui_functions.h"

#include "eqapp_imgui_consolewindow.h"

void EQAPP_Load()
{
    std::cout << "Loading..." << std::endl;

    EQAPP_Memory_Load();
    EQAPP_Sounds_Load();
    EQAPP_NamedSpawns_Load();
    EQAPP_LoadEspCustom();
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

    EQAPP_RemoveMapLabels();

    EQAPP_Memory_Unload();

    EQAPP_FreeCamera_Set(false);

    HWND window = EQ_ReadMemory<HWND>(EQ_WINDOW_HWND);
    if (window != NULL)
    {
        SetWindowTextA(window, EQ_STRING_WINDOW_TITLE);
    }

    std::cout << "Unloaded." << std::endl;

    g_bExit = 1;
}

void EQAPP_Console_Load()
{
    g_consoleStreamBuffer = std::cout.rdbuf(g_consoleStringStream.rdbuf());

    g_bConsole = 1;
}

void EQAPP_Console_Unload()
{
    std::cout.rdbuf(g_consoleStreamBuffer);

    g_bConsole = 0;
}

void EQAPP_WindowTitles_Execute()
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

        if (g_hwndConsoleWindow != NULL)
        {
            SetWindowTextA(g_hwndConsoleWindow, g_consoleWindowTitle);
        }
    }
    else
    {
        char playerName[0x40] = {0};
        memcpy(playerName, (LPVOID)(playerSpawn + 0xE4), sizeof(playerName));

        std::stringstream ss;
        ss << EQ_STRING_WINDOW_TITLE << ": " << playerName;

        SetWindowTextA(window, ss.str().c_str());

        if (g_hwndConsoleWindow != NULL)
        {
            std::stringstream ss;
            ss << g_applicationName << ": " << playerName;

            SetWindowTextA(g_hwndConsoleWindow, ss.str().c_str());
        }
    }
}

void EQAPP_LoadEspCustom()
{
    std::cout << "Loading custom ESP..." << std::endl;

    g_espCustomList.clear();

    std::string zoneShortName = EQ_GetZoneShortName();
    if (zoneShortName.size() == 0)
    {
        std::cout << __FUNCTION__ << ": zone short name is null" << std::endl;
        return;
    }

    std::stringstream filePath;
    filePath << "eqapp/esp/" << zoneShortName << ".txt";

    std::ifstream file;
    file.open(filePath.str().c_str(), std::ios::in);
    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": failed to open file: " << filePath.str() << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        float y;
        float x;
        float z;
        unsigned int red;
        unsigned int green;
        unsigned int blue;
        unsigned int size;
        char text[1024];

        int result = sscanf_s(line.c_str(), "%f %f %f %d %d %d %d %s", &y, &x, &z, &red, &green, &blue, &size, text, sizeof(text));

        if (result == 8)
        {
            EQ_StringReplaceUnderscoresWithSpaces(text);

            EQAPPESPCUSTOM espCustom;
            espCustom.y        = y;
            espCustom.x        = x;
            espCustom.z        = z;
            espCustom.color    = (255 << 24) + (red << 16) + (green << 8) + blue;
            espCustom.size     = size;
            espCustom.text     = text;

            g_espCustomList.push_back(espCustom);
        }
    }

    file.close();
}

void EQAPP_MapLocations_WriteToFile()
{
    std::cout << "Saving map locations to file..." << std::endl;

    std::string zoneShortName = EQ_GetZoneShortName();
    if (zoneShortName.size() == 0)
    {
        std::cout << __FUNCTION__ << ": zone short name is null" << std::endl;
        return;
    }

    std::stringstream filePath;
    filePath << "eqapp/maplocations/" << zoneShortName << ".txt";

    std::fstream file;
    file.open(filePath.str().c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": failed to open file: " << filePath.str() << std::endl;
        return;
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

        std::string mapLocation = EQ_GetSpawnMapLocationString(spawn);

        file << mapLocation << std::endl;

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    file.close();
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

    if (EQ_HasTimePassed(g_onScreenTextTimer, g_onScreenTextTimerDelay) == true)
    {
        g_onScreenTextList.pop_front();
    }

    std::stringstream ssDrawText;

    size_t textCount = 0;

    for (auto g_onScreenTextList_it = g_onScreenTextList.rbegin(); g_onScreenTextList_it != g_onScreenTextList.rend(); g_onScreenTextList_it++)
    {
        std::string text = *g_onScreenTextList_it;

        if (textCount >= g_onScreenTextCountMax)
        {
            break;
        }

        ssDrawText << text << "\n";

        textCount++;
    }

    EQ_DrawText(ssDrawText.str().c_str(), g_onScreenTextX, g_onScreenTextY, g_onScreenTextColor, 2);
}

void EQAPP_ZoneShortNames_Load()
{
    std::cout << "Loading short zone names..." << std::endl;

    g_zoneShortNamesList.clear();

    std::ifstream file;
    file.open("eqapp/zoneshortnames.txt", std::ios::in);
    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": failed to open file: eqapp/zoneshortnames.txt" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        g_zoneShortNamesList.push_back(line);
    }

    g_zoneShortNamesList_it = g_zoneShortNamesList.begin();

    file.close();
}

void EQAPP_DoHud()
{
    if (g_hudIsEnabled == false)
    {
        return;
    }

    float width  = g_hudWidth;
    float height = g_hudHeight;

    EQ_DrawQuad((float)g_hudX, (float)g_hudY, width, height, EQ_TOOLTIP_COLOR);

    unsigned int fontSize   = 2;
    unsigned int fontHeight = EQ_GetFontHeight(fontSize);

    unsigned int numDrawText = 0;

    std::stringstream ssDrawText;

    ssDrawText << g_applicationName << " - " << std::dec << (int)EQ_GetAverageFps() << " FPS\n";
    numDrawText++;

    DWORD numPlayersInZone = EQ_GetNumPlayersInZone();
    if (numPlayersInZone != 0)
    {
        ssDrawText << "Players in Zone: " << numPlayersInZone << "\n";
        numDrawText++;
    }

    if (g_speedHackIsEnabled == true)
    {
        ssDrawText << "Speed: " << g_speedHackModifier << "\n";
        numDrawText++;
    }

    if (g_freeCameraIsEnabled == true)
    {
        ssDrawText << "Free Camera is on.\n";
        numDrawText++;
    }

    if (EQ_IsAutoAttackEnabled() == true)
    {
        ssDrawText << "Auto attack is on.\n";
        numDrawText++;
    }

    if (EQ_IsAutoFireEnabled() == true)
    {
        ssDrawText << "Auto fire is on.\n";
        numDrawText++;
    }

    if (g_alwaysAttackIsEnabled == true)
    {
        ssDrawText << "Always attack is on.\n";
        numDrawText++;
    }

    if (g_alwaysHotbuttonIsEnabled == true)
    {
        ssDrawText << "Always Hotbutton: " << (g_alwaysHotbuttonIndex + 1) << " (" << (g_alwaysHotbuttonTimerDelay / 1000) << "s)\n";
        numDrawText++;
    }

    if (g_combatHotbuttonIsEnabled == true)
    {
        ssDrawText << "Combat Hotbutton: " << (g_combatHotbuttonIndex + 1) << " (" << (g_combatHotbuttonTimerDelay / 1000) << "s)\n";
        numDrawText++;
    }

    if (g_targetBeepIsEnabled == true)
    {
        ssDrawText << "Target Beep: " << g_targetBeepName << " (" << (g_targetBeepTimerDelay / 1000) << "s)\n";
        numDrawText++;
    }

    if (g_spawnBeepIsEnabled == true && g_spawnBeepName.size() != 0)
    {
        ssDrawText << "Spawn Beep: " << g_spawnBeepName << "\n";
        numDrawText++;
    }

    if (g_locatorIsEnabled == true)
    {
        ssDrawText << "Locator: " << g_locatorY << ", " << g_locatorX << ", " << g_locatorZ << "\n";
        numDrawText++;
    }

    if (g_findIsEnabled == true && g_findSpawnName.size() != 0)
    {
        ssDrawText << "Find: " << g_findSpawnName << "\n";
        numDrawText++;
    }

    if (g_autoLootIsEnabled == true)
    {
        ssDrawText << "Auto Loot is on.\n";
        numDrawText++;

        for (auto& itemName : g_autoLootList)
        {
            ssDrawText << "AL: " << itemName << "\n";
            numDrawText++;
        }
    }

    if (g_debugIsEnabled == true)
    {
        ssDrawText << "Debug is on.\n";
        numDrawText++;

        ssDrawText << "ESP Num Draw Text: " << g_espNumDrawText << "\n";
        numDrawText++;
    }

    EQ_DrawText(ssDrawText.str().c_str(), g_hudX, g_hudY, 0xFFFFFFFF, 2);

    if (numDrawText > 0)
    {
        g_hudHeight = (float)((numDrawText * fontHeight) + 2.0f);
    }
}

void EQAPP_DoTargetBeep()
{
    if (g_targetBeepIsEnabled == false)
    {
        return;
    }

    if (EQ_HasTimePassed(g_targetBeepTimer, g_targetBeepTimerDelay) == false)
    {
        return;
    }

    DWORD targetSpawn = EQ_GetTargetSpawn();
    if (targetSpawn == NULL)
    {
        return;
    }

    char spawnName[0x40] = {0};
    memcpy(spawnName, (LPVOID)(targetSpawn + 0xE4), sizeof(spawnName));

    if (strlen(spawnName) < 2)
    {
        return;
    }

    if (g_targetBeepName.size() != 0 && strstr(spawnName, g_targetBeepName.c_str()) != NULL)
    {
        MessageBeep(0);
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

    if (strlen(spawnName) < 2)
    {
        return;
    }

    for (auto& noBeep : g_noBeepList)
    {
        if (noBeep.size() != 0 && strcmp(spawnName, noBeep.c_str()) == 0)
        {
            return;
        }
    }

    for (auto& namedSpawn : g_namedSpawnsList)
    {
        if (namedSpawn.size() != 0 && strstr(spawnName, namedSpawn.c_str()) != NULL)
        {
            MessageBeep(0);
            break;
        }
    }

    if (g_spawnBeepIsEnabled == true)
    {
        if (g_spawnBeepName.size() != 0 && strstr(spawnName, g_spawnBeepName.c_str()) != NULL)
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

    DWORD playerSpawn = EQ_GetPlayerSpawn();
    if (playerSpawn == NULL)
    {
        return;
    }

    DWORD standingState = EQ_ReadMemory<BYTE>(playerSpawn + 0x279);
    if (standingState != EQ_STANDING_STATE_STANDING)
    {
        return;
    }

    DWORD targetSpawn = EQ_GetTargetSpawn();
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

    DWORD targetSpawn = EQ_GetTargetSpawn();
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

    if (EQ_HasTimePassed(g_combatHotbuttonTimer, g_combatHotbuttonTimerDelay) == false)
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

    if (EQ_HasTimePassed(g_alwaysHotbuttonTimer, g_alwaysHotbuttonTimerDelay) == false)
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

    DWORD playerSpawn = EQ_GetPlayerSpawn();
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

    if (EQ_HasTimePassed(g_drawDistanceTimer, g_drawDistanceTimerDelay) == false)
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

    if (EQ_HasTimePassed(g_mapLabelsTimer, g_mapLabelsTimerDelay) == false)
    {
        return;
    }

    EQAPP_RemoveMapLabels();

    DWORD playerSpawn = EQ_GetPlayerSpawn();

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

        EQXYZ location;
        location.X = -spawnX;
        location.Y = -spawnY;
        location.Z = spawnZ;

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

        EQMAPLABEL mapLabel;
        mapLabel.Location    = location;
        mapLabel.Color       = color;
        mapLabel.Size        = 2;
        mapLabel.Label       = (PCHAR)(spawn + 0xE4); // spawn name
        mapLabel.Layer       = 3;
        mapLabel.Width       = 20;
        mapLabel.Height      = 12;
        mapLabel.Data        = g_mapLabelsData;

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

        // drawing a line instead of text for performance
        EQ_DrawLine((float)screenX, (float)screenY, 1.0f, (float)screenX, (float)(screenY + 1.0f), 1.0f, argbColor);
    }
}

void EQAPP_UpdateEspSpawnList()
{
    if (EQ_HasTimePassed(g_espSpawnListTimer, g_espSpawnListTimerDelay) == false)
    {
        return;
    }

    g_espSpawnList.clear();

    unsigned int numNpcCorpse = 0;

    DWORD targetSpawn = EQ_GetTargetSpawn();
    DWORD playerSpawn = EQ_GetPlayerSpawn();

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

        espSpawn.standingState = EQ_ReadMemory<BYTE>(spawn + 0x279);

        espSpawn.isHoldingPrimary      = EQ_ReadMemory<BYTE>(spawn + 0x10E0);
        espSpawn.isHoldingSecondary    = EQ_ReadMemory<BYTE>(spawn + 0x10E4);

        espSpawn.isPet    = EQ_ReadMemory<DWORD>(spawn + 0x260);
        espSpawn.isLfg    = EQ_ReadMemory<BYTE>(spawn + 0x1F1);
        espSpawn.isPvp    = EQ_ReadMemory<BYTE>(spawn + 0x2BC);
        espSpawn.isTrader = EQ_ReadMemory<DWORD>(spawn + 0x228);
        espSpawn.isAfk    = EQ_ReadMemory<DWORD>(spawn + 0x284);
        espSpawn.isGm     = EQ_ReadMemory<BYTE>(spawn + 0x4DA);

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
            espSpawn.isHalfDistance = true;
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

    DWORD playerSpawn = EQ_GetPlayerSpawn();

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

            if (spawn.isHalfDistance == true)
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

            DWORD textColor = 0xFFFFFFFF; // white

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
                    textColor = 0xFFC0C0C0;
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

            if (spawn.isHalfDistance == true)
            {
                EQ_HexColorDarken(textColor, 0.9f);
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

            std::stringstream ssDrawText;
            ssDrawText << "+ " << spawn.name;

            if (spawn.type == EQ_SPAWN_TYPE_PLAYER_CORPSE || spawn.type == EQ_SPAWN_TYPE_NPC_CORPSE)
            {
                ssDrawText << " corpse";
            }

            ssDrawText << " L" << spawn.level;

            if (spawn._class > EQ_CLASS_UNKNOWN && spawn._class < EQ_CLASS_BANKER)
            {
                if (spawn.type == EQ_SPAWN_TYPE_PLAYER || (spawn.type == EQ_SPAWN_TYPE_NPC && EQ_IsKeyControlPressed() == true))
                {
                    const char* spawnClassThreeLetterCode = EQ_CEverQuest->GetClassThreeLetterCode(spawn._class);

                    ssDrawText << " " << spawnClassThreeLetterCode;
                }
            }

            ssDrawText << " (" << (int)spawn.distance << ")";

            if (spawn.isLfg == 1)
            {
                ssDrawText << "  LFG";
            }

            if (spawn.isPvp == 1)
            {
                ssDrawText << "  PVP";
            }

            if (spawn.isTrader == 1)
            {
                ssDrawText << "  TRADER";
            }

            if (spawn.isAfk == 1)
            {
                ssDrawText << "  AFK";
            }

            if (spawn.isGm == 1)
            {
                ssDrawText << "  *GM*";
            }

            ssDrawText << "\n";
            g_espNumDrawText++;

            if (spawn.type == EQ_SPAWN_TYPE_PLAYER)
            {
                if (spawn.standingState == EQ_STANDING_STATE_FEIGNED)
                {
                    ssDrawText << "*Feign Death*\n";
                    g_espNumDrawText++;
                }

                if (EQ_IsKeyControlPressed() == true)
                {
                    if (spawn.guildId > 0 && spawn.guildId < EQ_NUM_GUILDS)
                    {
                        const char* spawnGuildName = EQ_EQ_Guilds.GetGuildNameById(spawn.guildId);

                        ssDrawText << "<" << spawnGuildName << ">\n";
                        g_espNumDrawText++;
                    }
                }
            }
            else if (spawn.type == EQ_SPAWN_TYPE_NPC)
            {
                if (strlen(spawn.lastName) > 0)
                {
                    ssDrawText << "(" << spawn.lastName << ")\n";
                    g_espNumDrawText++;
                }

                if (EQ_IsKeyControlPressed() == true)
                {
                    const char* spawnBodyTypeDescription = EQ_CEverQuest->GetBodyTypeDesc(spawn.spawnInfo + 0x128);

                    ssDrawText << "[" << spawnBodyTypeDescription << "]\n";
                    g_espNumDrawText++;

                    if (spawn.isHoldingPrimary != 0 && spawn.isHoldingSecondary != 0)
                    {
                        ssDrawText << "Holding Primary & Secondary\n";
                        g_espNumDrawText++;
                    }
                    else if (spawn.isHoldingPrimary != 0)
                    {
                        ssDrawText << "Holding Primary\n";
                        g_espNumDrawText++;
                    }
                    else if (spawn.isHoldingSecondary != 0)
                    {
                        ssDrawText << "Holding Secondary\n";
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
                        ssDrawText << "(" << spawnCastSpell->spellName << ")\n";
                        g_espNumDrawText++;
                        break;
                    }
                }
            }

            EQ_DrawText(ssDrawText.str().c_str(), screenX, screenY, textColor, fontSize);

            if (g_findIsEnabled == true && spawn.isFindSpawn == true)
            {
                DWORD windowWidth  = EQ_ReadMemory<DWORD>(EQ_WINDOW_WIDTH);
                DWORD windowHeight = EQ_ReadMemory<DWORD>(EQ_WINDOW_HEIGHT);

                EQ_DrawLine((float)(windowWidth / 2), (float)windowHeight, 0.0f, (float)screenX, (float)screenY, 0.0f, g_espFindColor);
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

                EQ_DrawText(ss.str().c_str(), screenX, screenY, g_espGroundSpawnColor, 2);
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

                    EQ_DrawText(ss.str().c_str(), screenX, screenY, g_espDoorColor, 2);
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
                unsigned int fontSize   = 2;
                unsigned int fontHeight = EQ_GetFontHeight(fontSize);

                DWORD zoneObject = EQ_ReadMemory<DWORD>(pointer2 + 0x5C);

                while (zoneObject)
                {
                    DWORD zoneObject0x0C = EQ_ReadMemory<DWORD>(zoneObject + 0x0C);

                    if (zoneObject0x0C == 2)
                    {
                        zoneObject = EQ_ReadMemory<DWORD>(zoneObject + 0x04); // next
                        continue;
                    }

                    //DWORD zoneObject0x2C = EQ_ReadMemory<BYTE>(zoneObject + 0x2C);
                    //DWORD zoneObject0x2D = EQ_ReadMemory<BYTE>(zoneObject + 0x2D);

                    // skip player and npc models
                    //if (EQ_IsKeyControlPressed() == false && zoneObject0x2C == 1 && zoneObject0x2D == 0)
                    //{
                        //zoneObject = EQ_ReadMemory<DWORD>(zoneObject + 0x04); // next
                        //continue;
                    //}

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

                    std::stringstream ssDrawText;
                    ssDrawText << "+ " << zoneObjectName << " (" << (int)zoneObjectDistance << ")\n";

                    ssDrawText << "Y: " << zoneObjectY << "\n";
                    ssDrawText << "X: " << zoneObjectX << "\n";
                    ssDrawText << "Z: " << zoneObjectZ << "\n";

                    EQ_DrawText(ssDrawText.str().c_str(), screenX, screenY, g_espZoneObjectColor, fontSize);

                    zoneObject = EQ_ReadMemory<DWORD>(zoneObject + 0x04); // next
                }
            }
        }
    }

    if (g_espCustomIsEnabled == true)
    {
        for (auto& espCustom : g_espCustomList)
        {
            float distance = EQ_CalculateDistance3d(playerX, playerY, playerZ, espCustom.x, espCustom.y, espCustom.z);
            if (distance > g_espCustomDistance)
            {
                continue;
            }

            int screenX = -1;
            int screenY = -1;
            bool result = EQ_WorldSpaceToScreenSpace(espCustom.x, espCustom.y, espCustom.z, screenX, screenY);
            if (result == false)
            {
                continue;
            }

            std::stringstream ss;
            ss << "# " << espCustom.text << " (" << (int)distance << ")";
            EQ_DrawText(ss.str().c_str(), screenX, screenY, espCustom.color, espCustom.size);
        }
    }

    if (g_espWaypointIsEnabled == true)
    {
        for (auto& waypoint : g_waypointList)
        {
            float distance = EQ_CalculateDistance3d(playerX, playerY, playerZ, waypoint.x, waypoint.y, waypoint.z);
            if (distance > g_espWaypointDistance)
            {
                continue;
            }

            int screenX = -1;
            int screenY = -1;
            bool result = EQ_WorldSpaceToScreenSpace(waypoint.x, waypoint.y, waypoint.z, screenX, screenY);
            if (result == false)
            {
                continue;
            }

            for (auto& connectIndex : waypoint.connectList)
            {
                if (connectIndex > g_waypointList.size())
                {
                    continue;
                }

                PEQAPPWAYPOINT connectWaypoint = EQAPP_Waypoint_GetByIndex(connectIndex);
                if (connectWaypoint == NULL)
                {
                    std::cout << __FUNCTION__ << ": connect waypoint is null" << std::endl;
                    continue;
                }

                float distance2 = EQ_CalculateDistance3d(playerX, playerY, playerZ, connectWaypoint->x, connectWaypoint->y, connectWaypoint->z);
                if (distance2 > g_espWaypointDistance)
                {
                    continue;
                }

                int screenX2 = -1;
                int screenY2 = -1;
                bool result2 = EQ_WorldSpaceToScreenSpace(connectWaypoint->x, connectWaypoint->y, connectWaypoint->z, screenX2, screenY2);
                if (result2 == false)
                {
                    continue;
                }

                EQ_DrawLine((float)screenX, (float)screenY, 0.0f, (float)screenX2, (float)screenY2, 0.0f, g_espWaypointColor);
            }

            std::stringstream ss;
            ss << "@ " << waypoint.name << " (" << (int)distance << ")";
            EQ_DrawText(ss.str().c_str(), screenX, screenY, g_espWaypointColor, 2);
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

            EQ_DrawText(ss.str().c_str(), screenX, screenY, g_espLocatorColor, 5);
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
        if (delay < g_spawnCastSpellTimerDelay)
        {
            delay = g_spawnCastSpellTimerDelay;
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
        std::cout << __FUNCTION__ << ": not in-game" << std::endl;
        return;
    }

    EQAPP_COUT_SaveFlags();

    std::cout << "Spawn List:";
    if (filterSpawnName != NULL)
    {
        std::cout << " " << filterSpawnName;
    }
    std::cout << std::endl;

    DWORD playerSpawn = EQ_GetPlayerSpawn();

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

    EQAPP_COUT_RestoreFlags();
}

void EQAPP_AutoLoot_Execute()
{
    if (g_autoLootIsEnabled == false || g_autoLootList.size() == 0)
    {
        return;
    }

    if (EQ_IsWindowVisible(EQ_POINTER_CLootWnd) == false)
    {
        g_autoLootTimer = EQ_GetTimer();
        return;
    }

    if (EQ_HasTimePassed(g_autoLootTimer, g_autoLootTimerDelay) == false)
    {
        return;
    }

    for (auto& itemName : g_autoLootList)
    {
        bool result = EQ_LootItemByName(itemName.c_str());
    }
}

void EQAPP_AutoLoot_Toggle()
{
    EQ_ToggleBool(g_autoLootIsEnabled);

    if (g_autoLootIsEnabled == false)
    {
        g_autoLootList.clear();
    }

    EQAPP_PrintBool("Auto Loot", g_autoLootIsEnabled);
}

void EQAPP_AutoLoot_Reset()
{
    g_autoLootList.clear();

    std::cout << "Auto Loot reset." << std::endl;
}

void EQAPP_DoBankList(const char* filterItemName)
{
    if (EQ_IsInGame() == false)
    {
        std::cout << __FUNCTION__ << ": not in-game" << std::endl;
        return;
    }

    DWORD charInfo = EQ_ReadMemory<DWORD>(EQ_POINTER_CHAR_INFO);
    if (charInfo == NULL)
    {
        std::cout << __FUNCTION__ << ": char info is null" << std::endl;
        return;
    }

    EQAPP_COUT_SaveFlags();

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

    EQAPP_COUT_RestoreFlags();
}

void EQAPP_DoInventoryList(const char* filterItemName)
{
    if (EQ_IsInGame() == false)
    {
        std::cout << __FUNCTION__ << ": not in-game" << std::endl;
        return;
    }

    DWORD charInfo2 = EQ_GetCharInfo2();
    if (charInfo2 == NULL)
    {
        std::cout << __FUNCTION__ << ": char info 2 is null" << std::endl;
        return;
    }

    EQAPP_COUT_SaveFlags();

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

    EQAPP_COUT_RestoreFlags();
}

void EQAPP_InterpretCommand(const char* command)
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
        EQAPP_Unload();
        return;
    }

    // toggle debug
    if (strcmp(command, "//debug") == 0)
    {
        EQ_ToggleBool(g_debugIsEnabled);
        EQAPP_PrintBool("Debug", g_debugIsEnabled);
        return;
    }

    // test
    if (strcmp(command, "//test") == 0)
    {
        std::cout << "test" << std::endl;

        DWORD playerSpawn = EQ_GetPlayerSpawn();
        if (playerSpawn == NULL)
        {
            return;
        }

        FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
        FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
        FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

        unsigned int waypointIndex = EQAPP_WaypointList_GetIndexNearestToLocation(playerY, playerX, playerZ);

        std::cout << "EQAPP_WaypointList_GetIndexNearestToSpawn: " << waypointIndex << std::endl;

        PEQAPPWAYPOINT waypoint = EQAPP_Waypoint_GetByIndex(waypointIndex);
        if (waypoint != NULL)
        {
            EQ_TurnPlayerTowardsLocationGradually(waypoint->y, waypoint->x);
        }

        //DWORD mapViewMap = EQ_GetMapViewMap();
        //if (mapViewMap == NULL)
        //{
            //return;
        //}

        //((MapViewMap*)mapViewMap)->AddLabel(-playerX, -playerY, playerZ, 0xFFFF0000, 2, "TESTING123");

        return;
    }

    // toggle hud
    if (strcmp(command, "//hud") == 0)
    {
        EQ_ToggleBool(g_hudIsEnabled);
        EQAPP_PrintBool("HUD", g_hudIsEnabled);
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

            std::cout << "Loot Result: " << itemName << " , " << std::boolalpha << lootResult << std::noboolalpha << std::endl;
        }
    }

    // toggle auto loot
    if (strcmp(command, "//autoloot") == 0 || strcmp(command, "//al") == 0)
    {
        EQAPP_AutoLoot_Toggle();
        return;
    }

    // reset auto loot
    if (strcmp(command, "//resetautoloot") == 0 || strcmp(command, "//resetal") == 0 || strcmp(command, "//ral") == 0)
    {
        EQAPP_AutoLoot_Reset();
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
        DWORD playerSpawn = EQ_GetPlayerSpawn();
        DWORD targetSpawn = EQ_GetTargetSpawn();

        if (playerSpawn != NULL && targetSpawn != NULL)
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
        DWORD playerSpawn = EQ_GetPlayerSpawn();
        DWORD targetSpawn = EQ_GetTargetSpawn();

        if (playerSpawn != NULL && targetSpawn != NULL)
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
        DWORD targetSpawn = EQ_GetTargetSpawn();

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
        EQAPP_PrintBool("ESP", g_espIsEnabled);
        return;
    }

    // toggle esp spawn
    if (strcmp(command, "//espspawn") == 0 || strcmp(command, "//esps") == 0)
    {
        EQ_ToggleBool(g_espSpawnIsEnabled);
        EQAPP_PrintBool("ESP Spawn", g_espSpawnIsEnabled);
        return;
    }

    // toggle esp ground spawn
    if (strcmp(command, "//espgroundspawn") == 0 || strcmp(command, "//espg") == 0)
    {
        EQ_ToggleBool(g_espGroundSpawnIsEnabled);
        EQAPP_PrintBool("ESP Ground Spawn", g_espGroundSpawnIsEnabled);
        return;
    }

    // toggle esp door
    if (strcmp(command, "//espdoor") == 0 || strcmp(command, "//espd") == 0)
    {
        EQ_ToggleBool(g_espDoorIsEnabled);
        EQAPP_PrintBool("ESP Door", g_espDoorIsEnabled);
        return;
    }

    // toggle esp skeleton
    if (strcmp(command, "//espskeleton") == 0 || strcmp(command, "//espsk") == 0)
    {
        EQ_ToggleBool(g_espSkeletonIsEnabled);
        EQAPP_PrintBool("ESP Skeleton", g_espSkeletonIsEnabled);
        return;
    }

    // toggle esp zone object
    if (strcmp(command, "//espzoneobject") == 0 || strcmp(command, "//espzo") == 0)
    {
        EQ_ToggleBool(g_espZoneObjectIsEnabled);
        EQAPP_PrintBool("ESP Zone Object", g_espZoneObjectIsEnabled);
        return;
    }

    // toggle esp custom
    if (strcmp(command, "//espcustom") == 0 || strcmp(command, "//espc") == 0)
    {
        EQ_ToggleBool(g_espCustomIsEnabled);
        EQAPP_PrintBool("ESP Custom", g_espCustomIsEnabled);
        return;
    }

    // toggle esp waypoint
    if (strcmp(command, "//espwaypoint") == 0 || strcmp(command, "//espwp") == 0)
    {
        EQ_ToggleBool(g_espWaypointIsEnabled);
        EQAPP_PrintBool("ESP Waypoint", g_espWaypointIsEnabled);
        return;
    }

    // load esp custom
    if (strcmp(command, "//loadespcustom") == 0 || strcmp(command, "//loadec") == 0 || strcmp(command, "//lec") == 0)
    {
        EQAPP_LoadEspCustom();
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

    // set esp custom distance
    if (strncmp(command, "//setespcustomdistance ", 23) == 0 || strncmp(command, "//setespc ", 10) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);
        if (result == 2)
        {
            g_espCustomDistance = distance;

            std::cout << "ESP Custom Distance: " << distance << std::endl;
        }

        return;
    }

    // set esp waypoint distance
    if (strncmp(command, "//setespwaypointdistance ", 25) == 0 || strncmp(command, "//setespwp ", 11) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);
        if (result == 2)
        {
            g_espWaypointDistance = distance;

            std::cout << "ESP Waypoint Distance: " << distance << std::endl;
        }

        return;
    }

    // get esp custom location
    if (strcmp(command, "//getespcustomlocation") == 0 || strcmp(command, "//getecl") == 0 || strcmp(command, "//gecl") == 0)
    {
        DWORD spawnInfo = EQ_GetTargetSpawn();
        if (spawnInfo == NULL)
        {
            spawnInfo = EQ_GetPlayerSpawn();
            if (spawnInfo == NULL)
            {
                return;
            }
        }

        EQ_CopySpawnEspCustomLocationToClipboard(spawnInfo);

        std::cout << "Copying esp custom location to the clipboard..." << std::endl;

        return;
    }

    // toggle speed
    if (strcmp(command, "//speed") == 0)
    {
        EQ_ToggleBool(g_speedHackIsEnabled);
        EQAPP_PrintBool("Speed Hack", g_speedHackIsEnabled);
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
        EQAPP_PrintBool("Locator", g_locatorIsEnabled);
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
        EQAPP_PrintBool("Find", g_findIsEnabled);
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
        EQAPP_PrintBool("Line to Target", g_lineToTargetIsEnabled);
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
            EQAPP_Memory_ToggleByIndex(index);
        }

        return;
    }

    // always attack
    if (strcmp(command, "//alwaysattack") == 0 || strcmp(command, "//aa") == 0)
    {
        EQ_ToggleBool(g_alwaysAttackIsEnabled);
        EQAPP_PrintBool("Always Attack", g_alwaysAttackIsEnabled);
        return;
    }

    // toggle combat hotbutton
    if (strcmp(command, "//combathotbutton") == 0 || strcmp(command, "//chb") == 0)
    {
        EQ_ToggleBool(g_combatHotbuttonIsEnabled);
        EQAPP_PrintBool("Combat Hotbutton", g_combatHotbuttonIsEnabled);
        return;
    }

    // set combat hotbutton
    if (strncmp(command, "//setcombathotbutton ", 21) == 0 || strncmp(command, "//setchb ", 9) == 0 || strncmp(command, "//schb ", 7) == 0)
    {
        char commandEx[128];

        unsigned int buttonIndex = 0;
        unsigned int delay       = 1;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &buttonIndex, &delay);
        if (result == 3)
        {
            // TODO make function for this

            if (buttonIndex < 1 || buttonIndex > 10)
            {
                std::cout << "No hotbutton exists at specified index: " << buttonIndex << std::endl;
                return;
            }

            g_combatHotbuttonIndex = buttonIndex - 1;

            g_combatHotbuttonTimerDelay = delay * 1000;

            std::cout << "Combat Hotbutton: " << buttonIndex << " (" << delay << " second(s))" << std::endl;

            g_combatHotbuttonIsEnabled = true;
        }

        return;
    }

    // toggle always hotbutton
    if (strcmp(command, "//alwayshotbutton") == 0 || strcmp(command, "//ahb") == 0)
    {
        EQ_ToggleBool(g_alwaysHotbuttonIsEnabled);
        EQAPP_PrintBool("Always Hotbutton", g_alwaysHotbuttonIsEnabled);
        return;
    }

    // set always hotbutton
    if (strncmp(command, "//setalwayshotbutton ", 21) == 0 || strncmp(command, "//setahb ", 9) == 0 || strncmp(command, "//sahb ", 7) == 0)
    {
        char commandEx[128];

        unsigned int buttonIndex = 0;
        unsigned int delay       = 1;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &buttonIndex, &delay);
        if (result == 3)
        {
            // TODO: make function for this

            if (buttonIndex < 1 || buttonIndex > 10)
            {
                std::cout << "No hotbutton exists at specified index: " << buttonIndex << std::endl;
                return;
            }

            g_alwaysHotbuttonIndex = buttonIndex - 1;

            g_alwaysHotbuttonTimerDelay = delay * 1000;

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

            DWORD playerSpawn = EQ_GetPlayerSpawn();
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
        EQAPP_PrintBool("Draw Distance", g_drawDistanceIsEnabled);
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
            // TODO: make function

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

            std::cout << "Field of View: " << fov << std::endl;
        }

        return;
    }

    // toggle target beep
    if (strcmp(command, "//targetbeep") == 0 || strcmp(command, "//tbeep") == 0)
    {
        EQ_ToggleBool(g_targetBeepIsEnabled);
        EQAPP_PrintBool("Target Beep", g_targetBeepIsEnabled);
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

            g_targetBeepTimerDelay = delay * 1000;

            std::cout << "Target Beep: " << name << " (" << delay << " second(s))" << std::endl;

            g_targetBeepIsEnabled = true;
        }

        return;
    }

    // toggle spawn beep
    if (strcmp(command, "//spawnbeep") == 0 || strcmp(command, "//sbeep") == 0)
    {
        EQ_ToggleBool(g_spawnBeepIsEnabled);
        EQAPP_PrintBool("Spawn Beep", g_spawnBeepIsEnabled);
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
        EQAPP_PrintBool("Height", g_heightIsEnabled);
        return;
    }

    // set height
    if (strncmp(command, "//setheight ", 12) == 0 || strncmp(command, "//seth ", 7) == 0 || strncmp(command, "//sh ", 5) == 0)
    {
        DWORD targetSpawn = EQ_GetTargetSpawn();
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

    // toggle map labels
    if (strcmp(command, "//maplabels") == 0 || strcmp(command, "//ml") == 0)
    {
        EQ_ToggleBool(g_mapLabelsIsEnabled);
        EQAPP_PrintBool("Map Labels", g_mapLabelsIsEnabled);

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
        EQAPP_PrintBool("Map Labels Filter", g_mapLabelsFilterIsEnabled);
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

        std::string zoneLongName = EQ_GetZoneLongName();
        if (zoneLongName.size() == 0)
        {
            std::cout << __FUNCTION__ << ": //getzoneinfo: zone long name is null" << std::endl;
            return;
        }

        std::string zoneShortName = EQ_GetZoneShortName();
        if (zoneShortName.size() == 0)
        {
            std::cout << __FUNCTION__ << ": //getzoneinfo: zone short name is null" << std::endl;
            return;
        }

        std::cout << "Zone Info: " << zoneLongName << " (" << zoneShortName << ") " << "(ID: " << zoneId << ")" << std::endl;

        return;
    }

    // load memory
    if (strcmp(command, "//loadmemory") == 0 || strcmp(command, "//loadm") == 0 || strcmp(command, "//lm") == 0)
    {
        EQAPP_Memory_Unload();
        EQAPP_Memory_Load();
        return;
    }

    // toggle sounds
    if (strcmp(command, "//sounds") == 0)
    {
        EQ_ToggleBool(g_soundsIsEnabled);
        EQAPP_PrintBool("Sounds", g_soundsIsEnabled);
        return;
    }

    // load sounds
    if (strcmp(command, "//loadsounds") == 0 || strcmp(command, "//loads") == 0 || strcmp(command, "//ls") == 0)
    {
        EQAPP_Sounds_Load();
        return;
    }

    // toggle named spawns
    if (strcmp(command, "//namedspawns") == 0 || strcmp(command, "//ns") == 0)
    {
        EQ_ToggleBool(g_namedSpawnsIsEnabled);
        EQAPP_PrintBool("Named Spawns" , g_namedSpawnsIsEnabled);
        return;
    }

    // load named spawns
    if (strcmp(command, "//loadnamedspawns") == 0 || strcmp(command, "//loadns") == 0 || strcmp(command, "//lns") == 0)
    {
        EQAPP_NamedSpawns_Load();
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
        EQAPP_CharacterFile_Write();
        return;
    }

    // load text overlay chat text
    if (strcmp(command, "//loadtextoverlaychattext") == 0 || strcmp(command, "//loadtoct") == 0 || strcmp(command, "//ltoct") == 0)
    {
        EQAPP_TextOverlayChatText_Load();
        return;
    }

    // load no beep
    if (strcmp(command, "//loadnobeep") == 0 || strcmp(command, "//loadnb") == 0 || strcmp(command, "//lnb") == 0)
    {
        EQAPP_NoBeep_Load();
        return;
    }

    // load zone short names
    if (strcmp(command, "//loadzoneshortnames") == 0 || strcmp(command, "//loadzsn") == 0 || strcmp(command, "//lzsn") == 0)
    {
        EQAPP_ZoneShortNames_Load();
        return;
    }

    // toggle hide corpse looted
    if (strcmp(command, "//hidecorpselooted") == 0 || strcmp(command, "//hcl") == 0)
    {
        EQ_ToggleBool(g_hideCorpseLootedIsEnabled);
        EQAPP_PrintBool("Hide Corpse Looted", g_hideCorpseLootedIsEnabled);
        return;
    }

    // open all doors
    if (strcmp(command, "//openalldoors") == 0 || strcmp(command, "//opendoors") == 0 || strcmp(command, "//oad") == 0)
    {
        EQAPP_Doors_OpenAll(true);

        return;
    }

    // close all doors
    if (strcmp(command, "//closealldoors") == 0 || strcmp(command, "//closedoors") == 0 || strcmp(command, "//cad") == 0)
    {
        EQAPP_Doors_OpenAll(false);

        return;
    }

    // census
    if (strcmp(command, "//census") == 0)
    {
        EQAPP_ToggleCensus();
        return;
    }

    // toggle free camera
    if (strcmp(command, "//freecamera") == 0 || strcmp(command, "//freecam") == 0 || strcmp(command, "//fc") == 0)
    {
        EQAPP_FreeCamera_Toggle();
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
        DWORD targetSpawn = EQ_GetTargetSpawn();
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
        EQAPP_MapLocations_WriteToFile();
        return;
    }

    // get map location
    if (strcmp(command, "//getmaplocation") == 0 || strcmp(command, "//getmaploc") == 0)
    {
        DWORD spawnInfo = NULL;

        spawnInfo = EQ_GetTargetSpawn();
        if (spawnInfo == NULL)
        {
            spawnInfo = EQ_GetPlayerSpawn();
            if (spawnInfo == NULL)
            {
                return;
            }
        }

        EQ_CopySpawnMapLocationToClipboard(spawnInfo);

        std::cout << "Copying map location to the clipboard..." << std::endl;

        return;
    }

    // open map file
    if (strcmp(command, "//openmapfile") == 0 || strcmp(command, "//omf") == 0)
    {
        std::string zoneShortName = EQ_GetZoneShortName();
        if (zoneShortName.size() == 0)
        {
            std::cout << __FUNCTION__ << ": //openmapfile: zone short name is null" << std::endl;
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

    // waypoint add
    if (strcmp(command, "//waypointadd") == 0 || strcmp(command, "//wpa") == 0)
    {
        EQAPP_Waypoint_Add();
        return;
    }

    // waypoint remove
    if (strncmp(command, "//waypointremove ", 17) == 0 || strncmp(command, "//wpr ", 6) == 0)
    {
        char commandEx[128];

        unsigned int index;

        int result = sscanf_s(command, "%s %d", commandEx, sizeof(commandEx), &index);
        if (result == 2)
        {
            EQAPP_Waypoint_Remove(index);
        }

        return;
    }

    // waypoint connect
    if (strncmp(command, "//waypointconnect ", 18) == 0 || strncmp(command, "//wpc ", 6) == 0)
    {
        char commandEx[128];

        unsigned int fromIndex;
        unsigned int toIndex;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &fromIndex, &toIndex);
        if (result == 3)
        {
            EQAPP_Waypoint_Connect(fromIndex, toIndex);
        }

        return;
    }

    // waypoint disconnect
    if (strncmp(command, "//waypointdisconnect ", 21) == 0 || strncmp(command, "//wpd ", 6) == 0)
    {
        char commandEx[128];

        unsigned int fromIndex;
        unsigned int toIndex;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &fromIndex, &toIndex);
        if (result == 3)
        {
            EQAPP_Waypoint_Disconnect(fromIndex, toIndex);
        }

        return;
    }

    // waypoint list print
    if (strcmp(command, "//waypointlist") == 0 || strcmp(command, "//wpl") == 0 || strcmp(command, "//waypointlistprint") == 0 || strcmp(command, "//wplp") == 0)
    {
        EQAPP_WaypointList_Print();
        return;
    }

    // waypoint list clear
    if (strcmp(command, "//waypointlistclear") == 0 || strcmp(command, "//wplc") == 0)
    {
        EQAPP_WaypointList_Clear();
        return;
    }

    // waypoint list load
    if (strcmp(command, "//waypointlistload") == 0 || strcmp(command, "//wpll") == 0)
    {
        EQAPP_WaypointList_Load();
        return;
    }

    // waypoint list save
    if (strcmp(command, "//waypointlistsave") == 0 || strcmp(command, "//wpls") == 0)
    {
        EQAPP_WaypointList_Save();
        return;
    }

    // waypoint list get path
    if (strncmp(command, "//waypointlistgetpath ", 22) == 0 || strncmp(command, "//wplgp ", 8) == 0)
    {
        char commandEx[128];

        unsigned int fromIndex;
        unsigned int toIndex;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &fromIndex, &toIndex);

        if (result == 3)
        {
            EQAPP_WaypointList_GetPath(fromIndex, toIndex);
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

    DWORD playerSpawn = EQ_GetPlayerSpawn();

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
    EQAPP_Unload();

    return EQAPP_REAL_Exit();
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

    EQAPP_WindowTitles_Execute();

    EQAPP_FreeCamera_Keys();

    EQAPP_AutoLoot_Execute();
    EQAPP_DoSpawnCastSpell();
    EQAPP_DoTargetBeep();
    EQAPP_DoCombatHotbutton();
    EQAPP_DoAlwaysHotbutton();
    EQAPP_DoAlwaysAttack();
    EQAPP_DoMaxSwimming();
    EQAPP_DoHeight();
    EQAPP_DoDrawDistance();
    EQAPP_DoHideCorpseLooted();
    EQAPP_Census_Execute();

    if (EQAPP_IsAnImportantWindowOpen() == false && EQ_IsKeyShiftPressed() == false)
    {
        EQAPP_LineToTarget_Execute();
        EQAPP_DoEsp();
        EQAPP_NamedSpawns_Execute();
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
            DWORD playerSpawn = EQ_GetPlayerSpawn();
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

    if (g_censusIsEnabled == true)
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

    EQAPP_FreeCamera_Set(false);

    EQAPP_CharacterFile_Write();

    EQAPP_TextOverlayChatText_Load();
    EQAPP_NoBeep_Load();
    EQAPP_NamedSpawns_Load();
    EQAPP_LoadEspCustom();

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
        DWORD playerSpawn = EQ_GetPlayerSpawn();

        if (playerSpawn != NULL && (DWORD)pThis == playerSpawn)
        {
            EQAPP_FreeCamera_Set(false);
        }
    }

    if (a1 == EQ_STANDING_STATE_DUCKING)
    {
        g_censusIsEnabled = false;
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
        EQAPP_DoMapLabels();
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

    std::cout << "The game is saving map layer #" << a2 << std::endl;

    EQAPP_RemoveMapLabels();

    return EQAPP_REAL_MapViewMap__SaveEx(pThis, a1, a2);
}

DWORD WINAPI EQAPP_ThreadConsole(LPVOID param)
{
    if (!glfwInit())
    {
        return 0;
    }

    g_consoleWindow = glfwCreateWindow(g_consoleWindowWidth, g_consoleWindowHeight, "EQAPP Console", NULL, NULL);
    if (!g_consoleWindow)
    {
        glfwTerminate();
        ImGui_ImplGlfw_Shutdown();

        g_bConsole = 0;

        return 0;
    }

    glfwMakeContextCurrent(g_consoleWindow);
    ImGui_ImplGlfw_Init(g_consoleWindow, true);

    g_hwndConsoleWindow = glfwGetWin32Window(g_consoleWindow);

    EQAPP_CenterWindow(g_hwndConsoleWindow);

    // disable close button
    EnableMenuItem(GetSystemMenu(g_hwndConsoleWindow, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

    const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    g_videoModeWidth  = videoMode->width;
    g_videoModeHeight = videoMode->height;

    EQAPP_Console_Load();

    ImVec4 clearColor = ImColor(0, 128, 96);

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
            glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
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
    EQAPP_LoadGraphicsDllFunctions();

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
