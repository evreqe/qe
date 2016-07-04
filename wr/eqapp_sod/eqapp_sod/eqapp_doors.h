#ifndef EQAPP_DOORS_H
#define EQAPP_DOORS_H

void EQAPP_Doors_OpenAll(bool bOpen);

void EQAPP_Doors_OpenAll(bool bOpen)
{
    std::cout << "Setting open state for ALL doors to: " << std::boolalpha << bOpen << std::noboolalpha << std::endl;

    DWORD switchManager = EQ_ReadMemory<DWORD>(EQ_POINTER_SWITCH_MANAGER);
    if (switchManager == NULL)
    {
        std::cout << __FUNCTION__ << ": switch manager is null" << std::endl;
        return;
    }

    DWORD numDoors = EQ_ReadMemory<DWORD>(switchManager + 0x00);
    if (numDoors == 0)
    {
        std::cout << __FUNCTION__ << ": number of doors is zero" << std::endl;
        return;
    }

    for (size_t i = 0; i < numDoors; i++)
    {
        DWORD doorInfo = EQ_ReadMemory<DWORD>(switchManager + 0x04 + (i * 4));
        if (doorInfo == NULL)
        {
            continue;
        }

        if (bOpen == true)
        {
            EQ_Door_SetState(doorInfo, EQ_DOOR_STATE_OPEN);
        }
        else
        {
            EQ_Door_SetState(doorInfo, EQ_DOOR_STATE_CLOSED);
        }
    }
}

#endif // EQAPP_DOORS_H
