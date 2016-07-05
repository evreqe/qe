#ifndef EQAPP_MAPLOCATIONS_H
#define EQAPP_MAPLOCATIONS_H

void EQAPP_MapLocations_WriteToFile();

void EQAPP_MapLocations_WriteToFile()
{
    std::cout << "Saving map locations to file..." << std::endl;

    std::string zoneShortName = EQ_GetZoneShortName();
    if (zoneShortName.size() == 0)
    {
        std::cout << __FUNCTION__ << ": zone short name is null" << std::endl;
        return;
    }

    std::stringstream filePath;
    filePath << "eqapp/maplocations/" << zoneShortName << ".txt";

    std::fstream file;
    file.open(filePath.str().c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": failed to open file: " << filePath.str() << std::endl;
        return;
    }

    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125);

        if (spawnType != EQ_SPAWN_TYPE_NPC)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        std::string mapLocation = EQ_GetSpawnMapLocationString(spawn);

        file << mapLocation << std::endl;

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    file.close();
}

#endif // EQAPP_MAPLOCATIONS_H
