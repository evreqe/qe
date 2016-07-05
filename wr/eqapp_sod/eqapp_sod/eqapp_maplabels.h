#ifndef EQAPP_MAPLABELS_H
#define EQAPP_MAPLABELS_H

bool g_mapLabelsIsEnabled = false;
bool g_mapLabelsFilterIsEnabled = false;
DWORD g_mapLabelsTimer = 0;
DWORD g_mapLabelsTimerDelay = 10000;
std::string g_mapLabelsFilterName;
DWORD g_mapLabelsData = 255;
DWORD g_mapLabelsSize = 2;
DWORD g_mapLabelsLayer = 3;
DWORD g_mapLabelsWidth = 20;
DWORD g_mapLabelsHeight = 12;

void EQAPP_MapLabels_Remove();
void EQAPP_MapLabels_Execute();

void EQAPP_MapLabels_Remove()
{
    EQ_MapWindow_RemoveLabelByData(g_mapLabelsData);
}

void EQAPP_MapLabels_Execute()
{
    if (g_mapLabelsIsEnabled == false)
    {
        return;
    }

    if (EQ_HasTimePassed(g_mapLabelsTimer, g_mapLabelsTimerDelay) == false)
    {
        return;
    }

    EQAPP_MapLabels_Remove();

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
        location.X = -spawnX; // X and Y must be negative
        location.Y = -spawnY; // X and Y must be negative
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
        mapLabel.Size        = g_mapLabelsSize;
        mapLabel.Label       = (PCHAR)(spawn + 0xE4); // spawn name
        mapLabel.Layer       = g_mapLabelsLayer;
        mapLabel.Width       = g_mapLabelsWidth;
        mapLabel.Height      = g_mapLabelsHeight;
        mapLabel.Data        = g_mapLabelsData;

        EQ_MapWindow_AddLabel(&mapLabel);

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    EQ_UpdateMap();
}

#endif //EQAPP_MAPLABELS_H
