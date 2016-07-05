#ifndef EQAPP_CHANGEHEIGHT_H
#define EQAPP_CHANGEHEIGHT_H

bool g_changeHeightIsEnabled = true;
float g_changeHeightMinimum = 1.0f;
float g_changeHeightMaximum = 5.0f;

void EQAPP_ChangeHeight_Execute();

void EQAPP_ChangeHeight_Execute()
{
    if (g_changeHeightIsEnabled == false)
    {
        return;
    }

    DWORD playerSpawn = EQ_GetPlayerSpawn();
    if (playerSpawn == NULL)
    {
        return;
    }

    float height = EQ_ReadMemory<FLOAT>(playerSpawn + 0x13C);

    if (height < g_changeHeightMinimum)
    {
        EQ_SetSpawnHeight(playerSpawn, g_changeHeightMinimum);
    }
    else if (height > g_changeHeightMaximum)
    {
        EQ_SetSpawnHeight(playerSpawn, g_changeHeightMaximum);
    }
}

#endif // EQAPP_CHANGEHEIGHT_H
