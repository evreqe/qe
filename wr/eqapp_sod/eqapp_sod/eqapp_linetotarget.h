#ifndef EQAPP_LINETOTARGET_H
#define EQAPP_LINETOTARGET_H

bool g_lineToTargetIsEnabled = true;
DWORD g_lineToTargetColor = 0xFFFF00FF;

void EQAPP_LineToTarget_Execute();

void EQAPP_LineToTarget_Execute()
{
    if (g_lineToTargetIsEnabled == false)
    {
        return;
    }

    DWORD targetSpawn = EQ_GetTargetSpawn();
    if (targetSpawn == NULL)
    {
        return;
    }

    FLOAT targetY = EQ_ReadMemory<FLOAT>(targetSpawn + 0x64);
    FLOAT targetX = EQ_ReadMemory<FLOAT>(targetSpawn + 0x68);
    FLOAT targetZ = EQ_ReadMemory<FLOAT>(targetSpawn + 0x6C);

    int screenX = -1;
    int screenY = -1;
    bool result = EQ_WorldSpaceToScreenSpace(targetX, targetY, targetZ, screenX, screenY);
    if (result == true)
    {
        DWORD windowWidth  = EQ_ReadMemory<DWORD>(EQ_WINDOW_WIDTH);
        DWORD windowHeight = EQ_ReadMemory<DWORD>(EQ_WINDOW_HEIGHT);

        EQ_DrawLine((float)(windowWidth / 2), (float)windowHeight, 0.0f, (float)screenX, (float)screenY, 0.0f, g_lineToTargetColor);
    }
}

#endif // EQAPP_LINETOTARGET_H
