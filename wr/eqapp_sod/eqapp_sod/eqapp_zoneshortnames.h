#ifndef EQAPP_ZONESHORTNAMES_H
#define EQAPP_ZONESHORTNAMES_H

std::vector<std::string> g_zoneShortNamesList;
std::vector<std::string>::iterator g_zoneShortNamesListIterator;

void EQAPP_ZoneShortNames_Load();

void EQAPP_ZoneShortNames_Load()
{
    std::cout << "Loading short zone names..." << std::endl;

    g_zoneShortNamesList.clear();

    std::ifstream file;
    file.open("eqapp/zoneshortnames.txt", std::ios::in);
    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": failed to open file: eqapp/zoneshortnames.txt" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        g_zoneShortNamesList.push_back(line);
    }

    g_zoneShortNamesListIterator = g_zoneShortNamesList.begin();

    file.close();
}

#endif // EQAPP_ZONESHORTNAMES_H
