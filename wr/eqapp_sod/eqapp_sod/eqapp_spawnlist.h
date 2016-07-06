#ifndef EQAPP_SPAWNLIST_H
#define EQAPP_SPAWNLIST_H

void EQAPP_SpawnList_Print(const char* filterSpawnName);

void EQAPP_SpawnList_Print(const char* filterSpawnName)
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

#endif // EQAPP_SPAWNLIST_H
