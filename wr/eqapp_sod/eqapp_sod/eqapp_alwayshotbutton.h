#ifndef EQAPP_ALWAYSHOTBUTTON_H
#define EQAPP_ALWAYSHOTBUTTON_H

bool g_alwaysHotbuttonIsEnabled = false;
unsigned int g_alwaysHotbuttonIndex = 0;
DWORD g_alwaysHotbuttonTimer = 0;
DWORD g_alwaysHotbuttonTimerDelay = 1000;

void EQAPP_AlwaysHotbutton_Execute();

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

    EQ_CHotButtonWnd->DoHotButton(g_alwaysHotbuttonIndex, 0);
}

#endif // EQAPP_ALWAYSHOTBUTTON_H
