#ifndef EQAPP_ESP_FUNCTIONS_H
#define EQAPP_ESP_FUNCTIONS_H

#include "eqapp_esp.h"

#include "eqapp_esp_custom.h"
#include "eqapp_esp_spawnlist.h"

void EQAPP_ESP_Execute();
void EQAPP_ESP_Spawns_Draw();
void EQAPP_ESP_GroundSpawns_Draw();
void EQAPP_ESP_Doors_Draw();
void EQAPP_ESP_ZoneObjects_Draw();
void EQAPP_ESP_Waypoints_Draw();
void EQAPP_ESP_Locator_Draw();
void EQAPP_ESP_SpawnSkeleton_Draw(DWORD spawnInfo, DWORD argbColor);

void EQAPP_ESP_Execute()
{
    if (g_espIsEnabled == false)
    {
        return;
    }

    EQAPP_ESP_SpawnList_Update();

    g_espNumDrawText = 0;

    // draw order is least important to most important
    EQAPP_ESP_Custom_Draw();
    EQAPP_ESP_Waypoints_Draw();
    EQAPP_ESP_Doors_Draw();
    EQAPP_ESP_GroundSpawns_Draw();
    EQAPP_ESP_Locator_Draw();
    EQAPP_ESP_Spawns_Draw();
}

void EQAPP_ESP_Spawns_Draw()
{
    if (g_espSpawnIsEnabled == false)
    {
        return;
    }

    DWORD playerSpawn = EQ_GetPlayerSpawn();

    FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
    FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
    FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

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
                    EQAPP_ESP_SpawnSkeleton_Draw(spawn.spawnInfo, textColor);
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

        if (g_espFindIsEnabled == true && spawn.isFindSpawn == true)
        {
            DWORD windowWidth  = EQ_ReadMemory<DWORD>(EQ_WINDOW_WIDTH);
            DWORD windowHeight = EQ_ReadMemory<DWORD>(EQ_WINDOW_HEIGHT);

            EQ_DrawLine((float)(windowWidth / 2), (float)windowHeight, 0.0f, (float)screenX, (float)screenY, 0.0f, g_espFindColor);
        }
    }
}

void EQAPP_ESP_GroundSpawns_Draw()
{
    if (g_espGroundSpawnIsEnabled == false)
    {
        return;
    }

    DWORD playerSpawn = EQ_GetPlayerSpawn();

    FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
    FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
    FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

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

void EQAPP_ESP_Doors_Draw()
{
    if (g_espDoorIsEnabled == false)
    {
        return;
    }

    DWORD playerSpawn = EQ_GetPlayerSpawn();

    FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
    FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
    FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

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

void EQAPP_ESP_ZoneObjects_Draw()
{
    if (g_espZoneObjectIsEnabled == false)
    {
        return;
    }

    DWORD playerSpawn = EQ_GetPlayerSpawn();

    FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
    FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
    FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

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

void EQAPP_ESP_Waypoints_Draw()
{
    if (g_espWaypointIsEnabled == false)
    {
        return;
    }

    DWORD playerSpawn = EQ_GetPlayerSpawn();

    FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
    FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
    FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

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

void EQAPP_ESP_Locator_Draw()
{
    if (g_espLocatorIsEnabled == false)
    {
        return;
    }

    DWORD playerSpawn = EQ_GetPlayerSpawn();

    FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
    FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
    FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

    int screenX = -1;
    int screenY = -1;
    bool result = EQ_WorldSpaceToScreenSpace(g_espLocatorX, g_espLocatorY, g_espLocatorZ, screenX, screenY);
    if (result == true)
    {
        float distance = EQ_CalculateDistance3d(playerX, playerY, playerZ, g_espLocatorX, g_espLocatorY, g_espLocatorZ);

        std::stringstream ss;
        ss << "+ Locator (" << (int)distance << ")";

        EQ_DrawText(ss.str().c_str(), screenX, screenY, g_espLocatorColor, 5);
        g_espNumDrawText++;
    }
}

void EQAPP_ESP_SpawnSkeleton_Draw(DWORD spawnInfo, DWORD argbColor)
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

        // draw a line instead of text for performance
        EQ_DrawLine((float)screenX, (float)screenY, 1.0f, (float)screenX, (float)(screenY + 1.0f), 1.0f, argbColor);
    }
}

#endif // EQAPP_ESP_FUNCTIONS_H
