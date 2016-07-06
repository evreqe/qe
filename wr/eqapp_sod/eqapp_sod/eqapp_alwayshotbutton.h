#ifndef EQAPP_ALWAYSHOTBUTTON_H
#define EQAPP_ALWAYSHOTBUTTON_H

bool g_alwaysHotbuttonIsEnabled = false;
unsigned int g_alwaysHotbuttonIndex = 0;
DWORD g_alwaysHotbuttonTimer = 0;
DWORD g_alwaysHotbuttonTimerDelay = 1000;

void EQAPP_AlwaysHotbutton_Execute();
void EQAPP_AlwaysHotbutton_Set(unsigned buttonNumber, DWORD timerDelayInSeconds);

void EQAPP_AlwaysHotbutton_Execute()
{
    if (g_alwaysHotbuttonIsEnabled == false)
    {
        return;
    }

    if (EQ_HasTimePassed(g_alwaysHotbuttonTimer, g_alwaysHotbuttonTimerDelay) == false)
    {
        return;
    }

    EQ_DoHotButton(g_alwaysHotbuttonIndex);
}

void EQAPP_AlwaysHotbutton_Set(unsigned buttonNumber, DWORD timerDelayInSeconds)
{
    if (buttonNumber < EQ_HOTBUTTON_NUMBER_MIN || buttonNumber > EQ_HOTBUTTON_NUMBER_MAX)
    {
        std::cout << "[error] " << __FUNCTION__ << ": invalid hotbutton number specified: " << buttonNumber << std::endl;
        return;
    }

    g_alwaysHotbuttonIndex = buttonNumber - 1; // convert number to index

    g_alwaysHotbuttonTimerDelay = (DWORD)(timerDelayInSeconds * 1000); // convert seconds to milliseconds

    std::cout << "Always Hotbutton: " << buttonNumber << " (" << timerDelayInSeconds << " second(s))" << std::endl;
}

#endif // EQAPP_ALWAYSHOTBUTTON_H