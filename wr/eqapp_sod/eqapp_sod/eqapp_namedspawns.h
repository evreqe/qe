#ifndef EQAPP_NAMEDSPAWNS_H
#define EQAPP_NAMEDSPAWNS_H

bool g_namedSpawnsIsEnabled = true;
std::vector<std::string> g_namedSpawnsList;
unsigned int g_namedSpawnsX = 1200;
unsigned int g_namedSpawnsY = 28;
float g_namedSpawnsWidth  = 256.0f;
float g_namedSpawnsHeight = 512.0f;
DWORD g_namedSpawnsColor = 0xFFFFFFFF;

void EQAPP_NamedSpawns_Load();
void EQAPP_NamedSpawns_Execute();

void EQAPP_NamedSpawns_Load()
{
    std::cout << "Loading Named Spawns..." << std::endl;

    g_namedSpawnsList.clear();

    std::ifstream file;
    std::string line;

    file.open("eqapp/namedspawns.txt", std::ios::in);
    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": failed to open file: eqapp/namedspawns.txt" << std::endl;
        return;
    }

    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        std::cout << __FUNCTION__ << ": "<< line << std::endl;

        g_namedSpawnsList.push_back(line);
    }

    file.close();

    std::string zoneShortName = EQ_GetZoneShortName();
    if (zoneShortName.size() == 0)
    {
        std::cout << __FUNCTION__ << ": zone short name is null" << std::endl;
        return;
    }

    std::stringstream filePath;
    filePath << "eqapp/namedspawns/" << zoneShortName << ".txt";

    file.open(filePath.str().c_str(), std::ios::in);
    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": failed to open file: " << filePath.str() << std::endl;
        return;
    }

    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        std::cout << __FUNCTION__ << ": "<< line << std::endl;

        g_namedSpawnsList.push_back(line);
    }

    file.close();
}

void EQAPP_NamedSpawns_Execute()
{
    if (g_namedSpawnsIsEnabled == false || g_namedSpawnsList.size() == 0)
    {
        return;
    }

    unsigned int fontSize   = 2;
    unsigned int fontHeight = EQ_GetFontHeight(fontSize);

    unsigned int numSpawns = 0;

    if (g_namedSpawnsWidth > 0 && g_namedSpawnsHeight > 0)
    {
        EQ_DrawQuad((float)g_namedSpawnsX, (float)g_namedSpawnsY, g_namedSpawnsWidth, g_namedSpawnsHeight, EQ_TOOLTIP_COLOR);
    }

    std::stringstream ssDrawText;

    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125);

        if (spawnType != EQ_SPAWN_TYPE_NPC)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        int spawnIsPet = EQ_ReadMemory<DWORD>(spawn + 0x260);

        if (spawnIsPet != 0)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        int spawnLevel = EQ_ReadMemory<BYTE>(spawn + 0x315);

        if (spawnLevel < 1 || spawnLevel > 100)
        {
            spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
            continue;
        }

        char spawnName[0x40] = {0};
        memcpy(spawnName, (LPVOID)(spawn + 0xE4), sizeof(spawnName));

        for (auto& namedSpawn : g_namedSpawnsList)
        {
            if (strstr(spawnName, namedSpawn.c_str()) != NULL)
            {
                ssDrawText << spawnName << "\n";
                numSpawns++;
                break;
            }
        }

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    EQ_DrawText(ssDrawText.str().c_str(), g_namedSpawnsX, g_namedSpawnsY, g_namedSpawnsColor, fontSize);

    if (numSpawns > 0)
    {
        g_namedSpawnsHeight = (float)(numSpawns * fontHeight);
    }
    else
    {
        g_namedSpawnsHeight = 0.0f;
    }
}

#endif // EQAPP_NAMEDSPAWNS_H