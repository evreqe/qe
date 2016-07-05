#ifndef EQAPP_BANKLIST_H
#define EQAPP_BANKLIST_H

void EQAPP_BankList_Print(const char* filterItemName);

void EQAPP_BankList_Print(const char* filterItemName)
{
    if (EQ_IsInGame() == false)
    {
        std::cout << __FUNCTION__ << ": not in-game" << std::endl;
        return;
    }

    DWORD charInfo = EQ_ReadMemory<DWORD>(EQ_POINTER_CHAR_INFO);
    if (charInfo == NULL)
    {
        std::cout << __FUNCTION__ << ": char info is null" << std::endl;
        return;
    }

    EQAPP_COUT_SaveFlags();

    std::cout << "Bank List:";
    if (filterItemName != NULL)
    {
        std::cout << " " << filterItemName;
    }
    std::cout << std::endl;

    for (size_t i = 0; i < EQ_NUM_BANK_SLOTS; i++)
    {
        DWORD itemInfo = EQ_ReadMemory<DWORD>(charInfo + (0x1128 + (i * 4)));
        if (itemInfo == NULL)
        {
            continue;
        }

        DWORD itemCount = EQ_ReadMemory<DWORD>(itemInfo + 0xD0);

        PCHAR itemName = EQ_ReadMemory<PCHAR>(itemInfo + 0xB8);
        if (itemName != NULL)
        {
            if (filterItemName != NULL)
            {
                DWORD itemSubInfo = EQ_ReadMemory<DWORD>(itemInfo + 0xB8);
                if (itemSubInfo == NULL)
                {
                    continue;
                }

                DWORD itemSlots = EQ_ReadMemory<BYTE>(itemSubInfo + 0x475);
                if (itemSlots == 0)
                {
                    if (strstr(itemName, filterItemName) == NULL)
                    {
                        continue;
                    }
                }
            }

            std::cout << std::setfill('0') << std::setw(2) << i + 1;

            // shared bank slots
            if (i == 24 || i == 25)
            {
                std::cout << "(Listsold)";
            }

            std::cout << ": ";

            if (itemCount > 1)
            {
                std::cout << itemCount << " x ";
            }

            std::cout << itemName << std::endl;
        }

        for (size_t j = 0; j < EQ_NUM_CONTAINER_SLOTS; j++)
        {
            DWORD containerItemInfo = EQ_ReadMemory<DWORD>(itemInfo + (0x0C + (j * 4)));
            if (containerItemInfo == NULL)
            {
                continue;
            }

            DWORD containerItemCount = EQ_ReadMemory<DWORD>(containerItemInfo + 0xD0);

            PCHAR containerItemName = EQ_ReadMemory<PCHAR>(containerItemInfo + 0xB8);
            if (containerItemName != NULL)
            {
                if (filterItemName != NULL)
                {
                    if (strstr(containerItemName, filterItemName) == NULL)
                    {
                        continue;
                    }
                }

                std::cout << std::setfill('0') << std::setw(2) << i + 1 << "-" << std::setfill('0') << std::setw(2) << j + 1 << ": ";

                if (containerItemCount > 1)
                {
                    std::cout << containerItemCount << " x ";
                }

                std::cout << containerItemName << std::endl;
            }
        }
    }

    EQAPP_COUT_RestoreFlags();
}

#endif // 
