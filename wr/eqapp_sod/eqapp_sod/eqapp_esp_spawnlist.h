#ifndef EQAPP_ESP_SPAWNLIST_H
#define EQAPP_ESP_SPAWNLIST_H

typedef struct _EQAPPESPSPAWN
{
    DWORD spawnInfo;
    char name[0x40];
    char lastName[0x20];
    float y;
    float x;
    float z;
    float distance;
    int level;
    int type;
    int _class;
    int guildId;
    int standingState;
    int isHoldingPrimary;
    int isHoldingSecondary;
    int isPet;
    int isLfg;
    int isPvp;
    int isTrader;
    int isAfk;
    int isGm;
    bool showAtAnyDistance = false;
    bool isTarget = false;
    bool isFindSpawn = false;
    bool isHalfDistance = false;
} EQAPPESPSPAWN, *PEQAPPESPSPAWN;

std::vector<EQAPPESPSPAWN> g_espSpawnList;
DWORD g_espSpawnListTimer = 0;
DWORD g_espSpawnListTimerDelay = 1000;

void EQAPP_ESP_SpawnList_Update();

void EQAPP_ESP_SpawnList_Update()
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

        espSpawn.isHoldingPrimary   = EQ_ReadMemory<BYTE>(spawn + 0x10E0);
        espSpawn.isHoldingSecondary = EQ_ReadMemory<BYTE>(spawn + 0x10E4);

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

        if (g_espFindIsEnabled == true && g_espFindSpawnName.size() != 0)
        {
            if (strstr(espSpawn.name, g_espFindSpawnName.c_str()) != NULL)
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
                if (numNpcCorpse > g_espNumNpcCorpseMax)
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

#endif // EQAPP_ESP_SPAWNLIST_H
