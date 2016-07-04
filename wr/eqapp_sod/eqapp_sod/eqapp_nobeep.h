#ifndef EQAPP_NOBEEP_H
#define EQAPP_NOBEEP_H

std::vector<std::string> g_noBeepList;

void EQAPP_NoBeep_Load();

void EQAPP_NoBeep_Load()
{
    std::cout << "Loading No Beep..." << std::endl;

    g_noBeepList.clear();

    std::ifstream file;
    file.open("eqapp/nobeep.txt", std::ios::in);
    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": failed to open file: eqapp/nobeep.txt" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        g_noBeepList.push_back(line);
    }

    file.close();
}

#endif // EQAPP_NOBEEP_H
