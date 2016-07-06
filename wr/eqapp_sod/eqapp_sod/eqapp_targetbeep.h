#ifndef EQAPP_TARGETBEEP_H
#define EQAPP_TARGETBEEP_H

bool g_targetBeepIsEnabled = false;
DWORD g_targetBeepTimer = 0;
DWORD g_targetBeepTimerDelay = 1000;
DWORD g_targetBeepTimerDelayInSeconds = 1;
std::string g_targetBeepName;

void EQAPP_TargetBeep_Execute();
void EQAPP_TargetBeep_Print();
void EQAPP_TargetBeep_Set(std::string spawnName, DWORD timerDelayInSeconds);

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

void EQAPP_TargetBeep_Print()
{
    if (g_targetBeepTimerDelay == 0)
    {
        std::cout << "[error] " << __FUNCTION__ << ": timer delay is zero" << std::endl;
        return;
    }

    std::cout << "Target Beep: " <<  g_targetBeepName << " (" << g_targetBeepTimerDelayInSeconds << " second(s))" << std::endl;
}

void EQAPP_TargetBeep_Set(std::string spawnName, DWORD timerDelayInSeconds)
{
    if (spawnName.size() == 0)
    {
        std::cout << "[error] " << __FUNCTION__ << ": spawn name size is zero" << std::endl;
    }

    g_targetBeepName = spawnName;

    g_targetBeepTimerDelay = (DWORD)(timerDelayInSeconds * 1000); // convert seconds to milliseconds

    g_targetBeepTimerDelayInSeconds = timerDelayInSeconds;

    EQAPP_TargetBeep_Print();
}

#endif // EQAPP_TARGETBEEP_H
