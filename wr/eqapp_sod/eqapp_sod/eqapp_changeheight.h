#ifndef EQAPP_CHANGEHEIGHT_H
#define EQAPP_CHANGEHEIGHT_H

bool g_changeHeightIsEnabled = true;
float g_changeHeightMinimum = 1.0f;
float g_changeHeightMaximum = 5.0f;
DWORD g_changeHeightTimer = 0;
DWORD g_changeHeightTimerDelay = 1000;

void EQAPP_ChangeHeight_Execute();

void EQAPP_ChangeHeight_Execute()
{
    if (g_changeHeightIsEnabled == false)
    {
        return;
    }

    if (EQ_HasTimePassed(g_changeHeightTimer, g_changeHeightTimerDelay) == false)
    {
        return;
    }

    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125);

        if (spawnType == EQ_SPAWN_TYPE_PLAYER)
        {
            float height = EQ_ReadMemory<FLOAT>(spawn + 0x13C);

            if (height < g_changeHeightMinimum)
            {
                EQ_SetSpawnHeight(spawn, g_changeHeightMinimum);
            }
            else if (height > g_changeHeightMaximum)
            {
                EQ_SetSpawnHeight(spawn, g_changeHeightMaximum);
            }
        }

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }
}

#endif // EQAPP_CHANGEHEIGHT_H
