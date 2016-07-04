#ifndef EQAPP_TEXTOVERLAYCHATTEXT_H
#define EQAPP_TEXTOVERLAYCHATTEXT_H

bool g_textOverlayOnChatTextIsEnabled = true;
std::vector<std::string> g_textOverlayChatTextList;
int g_textOverlayChatTextDuration = 5000;

void EQAPP_TextOverlayChatText_Load();

void EQAPP_TextOverlayChatText_Load()
{
    std::cout << "Loading Text Overlay Chat Text..." << std::endl;

    g_textOverlayChatTextList.clear();

    std::ifstream file;
    file.open("eqapp/textoverlaychattext.txt", std::ios::in);
    if (file.is_open() == false)
    {
        std::cout << __FUNCTION__ << ": failed to open file: eqapp/textoverlaychattext.txt" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        g_textOverlayChatTextList.push_back(line);
    }

    file.close();
}

#endif // EQAPP_TEXTOVERLAYCHATTEXT_H
