#ifndef EQAPP_COMBATHOTBUTTON_H
#define EQAPP_COMBATHOTBUTTON_H

bool g_combatHotbuttonIsEnabled = false;
unsigned int g_combatHotbuttonIndex = 0;
DWORD g_combatHotbuttonTimer = 0;
DWORD g_combatHotbuttonTimerDelay = 1000;

void EQAPP_CombatHotbutton_Execute();
void EQAPP_CombatHotbutton_Set(unsigned buttonNumber, DWORD timerDelayInSeconds);

void EQAPP_CombatHotbutton_Execute()
{
    if (g_combatHotbuttonIsEnabled == false)
    {
        return;
    }

    if (EQ_IsAutoAttackEnabled() == false)
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

    if (EQ_IsPlayerCastingSpell() == true)
    {
        return;
    }

    if (EQ_HasTimePassed(g_combatHotbuttonTimer, g_combatHotbuttonTimerDelay) == false)
    {
        return;
    }

    EQ_DoHotButton(g_combatHotbuttonIndex);
}

void EQAPP_CombatHotbutton_Set(unsigned buttonNumber, DWORD timerDelayInSeconds)
{
    if (buttonNumber < EQ_HOTBUTTON_NUMBER_MIN || buttonNumber > EQ_HOTBUTTON_NUMBER_MAX)
    {
        std::cout << "[error] " << __FUNCTION__ << ": invalid hotbutton number specified: " << buttonNumber << std::endl;
        return;
    }

    g_combatHotbuttonIndex = buttonNumber - 1; // convert number to index

    g_combatHotbuttonTimerDelay = (DWORD)(timerDelayInSeconds * 1000); // convert seconds to milliseconds

    std::cout << "Combat Hotbutton: " << buttonNumber << " (" << timerDelayInSeconds << " second(s))" << std::endl;
}

#endif // EQAPP_COMBATHOTBUTTON_H