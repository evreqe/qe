#ifndef EQAPP_AUTOLOOT_H
#define EQAPP_AUTOLOOT_H

bool g_autoLootIsEnabled = false;
std::vector<std::string> g_autoLootList;
DWORD g_autoLootTimer = 0;
DWORD g_autoLootTimerDelay = 500;

void EQAPP_AutoLoot_Execute();
void EQAPP_AutoLoot_Toggle();
void EQAPP_AutoLoot_Reset();

void EQAPP_AutoLoot_Execute()
{
    if (g_autoLootIsEnabled == false || g_autoLootList.size() == 0)
    {
        return;
    }

    if (EQ_IsWindowVisible(EQ_POINTER_CLootWnd) == false)
    {
        g_autoLootTimer = EQ_GetTimer();
        return;
    }

    if (EQ_HasTimePassed(g_autoLootTimer, g_autoLootTimerDelay) == false)
    {
        return;
    }

    for (auto& itemName : g_autoLootList)
    {
        bool result = EQ_LootItemByName(itemName.c_str());
        if (result == true)
        {
            std::cout << "Auto Loot Item: " << itemName << std::endl;
        }
    }
}

void EQAPP_AutoLoot_Toggle()
{
    EQ_ToggleBool(g_autoLootIsEnabled);

    if (g_autoLootIsEnabled == false)
    {
        g_autoLootList.clear();
    }

    EQAPP_PrintBool("Auto Loot", g_autoLootIsEnabled);
}

void EQAPP_AutoLoot_Reset()
{
    g_autoLootList.clear();

    std::cout << "Auto Loot reset." << std::endl;
}

#endif // EQAPP_AUTOLOOT_H
