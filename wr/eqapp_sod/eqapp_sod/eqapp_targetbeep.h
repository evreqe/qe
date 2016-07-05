#ifndef EQAPP_TARGETBEEP_H
#define EQAPP_TARGETBEEP_H

bool g_targetBeepIsEnabled = false;
DWORD g_targetBeepTimer = 0;
DWORD g_targetBeepTimerDelay = 1000;
std::string g_targetBeepName;

void EQAPP_TargetBeep_Execute();

void EQAPP_TargetBeep_Execute()
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

#endif // EQAPP_TARGETBEEP_H
