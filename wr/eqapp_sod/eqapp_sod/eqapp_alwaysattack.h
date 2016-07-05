#ifndef EQAPP_ALWAYSATTACK_H
#define EQAPP_ALWAYSATTACK_H

bool g_alwaysAttackIsEnabled = false;

void EQAPP_AlwaysAttack_Execute();

void EQAPP_AlwaysAttack_Execute()
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

    EQ_SetAutoAttack(true);
}

#endif // EQAPP_ALWAYSATTACK_H
