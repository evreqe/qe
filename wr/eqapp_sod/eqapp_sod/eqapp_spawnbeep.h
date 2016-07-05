#ifndef EQAPP_SPAWNBEEP_H
#define EQAPP_SPAWNBEEP_H

bool g_spawnBeepIsEnabled = false;
std::string g_spawnBeepName;

void EQAPP_SpawnBeep_Execute(DWORD spawnInfo);

void EQAPP_SpawnBeep_Execute(DWORD spawnInfo)
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

#endif // EQAPP_SPAWNBEEP_H
