#ifndef EQAPP_CENSUS_H
#define EQAPP_CENSUS_H

bool g_censusIsEnabled = false;
DWORD g_censusTimer = 0;
DWORD g_censusTimerDelay = 1000;

void EQAPP_Census_On();
void EQAPP_Census_Off();
void EQAPP_Census_Toggle();
void EQAPP_Census_Execute();

void EQAPP_Census_On()
{
    std::srand(unsigned(std::time(0)));
    std::random_shuffle(g_zoneShortNamesList.begin(), g_zoneShortNamesList.end());

    g_zoneShortNamesList_it = g_zoneShortNamesList.begin();

    g_censusIsEnabled = true;
    EQAPP_PrintBool("Census", g_censusIsEnabled);
}

void EQAPP_Census_Off()
{
    g_censusIsEnabled = false;
    EQAPP_PrintBool("Census", g_censusIsEnabled);
}

void EQAPP_ToggleCensus()
{
    if (g_censusIsEnabled == false)
    {
        EQAPP_Census_On();
    }
    else
    {
        EQAPP_Census_Off();
    }
}

void EQAPP_Census_Execute()
{
    if (g_censusIsEnabled == false)
    {
        return;
    }

    if (EQ_IsInGame() == false)
    {
        return;
    }

    if (EQ_HasTimePassed(g_censusTimer, g_censusTimerDelay) == false)
    {
        return;
    }

    if (g_zoneShortNamesList.size() == 0)
    {
        EQAPP_Census_Off();
        return;
    }

    if (g_zoneShortNamesList_it == g_zoneShortNamesList.end())
    {
        EQAPP_Census_Off();
        return;
    }

    std::stringstream ssCommand;
    ssCommand << "/who all " << *g_zoneShortNamesList_it;

    std::cout << "Census: " << ssCommand.str() << std::endl;

    DWORD playerSpawn = EQ_GetPlayerSpawn();
    if (playerSpawn == NULL)
    {
        std::cout << __FUNCTION__ << ": player spawn is null" << std::endl;
        EQAPP_Census_Off();
        return;
    }

    EQ_CEverQuest->InterpretCmd(playerSpawn, ssCommand.str().c_str());

    g_zoneShortNamesList_it++;
}

#endif // EQAPP_CENSUS_H
