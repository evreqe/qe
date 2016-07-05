#ifndef EQAPP_INTERPRETCOMMAND_H
#define EQAPP_INTERPRETCOMMAND_H

const std::vector<std::string> g_interpretCommandList
{
    "//help",
    "//unload",
    "//debug",
    "//test",
    "//hud",
    "//lootdebug",
    "//loot (item name)",
    "//autoloot, //al",
    "//resetautoloot, //resetal, //ral",
    "//autoloot (item name), //al (item name)",
    "//castray, //cr",
    "//getmeleerange, //getmr, //gmr",
    "//getspellname (spell id), //getsn (spell id), //gsn (spell id)",
    "//gettargetinfo, //getti, //gti",
    "//bank, //money",
    "//banklist, //bl",
    "//banklist (item name), //bl (item name)",
    "//inventorylist, //invlist, //il",
    "//inventorylist (item name), //invlist (item name), //il (item name)",
    "//spawnlist, //sl",
    "//spawnlist (spawn name), //sl (spawn name)",
    "//esp",
    "//espspawn, //esps",
    "//espgroundspawn, //espg",
    "//espdoor, //espd",
    "//espskeleton, //espsk",
    "//espzoneobject, //espzo",
    "//espcustom, //espc",
    "//setespspawndistance (distance), //setesps (distance), //setesp (distance)",
    "//setespgroundspawndistance (distance), //setespg (distance)",
    "//setespdoordistance (distance), //setespd (distance)",
    "//setespskeletondistance (distance), //setespsk (distance)",
    "//setespzoneobjectdistance (distance), //setespzo (distance)",
    "//setespcustomdistance (distance), //setespc (distance)",
    "//speed",
    "//setspeed sow",
    "//setspeed run1",
    "//setspeed run2",
    "//setspeed run3",
    "//setspeed (speed), //sets (speed), //ss (speed)",
    "//locator, //loc",
    "//setlocator (y) (x) (z), //setloc (y) (x) (z)",
    "//find",
    "//setfind (spawn name)",
    "//linetotarget, //ltt",
    "//memorylist, //memlist",
    "//memory (index), //mem (index)",
    "//alwaysattack, //aa",
    "//combathotbutton, //chb",
    "//setcombathotbutton (index), //setchb (index), //schb (index)",
    "//alwayshotbutton, //ahb",
    "//setalwayshotbutton (index), //setahb (index), //sahb (index)",
    "//setcollisionradius (radius), //setcr (radius), //scr (radius)",
    "//drawdistance, //dd",
    "//setdrawdistance (distance), //setdd  (distance)",
    "//setfieldofview (fov), //setfov (fov)",
    "//targetbeep, //tbeep",
    "//settargetbeep (name) (delay), //settbeep (name) (delay), //stb (name) (delay)",
    "//spawnbeep, //sbeep",
    "//setspawnbeep (name), //setsbeep (name), //ssb (name)",
    "//height",
    "//setheight (height), //seth (height), //sh (height)",
    "//maplabels, //ml",
    "//addmaplabels, //addml, //aml",
    "//removemaplabels, //removeml, //rml",
    "//getmaplabels, //getml, //gml",
    "//filtermaplabels, //filterml, //fml",
    "//filtermaplabels (spawn name), //filterml (spawn name), //fml (spawn name)",
    "//getzoneinfo, //getzone, //getzi, //gzi",
    "//loadmemory, //loadm, //lm",
    "//sounds",
    "//loadsounds, //loads, //ls",
    "//namedspawns, //ns",
    "//loadnamedspawns, //loadns, //lns",
    "//loadespcustom, //loadec, //lec",
    "//getnamedspawns, //getns, //gns",
    "//writecharacterfile, //writechar, //writecf, //wcf",
    "//loadtextoverlaychattext, //loadtoct, //ltoct",
    "//loadnobeep, //loadnb, //lnb",
    "//loadshortzonenames, //loadzsn, //lzsn",
    "//hidecorpselooted, //hcl",
    "//openalldoors, //opendoors, //oad",
    "//census",
    "//freecamera, //freecam, //fc",
    "//setviewactor (spawn name), //setview (spawn name), //setva (spawn name), //sva (spawn name)",
    "//setviewtarget, //setvt, //svt",
    "//resetviewactor, //resetview, //resetva, //rva",
    "//maplocations, //maplocs",
    "//getmaplocation, //getmaploc",
    "//getespcustomlocation, //getecl, //gecl",
    "//openmapfile, //omf",
};

void EQAPP_InterpretCommand(const char* command);

void EQAPP_InterpretCommand(const char* command)
{
    // help
    if (strcmp(command, "//help") == 0)
    {
        for (auto& slashCommand : g_interpretCommandList)
        {
            std::cout << slashCommand << std::endl;
        }

        return;
    }

    // unload
    if (strcmp(command, "//unload") == 0)
    {
        EQAPP_Unload();
        return;
    }

    // toggle debug
    if (strcmp(command, "//debug") == 0)
    {
        EQ_ToggleBool(g_debugIsEnabled);
        EQAPP_PrintBool("Debug", g_debugIsEnabled);
        return;
    }

    // test
    if (strcmp(command, "//test") == 0)
    {
        std::cout << "test" << std::endl;

        DWORD playerSpawn = EQ_GetPlayerSpawn();
        if (playerSpawn == NULL)
        {
            return;
        }

        FLOAT playerY = EQ_ReadMemory<FLOAT>(playerSpawn + 0x64);
        FLOAT playerX = EQ_ReadMemory<FLOAT>(playerSpawn + 0x68);
        FLOAT playerZ = EQ_ReadMemory<FLOAT>(playerSpawn + 0x6C);

        unsigned int waypointIndex = EQAPP_WaypointList_GetIndexNearestToLocation(playerY, playerX, playerZ);

        std::cout << "EQAPP_WaypointList_GetIndexNearestToSpawn: " << waypointIndex << std::endl;

        PEQAPPWAYPOINT waypoint = EQAPP_Waypoint_GetByIndex(waypointIndex);
        if (waypoint != NULL)
        {
            EQ_TurnPlayerTowardsLocation(waypoint->y, waypoint->x);
        }

        //DWORD mapViewMap = EQ_GetMapViewMap();
        //if (mapViewMap == NULL)
        //{
            //return;
        //}

        //((MapViewMap*)mapViewMap)->AddLabel(-playerX, -playerY, playerZ, 0xFFFF0000, 2, "TESTING123");

        return;
    }

    // toggle hud
    if (strcmp(command, "//hud") == 0)
    {
        EQ_ToggleBool(g_hudIsEnabled);
        EQAPP_PrintBool("HUD", g_hudIsEnabled);
        return;
    }

    // loot debug
    if (strcmp(command, "//lootdebug") == 0)
    {
        std::cout << "Loot Debug:" << std::endl;

        DWORD lootWindow = EQ_ReadMemory<DWORD>(EQ_POINTER_CLootWnd);
        if (lootWindow == NULL)
        {
            std::cout << "Loot Window is NULL." << std::endl;
            return;
        }

        DWORD lootWindowIsVisible = EQ_ReadMemory<BYTE>(lootWindow + 0x124);
        if (lootWindowIsVisible == 0)
        {
            std::cout << "Loot Window is not open." << std::endl;
            return;
        }

        for (size_t i = 0; i < EQ_NUM_LOOT_WINDOW_SLOTS; i++)
        {
            DWORD itemInfo = EQ_ReadMemory<DWORD>(lootWindow + (0x228 + (i * 4)));
            if (itemInfo == NULL)
            {
                std::cout << i << ": itemInfo == NULL" << std::endl;
                continue;
            }

            PCHAR itemName = EQ_ReadMemory<PCHAR>(itemInfo + 0xB8);
            if (itemName == NULL)
            {
                std::cout << i << ": itemName == NULL" << std::endl;
                continue;
            }

            std::cout << i << ": " << itemName << std::endl;
        }
    }

    // loot
    if (strncmp(command, "//loot ", 7) == 0)
    {
        char commandEx[128];

        char itemName[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), itemName, sizeof(itemName));
        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(itemName);

            bool lootResult = EQ_LootItemByName(itemName);

            std::cout << "Loot Result: " << itemName << " , " << std::boolalpha << lootResult << std::noboolalpha << std::endl;
        }
    }

    // toggle auto loot
    if (strcmp(command, "//autoloot") == 0 || strcmp(command, "//al") == 0)
    {
        EQAPP_AutoLoot_Toggle();
        return;
    }

    // reset auto loot
    if (strcmp(command, "//resetautoloot") == 0 || strcmp(command, "//resetal") == 0 || strcmp(command, "//ral") == 0)
    {
        EQAPP_AutoLoot_Reset();
        return;
    }

    // add to auto loot list
    if (strncmp(command, "//autoloot ", 11) == 0 || strncmp(command, "//al ", 5) == 0)
    {
        char commandEx[128];

        char itemName[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), itemName, sizeof(itemName));
        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(itemName);

            g_autoLootList.push_back(itemName);

            std::cout << "Auto Loot added: " << itemName << std::endl;

            g_autoLootIsEnabled = true;
        }
    }

    // cast ray
    if (strcmp(command, "//castray") == 0 || strcmp(command, "//cr") == 0)
    {
        DWORD playerSpawn = EQ_GetPlayerSpawn();
        DWORD targetSpawn = EQ_GetTargetSpawn();

        if (playerSpawn != NULL && targetSpawn != NULL)
        {
            FLOAT targetY = EQ_ReadMemory<FLOAT>(targetSpawn + 0x64);
            FLOAT targetX = EQ_ReadMemory<FLOAT>(targetSpawn + 0x68);
            FLOAT targetZ = EQ_ReadMemory<FLOAT>(targetSpawn + 0x6C);

            int result = EQ_CastRay(playerSpawn, targetY, targetX, targetZ);

            std::cout << "Cast Ray result: " << result << std::endl;
        }

        return;
    }

    // get melee range
    if (strcmp(command, "//getmeleerange") == 0 || strcmp(command, "//getmr") == 0 || strcmp(command, "//gmr") == 0)
    {
        DWORD playerSpawn = EQ_GetPlayerSpawn();
        DWORD targetSpawn = EQ_GetTargetSpawn();

        if (playerSpawn != NULL && targetSpawn != NULL)
        {
            FLOAT meleeRange = EQ_get_melee_range(playerSpawn, targetSpawn);

            std::cout << "Melee Range: " << meleeRange << std::endl;
        }

        return;
    }

    // get spell name
    if (strncmp(command, "//getspellname ", 15) == 0 || strncmp(command, "//getsn ", 8) == 0 || strncmp(command, "//gsn ", 6) == 0)
    {
        char commandEx[128];

        int spellId = 0;

        int result = sscanf_s(command, "%s %d", commandEx, sizeof(commandEx), &spellId);
        if (result == 2)
        {
            std::string spellName = EQ_GetSpellNameById(spellId);

            std::cout << "Spell Name By ID: " << spellId << " == " << spellName << std::endl;
        }

        return;
    }

    // get target info
    if (strcmp(command, "//gettargetinfo") == 0 || strcmp(command, "//getti") == 0 || strcmp(command, "//gti") == 0)
    {
        DWORD targetSpawn = EQ_GetTargetSpawn();

        if (targetSpawn == NULL)
        {
            std::cout << "No target." << std::endl;
            return;
        }

        std::cout << "Target Info:" << std::endl;

        //

        char spawnName[0x40] = {0};
        memcpy(spawnName, (LPVOID)(targetSpawn + 0xA4), sizeof(spawnName));

        std::cout << "NAME:  " << spawnName;

        int spawnType = EQ_ReadMemory<BYTE>(targetSpawn + 0x125);
        if (spawnType != EQ_SPAWN_TYPE_PLAYER)
        {
            char spawnNameEx[0x40] = {0};
            memcpy(spawnNameEx, (LPVOID)(targetSpawn + 0xE4), sizeof(spawnNameEx));

            std::cout << " (" << spawnNameEx << ")";
        }

        std::cout << std::endl;

        //

        int spawnLevel = EQ_ReadMemory<BYTE>(targetSpawn + 0x315);

        std::cout << "LEVEL: " << spawnLevel << std::endl;

        //

        int spawnGuildId = EQ_ReadMemory<DWORD>(targetSpawn + 0x30C);

        const char* spawnGuildName = EQ_EQ_Guilds.GetGuildNameById(spawnGuildId);

        std::cout << "GUILD: " << spawnGuildName << " (" << spawnGuildId << ")" << std::endl;

        //

        int spawnRace = EQ_ReadMemory<DWORD>(targetSpawn + 0xE64);

        const char* spawnRaceDescription = EQ_CEverQuest->GetRaceDesc(spawnRace);

        std::cout << "RACE:  " << spawnRaceDescription << " (" << spawnRace << ")" << std::endl;

        //

        int spawnClass = EQ_ReadMemory<BYTE>(targetSpawn + 0xE68);

        const char* spawnClassDescription = EQ_CEverQuest->GetClassDesc(spawnClass);

        std::cout << "CLASS: " << spawnClassDescription << " (" << spawnClass << ")" << std::endl;

        //

        int spawnDeity = EQ_ReadMemory<DWORD>(targetSpawn + 0xD4D);

        const char* spawnDeityDescription = EQ_CEverQuest->GetDeityDesc(spawnDeity);

        std::cout << "DEITY: " << spawnDeityDescription << " (" << spawnDeity << ")" << std::endl;

        //

        const char* spawnBodyTypeDescription = EQ_CEverQuest->GetBodyTypeDesc(targetSpawn + 0x128);

        std::cout << "BANE:  " << spawnBodyTypeDescription << std::endl;

        return;
    }

    // bank or money
    if (strcmp(command, "//bank") == 0 || strcmp(command, "//money") == 0)
    {
        DWORD charInfo = EQ_ReadMemory<DWORD>(EQ_POINTER_CHAR_INFO);
        if (charInfo == NULL)
        {
            return;
        }

        DWORD platinum = EQ_ReadMemory<DWORD>(charInfo + 0xF5A4);
        DWORD gold     = EQ_ReadMemory<DWORD>(charInfo + 0xF5A8);
        DWORD silver   = EQ_ReadMemory<DWORD>(charInfo + 0xF5AC);
        DWORD copper   = EQ_ReadMemory<DWORD>(charInfo + 0xF5B0);

        std::cout << "You have " << platinum << "p " << gold << "g " << silver << "s " << copper << "c in the bank." << std::endl;
        return;
    }

    // print bank list
    if (strcmp(command, "//banklist") == 0 || strcmp(command, "//bl") == 0)
    {
        EQAPP_BankList_Print(NULL);
        return;
    }

    // search bank list
    if (strncmp(command, "//banklist ", 11) == 0 || strncmp(command, "//bl ", 5) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));
        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            EQAPP_BankList_Print(name);
        }

        return;
    }

    // print inventory list
    if (strcmp(command, "//inventorylist") == 0 || strcmp(command, "//invlist") == 0 || strcmp(command, "//il") == 0)
    {
        EQAPP_InventoryList_Print(NULL);
        return;
    }

    // search inventory list
    if (strncmp(command, "//inventorylist ", 16) == 0 || strncmp(command, "//invlist ", 10) == 0 || strncmp(command, "//il ", 5) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));
        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            EQAPP_InventoryList_Print(name);
        }

        return;
    }

    // print spawn list
    if (strcmp(command, "//spawnlist") == 0 || strcmp(command, "//sl") == 0)
    {
        EQAPP_SpawnList_Print(NULL);
        return;
    }

    // search spawn list
    if (strncmp(command, "//spawnlist ", 12) == 0 || strncmp(command, "//sl ", 5) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));
        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            EQAPP_SpawnList_Print(name);
        }

        return;
    }

    // toggle esp
    if (strcmp(command, "//esp") == 0)
    {
        EQ_ToggleBool(g_espIsEnabled);
        EQAPP_PrintBool("ESP", g_espIsEnabled);
        return;
    }

    // toggle esp spawn
    if (strcmp(command, "//espspawn") == 0 || strcmp(command, "//esps") == 0)
    {
        EQ_ToggleBool(g_espSpawnIsEnabled);
        EQAPP_PrintBool("ESP Spawn", g_espSpawnIsEnabled);
        return;
    }

    // toggle esp ground spawn
    if (strcmp(command, "//espgroundspawn") == 0 || strcmp(command, "//espg") == 0)
    {
        EQ_ToggleBool(g_espGroundSpawnIsEnabled);
        EQAPP_PrintBool("ESP Ground Spawn", g_espGroundSpawnIsEnabled);
        return;
    }

    // toggle esp door
    if (strcmp(command, "//espdoor") == 0 || strcmp(command, "//espd") == 0)
    {
        EQ_ToggleBool(g_espDoorIsEnabled);
        EQAPP_PrintBool("ESP Door", g_espDoorIsEnabled);
        return;
    }

    // toggle esp skeleton
    if (strcmp(command, "//espskeleton") == 0 || strcmp(command, "//espsk") == 0)
    {
        EQ_ToggleBool(g_espSkeletonIsEnabled);
        EQAPP_PrintBool("ESP Skeleton", g_espSkeletonIsEnabled);
        return;
    }

    // toggle esp zone object
    if (strcmp(command, "//espzoneobject") == 0 || strcmp(command, "//espzo") == 0)
    {
        EQ_ToggleBool(g_espZoneObjectIsEnabled);
        EQAPP_PrintBool("ESP Zone Object", g_espZoneObjectIsEnabled);
        return;
    }

    // toggle esp custom
    if (strcmp(command, "//espcustom") == 0 || strcmp(command, "//espc") == 0)
    {
        EQ_ToggleBool(g_espCustomIsEnabled);
        EQAPP_PrintBool("ESP Custom", g_espCustomIsEnabled);
        return;
    }

    // toggle esp waypoint
    if (strcmp(command, "//espwaypoint") == 0 || strcmp(command, "//espwp") == 0)
    {
        EQ_ToggleBool(g_espWaypointIsEnabled);
        EQAPP_PrintBool("ESP Waypoint", g_espWaypointIsEnabled);
        return;
    }

    // load esp custom
    if (strcmp(command, "//loadespcustom") == 0 || strcmp(command, "//loadec") == 0 || strcmp(command, "//lec") == 0)
    {
        EQAPP_ESP_Custom_Load();
        return;
    }

    // set esp spawn distance
    if (strncmp(command, "//setespspawndistance ", 22) == 0 || strncmp(command, "//setesps ", 10) == 0 || strncmp(command, "//setesp ", 9) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);
        if (result == 2)
        {
            g_espSpawnDistance = distance;

            std::cout << "ESP Spawn Distance: " << distance << std::endl;
        }

        return;
    }

    // set esp ground spawn distance
    if (strncmp(command, "//setespgroundspawndistance ", 28) == 0 || strncmp(command, "//setespg ", 10) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);
        if (result == 2)
        {
            g_espGroundSpawnDistance = distance;

            std::cout << "ESP Ground Spawn Distance: " << distance << std::endl;
        }

        return;
    }

    // set esp door distance
    if (strncmp(command, "//setespdoordistance ", 21) == 0 || strncmp(command, "//setespd ", 10) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);
        if (result == 2)
        {
            g_espDoorDistance = distance;

            std::cout << "ESP Door Distance: " << distance << std::endl;
        }

        return;
    }

    // set esp skeleton distance
    if (strncmp(command, "//setespskeletondistance ", 25) == 0 || strncmp(command, "//setespsk ", 11) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);
        if (result == 2)
        {
            g_espSkeletonDistance = distance;

            std::cout << "ESP Skeleton Distance: " << distance << std::endl;
        }

        return;
    }

    // set esp zone object distance
    if (strncmp(command, "//setespzoneobjectdistance ", 27) == 0 || strncmp(command, "//setespzo ", 11) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);
        if (result == 2)
        {
            g_espZoneObjectDistance = distance;

            std::cout << "ESP Zone Object Distance: " << distance << std::endl;
        }

        return;
    }

    // set esp custom distance
    if (strncmp(command, "//setespcustomdistance ", 23) == 0 || strncmp(command, "//setespc ", 10) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);
        if (result == 2)
        {
            g_espCustomDistance = distance;

            std::cout << "ESP Custom Distance: " << distance << std::endl;
        }

        return;
    }

    // set esp waypoint distance
    if (strncmp(command, "//setespwaypointdistance ", 25) == 0 || strncmp(command, "//setespwp ", 11) == 0)
    {
        char commandEx[128];

        float distance = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);
        if (result == 2)
        {
            g_espWaypointDistance = distance;

            std::cout << "ESP Waypoint Distance: " << distance << std::endl;
        }

        return;
    }

    // get esp custom location
    if (strcmp(command, "//getespcustomlocation") == 0 || strcmp(command, "//getecl") == 0 || strcmp(command, "//gecl") == 0)
    {
        DWORD spawnInfo = EQ_GetTargetSpawn();
        if (spawnInfo == NULL)
        {
            spawnInfo = EQ_GetPlayerSpawn();
            if (spawnInfo == NULL)
            {
                return;
            }
        }

        EQ_CopySpawnEspCustomLocationToClipboard(spawnInfo);

        std::cout << "Copying esp custom location to the clipboard..." << std::endl;

        return;
    }

    // toggle speed
    if (strcmp(command, "//speed") == 0)
    {
        EQ_ToggleBool(g_speedHackIsEnabled);
        EQAPP_PrintBool("Speed Hack", g_speedHackIsEnabled);
        return;
    }

    // set speed sow
    if (strcmp(command, "//setspeed sow") == 0)
    {
        g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_SPIRIT_OF_WOLF;
        std::cout << "Speed Modifier:  Spirit of Wolf" << std::endl;
        return;
    }

    // set speed run1
    if (strcmp(command, "//setspeed run1") == 0)
    {
        g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN1;
        std::cout << "Speed Modifier: AA Run 1" << std::endl;
        return;
    }

    // set speed run2
    if (strcmp(command, "//setspeed run2") == 0)
    {
        g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN2;
        std::cout << "Speed Modifier: AA Run 2" << std::endl;
        return;
    }

    // set speed run3
    if (strcmp(command, "//setspeed run3") == 0)
    {
        g_speedHackModifier = EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN3;
        std::cout << "Speed Modifier: AA Run 3" << std::endl;
        return;
    }

    // set speed
    if (strncmp(command, "//setspeed ", 11) == 0 || strncmp(command, "//sets ", 7) == 0 || strncmp(command, "//ss ", 5) == 0)
    {
        char commandEx[128];

        float modifier = 0.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &modifier);
        if (result == 2)
        {
            g_speedHackModifier = modifier;

            std::cout << "Speed Modifier: " << g_speedHackModifier << std::endl;
        }

        return;
    }

    // toggle locator
    if (strcmp(command, "//locator") == 0 || strcmp(command, "//loc") == 0)
    {
        EQ_ToggleBool(g_espLocatorIsEnabled);
        EQAPP_PrintBool("Locator", g_espLocatorIsEnabled);
        return;
    }

    // set locator
    if (strncmp(command, "//setlocator ", 13) == 0 || strncmp(command, "//setloc ", 9) == 0)
    {
        char commandEx[128];

        float y = 0.0f;
        float x = 0.0f;
        float z = 0.0f;

        int result = sscanf_s(command, "%s %f %f %f", commandEx, sizeof(commandEx), &y, &x, &z);
        if (result == 4)
        {
            g_espLocatorY = y;
            g_espLocatorX = x;
            g_espLocatorZ = z;

            std::cout << "Locator: " << y << ", " << x << ", " << z << std::endl;

            g_espLocatorIsEnabled = true;
        }

        return;
    }

    // toggle find
    if (strcmp(command, "//find") == 0)
    {
        EQ_ToggleBool(g_espFindIsEnabled);
        EQAPP_PrintBool("Find", g_espFindIsEnabled);
        return;
    }

    // set find
    if (strncmp(command, "//setfind ", 10) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));
        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            g_espFindSpawnName = name;

            std::cout << "Find: " << name << std::endl;

            g_espFindIsEnabled = true;
        }

        return;
    }

    // toggle line to target
    if (strcmp(command, "//linetotarget") == 0 || strcmp(command, "//ltt") == 0)
    {
        EQ_ToggleBool(g_lineToTargetIsEnabled);
        EQAPP_PrintBool("Line to Target", g_lineToTargetIsEnabled);
        return;
    }

    // memory list
    if (strcmp(command, "//memorylist") == 0 || strcmp(command, "//memlist") == 0)
    {
        std::cout << "Memory List:" << std::endl;

        for (auto& memory : g_memoryList)
        {
            std::cout << memory.index << " : " << memory.isEnabled << " : " << memory.name << " (" << memory.description << ")" << std::endl;
        }

        return;
    }

    // toggle memory
    if (strncmp(command, "//memory ", 9) == 0 || strncmp(command, "//mem ", 6) == 0)
    {
        char commandEx[128];

        unsigned int index = 0;

        int result = sscanf_s(command, "%s %d", commandEx, sizeof(commandEx), &index);
        if (result == 2)
        {
            EQAPP_Memory_ToggleByIndex(index);
        }

        return;
    }

    // always attack
    if (strcmp(command, "//alwaysattack") == 0 || strcmp(command, "//aa") == 0)
    {
        EQ_ToggleBool(g_alwaysAttackIsEnabled);
        EQAPP_PrintBool("Always Attack", g_alwaysAttackIsEnabled);
        return;
    }

    // toggle combat hotbutton
    if (strcmp(command, "//combathotbutton") == 0 || strcmp(command, "//chb") == 0)
    {
        EQ_ToggleBool(g_combatHotbuttonIsEnabled);
        EQAPP_PrintBool("Combat Hotbutton", g_combatHotbuttonIsEnabled);
        return;
    }

    // set combat hotbutton
    if (strncmp(command, "//setcombathotbutton ", 21) == 0 || strncmp(command, "//setchb ", 9) == 0 || strncmp(command, "//schb ", 7) == 0)
    {
        char commandEx[128];

        unsigned int buttonIndex = 0;
        unsigned int delay       = 1;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &buttonIndex, &delay);
        if (result == 3)
        {
            // TODO make function for this

            if (buttonIndex < 1 || buttonIndex > 10)
            {
                std::cout << "No hotbutton exists at specified index: " << buttonIndex << std::endl;
                return;
            }

            g_combatHotbuttonIndex = buttonIndex - 1;

            g_combatHotbuttonTimerDelay = delay * 1000;

            std::cout << "Combat Hotbutton: " << buttonIndex << " (" << delay << " second(s))" << std::endl;

            g_combatHotbuttonIsEnabled = true;
        }

        return;
    }

    // toggle always hotbutton
    if (strcmp(command, "//alwayshotbutton") == 0 || strcmp(command, "//ahb") == 0)
    {
        EQ_ToggleBool(g_alwaysHotbuttonIsEnabled);
        EQAPP_PrintBool("Always Hotbutton", g_alwaysHotbuttonIsEnabled);
        return;
    }

    // set always hotbutton
    if (strncmp(command, "//setalwayshotbutton ", 21) == 0 || strncmp(command, "//setahb ", 9) == 0 || strncmp(command, "//sahb ", 7) == 0)
    {
        char commandEx[128];

        unsigned int buttonIndex = 0;
        unsigned int delay       = 1;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &buttonIndex, &delay);
        if (result == 3)
        {
            // TODO: make function for this

            if (buttonIndex < 1 || buttonIndex > 10)
            {
                std::cout << "No hotbutton exists at specified index: " << buttonIndex << std::endl;
                return;
            }

            g_alwaysHotbuttonIndex = buttonIndex - 1;

            g_alwaysHotbuttonTimerDelay = delay * 1000;

            std::cout << "Always Hotbutton: " << buttonIndex << " (" << delay << " second(s))" << std::endl;

            g_alwaysHotbuttonIsEnabled = true;
        }

        return;
    }

    // set collision radius
    if (strncmp(command, "//setcollisionradius ", 21) == 0 || strncmp(command, "//setcr ", 8) == 0 || strncmp(command, "//scr ", 6) == 0)
    {
        char commandEx[128];

        float radius = 1.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &radius);
        if (result == 2)
        {
            if (radius < 0.1f)
            {
                std::cout << "Specified radius too low: " << radius << std::endl;
                return;
            }

            DWORD playerSpawn = EQ_GetPlayerSpawn();
            if (playerSpawn == NULL)
            {
                return;
            }

            EQ_SetSpawnCollisionRadius(playerSpawn, radius);

            std::cout << "Collision Radius: " << radius << std::endl;
        }

        return;
    }

    // toggle draw distance
    if (strcmp(command, "//drawdistance") == 0 || strcmp(command, "//dd") == 0)
    {
        EQ_ToggleBool(g_drawDistanceIsEnabled);
        EQAPP_PrintBool("Draw Distance", g_drawDistanceIsEnabled);
        return;
    }

    // set draw distance
    if (strncmp(command, "//setdrawdistance ", 18) == 0 || strncmp(command, "//setdd ", 8) == 0)
    {
        char commandEx[128];

        float distance = EQ_DRAW_DISTANCE_DEFAULT;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &distance);
        if (result == 2)
        {
            // TODO: make function

            if (distance < EQ_DRAW_DISTANCE_MINIMUM)
            {
                std::cout << "Specified draw distance too low: " << distance << std::endl;
                return;
            }

            g_drawDistance = distance;

            g_drawDistanceIsEnabled = true;

            std::cout << "Draw Distance: " << distance << std::endl;
        }

        return;
    }

    // set field of view
    if (strncmp(command, "//setfieldofview ", 17) == 0 || strncmp(command, "//setfov ", 9) == 0)
    {
        char commandEx[128];

        float fov = EQ_FIELD_OF_VIEW_DEFAULT;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &fov);
        if (result == 2)
        {
            if (fov <= 0.0f)
            {
                std::cout << "Specified field of view too low: " << fov << std::endl;
                return;
            }

            EQ_SetFieldOfView(fov);

            std::cout << "Field of View: " << fov << std::endl;
        }

        return;
    }

    // toggle target beep
    if (strcmp(command, "//targetbeep") == 0 || strcmp(command, "//tbeep") == 0)
    {
        EQ_ToggleBool(g_targetBeepIsEnabled);
        EQAPP_PrintBool("Target Beep", g_targetBeepIsEnabled);
        return;
    }

    // set target beep
    if (strncmp(command, "//settargetbeep ", 16) == 0 || strncmp(command, "//settbeep ", 11) == 0 || strncmp(command, "//stb ", 6) == 0)
    {
        char commandEx[128];

        char name[1024];

        unsigned int delay = 1;

        int result = sscanf_s(command, "%s %s %d", commandEx, sizeof(commandEx), name, sizeof(name), &delay);
        if (result == 3)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            g_targetBeepName = name;

            g_targetBeepTimerDelay = delay * 1000;

            std::cout << "Target Beep: " << name << " (" << delay << " second(s))" << std::endl;

            g_targetBeepIsEnabled = true;
        }

        return;
    }

    // toggle spawn beep
    if (strcmp(command, "//spawnbeep") == 0 || strcmp(command, "//sbeep") == 0)
    {
        EQ_ToggleBool(g_spawnBeepIsEnabled);
        EQAPP_PrintBool("Spawn Beep", g_spawnBeepIsEnabled);
        return;
    }

    // set spawn beep
    if (strncmp(command, "//setspawnbeep ", 15) == 0 || strncmp(command, "//setsbeep ", 11) == 0 || strncmp(command, "//ssb ", 6) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));
        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            g_spawnBeepName = name;

            std::cout << "Spawn Beep: " << name << std::endl;

            g_spawnBeepIsEnabled = true;
        }

        return;
    }

    // toggle change height
    if (strcmp(command, "//changeheight") == 0)
    {
        EQ_ToggleBool(g_changeHeightIsEnabled);
        EQAPP_PrintBool("Change Height", g_changeHeightIsEnabled);
        return;
    }

    // set height
    if (strncmp(command, "//setheight ", 12) == 0 || strncmp(command, "//seth ", 7) == 0 || strncmp(command, "//sh ", 5) == 0)
    {
        DWORD targetSpawn = EQ_GetTargetSpawn();
        if (targetSpawn == NULL)
        {
            std::cout << "Target not found!" << std::endl;
            return;
        }

        char commandEx[128];

        float height = g_changeHeightMaximum;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &height);
        if (result == 2)
        {
            EQ_SetSpawnHeight(targetSpawn, height);

            std::cout << "Set Target Height: " << height << std::endl;

            g_targetBeepIsEnabled = true;
        }

        return;
    }

    // toggle map labels
    if (strcmp(command, "//maplabels") == 0 || strcmp(command, "//ml") == 0)
    {
        EQ_ToggleBool(g_mapLabelsIsEnabled);
        EQAPP_PrintBool("Map Labels", g_mapLabelsIsEnabled);

        if (g_mapLabelsIsEnabled == false)
        {
            EQAPP_MapLabels_Remove();
        }

        return;
    }

    // add map labels
    if (strcmp(command, "//addmaplabels") == 0 || strcmp(command, "//addml") == 0 || strcmp(command, "//aml") == 0)
    {
        EQAPP_MapLabels_Execute();

        std::cout << "Adding map labels..." << std::endl;

        return;
    }

    // remove map labels
    if (strcmp(command, "//removemaplabels") == 0 || strcmp(command, "//removeml") == 0 || strcmp(command, "//rml") == 0)
    {
        EQAPP_MapLabels_Remove();

        std::cout << "Removing map labels..." << std::endl;

        return;
    }

    // get map labels
    if (strcmp(command, "//getmaplabels") == 0 || strcmp(command, "//getml") == 0 || strcmp(command, "//gml") == 0)
    {
        std::cout << "Map Labels:" << std::endl;

        DWORD mapViewWnd = EQ_ReadMemory<DWORD>(EQ_POINTER_CMapViewWnd);
        if (mapViewWnd == NULL)
        {
            return;
        }

        struct _EQMAPLABEL* mapLabel = EQ_ReadMemory<struct _EQMAPLABEL*>(mapViewWnd + 0x2D0);
        if (mapLabel == NULL)
        {
            return;
        }

        while (mapLabel != NULL)
        {
            std::cout << mapLabel->Label << std::endl;

            mapLabel = mapLabel->Next;
        }

        return;
    }

    // toggle filter map labels
    if (strcmp(command, "//filtermaplabels") == 0 || strcmp(command, "//filterml") == 0 || strcmp(command, "//fml") == 0)
    {
        EQ_ToggleBool(g_mapLabelsFilterIsEnabled);
        EQAPP_PrintBool("Map Labels Filter", g_mapLabelsFilterIsEnabled);
        return;
    }

    // filter map labels
    if (strncmp(command, "//filtermaplabels ", 18) == 0 || strncmp(command, "//filterml ", 11) == 0 || strncmp(command, "//fml ", 6) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));
        if (result == 2)
        {
            EQ_StringReplaceUnderscoresWithSpaces(name);

            g_mapLabelsFilterName = name;

            std::cout << "Map Labels Filter: " << name << std::endl;

            g_mapLabelsFilterIsEnabled = true;
        }

        return;
    }

    // get zone info
    if (strcmp(command, "//getzoneinfo") == 0 || strcmp(command, "//getzone") == 0 || strcmp(command, "//getzi") == 0 || strcmp(command, "//gzi") == 0)
    {
        DWORD zoneId = EQ_GetZoneId();

        std::string zoneLongName = EQ_GetZoneLongName();
        if (zoneLongName.size() == 0)
        {
            std::cout << __FUNCTION__ << ": //getzoneinfo: zone long name is null" << std::endl;
            return;
        }

        std::string zoneShortName = EQ_GetZoneShortName();
        if (zoneShortName.size() == 0)
        {
            std::cout << __FUNCTION__ << ": //getzoneinfo: zone short name is null" << std::endl;
            return;
        }

        std::cout << "Zone Info: " << zoneLongName << " (" << zoneShortName << ") " << "(ID: " << zoneId << ")" << std::endl;

        return;
    }

    // load memory
    if (strcmp(command, "//loadmemory") == 0 || strcmp(command, "//loadm") == 0 || strcmp(command, "//lm") == 0)
    {
        EQAPP_Memory_Unload();
        EQAPP_Memory_Load();
        return;
    }

    // toggle sounds
    if (strcmp(command, "//sounds") == 0)
    {
        EQ_ToggleBool(g_soundsIsEnabled);
        EQAPP_PrintBool("Sounds", g_soundsIsEnabled);
        return;
    }

    // load sounds
    if (strcmp(command, "//loadsounds") == 0 || strcmp(command, "//loads") == 0 || strcmp(command, "//ls") == 0)
    {
        EQAPP_Sounds_Load();
        return;
    }

    // toggle named spawns
    if (strcmp(command, "//namedspawns") == 0 || strcmp(command, "//ns") == 0)
    {
        EQ_ToggleBool(g_namedSpawnsIsEnabled);
        EQAPP_PrintBool("Named Spawns" , g_namedSpawnsIsEnabled);
        return;
    }

    // load named spawns
    if (strcmp(command, "//loadnamedspawns") == 0 || strcmp(command, "//loadns") == 0 || strcmp(command, "//lns") == 0)
    {
        EQAPP_NamedSpawns_Load();
        return;
    }

    // get named spawns
    if (strcmp(command, "//getnamedspawns") == 0 || strcmp(command, "//getns") == 0 || strcmp(command, "//gns") == 0)
    {
        std::cout << "Named Spawns:" << std::endl;

        size_t index = 1;

        for (auto& namedSpawn : g_namedSpawnsList)
        {
            std::cout << index << ": " << namedSpawn << std::endl;

            index++;
        }

        return;
    }

    // write character file
    if (strcmp(command, "//writecharacterfile") == 0 || strcmp(command, "//writechar") == 0 || strcmp(command, "//writecf") == 0 || strcmp(command, "//wcf") == 0)
    {
        EQAPP_CharacterFile_Write();
        return;
    }

    // load text overlay chat text
    if (strcmp(command, "//loadtextoverlaychattext") == 0 || strcmp(command, "//loadtoct") == 0 || strcmp(command, "//ltoct") == 0)
    {
        EQAPP_TextOverlayChatText_Load();
        return;
    }

    // load no beep
    if (strcmp(command, "//loadnobeep") == 0 || strcmp(command, "//loadnb") == 0 || strcmp(command, "//lnb") == 0)
    {
        EQAPP_NoBeep_Load();
        return;
    }

    // load zone short names
    if (strcmp(command, "//loadzoneshortnames") == 0 || strcmp(command, "//loadzsn") == 0 || strcmp(command, "//lzsn") == 0)
    {
        EQAPP_ZoneShortNames_Load();
        return;
    }

    // toggle hide corpse looted
    if (strcmp(command, "//hidecorpselooted") == 0 || strcmp(command, "//hcl") == 0)
    {
        EQ_ToggleBool(g_hideCorpseLootedIsEnabled);
        EQAPP_PrintBool("Hide Corpse Looted", g_hideCorpseLootedIsEnabled);
        return;
    }

    // open all doors
    if (strcmp(command, "//openalldoors") == 0 || strcmp(command, "//opendoors") == 0 || strcmp(command, "//oad") == 0)
    {
        EQAPP_Doors_OpenAll(true);

        return;
    }

    // close all doors
    if (strcmp(command, "//closealldoors") == 0 || strcmp(command, "//closedoors") == 0 || strcmp(command, "//cad") == 0)
    {
        EQAPP_Doors_OpenAll(false);

        return;
    }

    // census
    if (strcmp(command, "//census") == 0)
    {
        EQAPP_Census_Toggle();
        return;
    }

    // toggle free camera
    if (strcmp(command, "//freecamera") == 0 || strcmp(command, "//freecam") == 0 || strcmp(command, "//fc") == 0)
    {
        EQAPP_FreeCamera_Toggle();
        return;
    }

    // set view actor by name
    if (strncmp(command, "//setviewactor ", 15) == 0 || strncmp(command, "//setview ", 10) == 0 || strncmp(command, "//setva ", 8) == 0 || strncmp(command, "//sva ", 6) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));
        if (result == 2)
        {
            EQ_CDisplay->SetViewActorByName(name);

            std::cout << "Set View Actor: " << name << std::endl;
        }

        return;
    }

    // set view actor to target
    if (strcmp(command, "//setviewtarget") == 0 || strcmp(command, "//setvt") == 0 || strcmp(command, "//svt") == 0)
    {
        DWORD targetSpawn = EQ_GetTargetSpawn();
        if (targetSpawn == NULL)
        {
            std::cout << "Target not found!" << std::endl;
            return;
        }

        EQ_SetViewActorBySpawn(targetSpawn);

        std::cout << "Set View Actor to Target." << std::endl;

        return;
    }

    // reset view actor
    if (strcmp(command, "//resetviewactor") == 0 || strcmp(command, "//resetview") == 0 || strcmp(command, "//resetva") == 0 || strcmp(command, "//rva") == 0)
    {
        EQ_ResetViewActor();

        std::cout << "Reset View Actor." << std::endl;

        return;
    }

    // map locations
    if (strcmp(command, "//maplocations") == 0 || strcmp(command, "//maplocs") == 0)
    {
        EQAPP_MapLocations_WriteToFile();
        return;
    }

    // get map location
    if (strcmp(command, "//getmaplocation") == 0 || strcmp(command, "//getmaploc") == 0)
    {
        DWORD spawnInfo = NULL;

        spawnInfo = EQ_GetTargetSpawn();
        if (spawnInfo == NULL)
        {
            spawnInfo = EQ_GetPlayerSpawn();
            if (spawnInfo == NULL)
            {
                return;
            }
        }

        EQ_CopySpawnMapLocationToClipboard(spawnInfo);

        std::cout << "Copying map location to the clipboard..." << std::endl;

        return;
    }

    // open map file
    if (strcmp(command, "//openmapfile") == 0 || strcmp(command, "//omf") == 0)
    {
        std::string zoneShortName = EQ_GetZoneShortName();
        if (zoneShortName.size() == 0)
        {
            std::cout << __FUNCTION__ << ": //openmapfile: zone short name is null" << std::endl;
            return;
        }

        std::stringstream filePath;
        filePath << "maps\\" << zoneShortName << ".txt";

        ShellExecuteA(0, "open", filePath.str().c_str(), 0, 0, SW_SHOW);

        std::cout << "Opening map file: " << filePath.str() << std::endl;

        for (size_t i = 1; i < 4; i++)
        {
            std::stringstream filePath;
            filePath << "maps\\" << zoneShortName << "_" << i << ".txt";

            ShellExecuteA(0, "open", filePath.str().c_str(), 0, 0, SW_SHOW);

            std::cout << "Opening map file: " << filePath.str() << std::endl;
        }

        return;
    }

    // waypoint add
    if (strcmp(command, "//waypointadd") == 0 || strcmp(command, "//wpa") == 0)
    {
        EQAPP_Waypoint_Add();
        return;
    }

    // waypoint remove
    if (strncmp(command, "//waypointremove ", 17) == 0 || strncmp(command, "//wpr ", 6) == 0)
    {
        char commandEx[128];

        unsigned int index;

        int result = sscanf_s(command, "%s %d", commandEx, sizeof(commandEx), &index);
        if (result == 2)
        {
            EQAPP_Waypoint_Remove(index);
        }

        return;
    }

    // waypoint connect
    if (strncmp(command, "//waypointconnect ", 18) == 0 || strncmp(command, "//wpc ", 6) == 0)
    {
        char commandEx[128];

        unsigned int fromIndex;
        unsigned int toIndex;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &fromIndex, &toIndex);
        if (result == 3)
        {
            EQAPP_Waypoint_Connect(fromIndex, toIndex);
        }

        return;
    }

    // waypoint disconnect
    if (strncmp(command, "//waypointdisconnect ", 21) == 0 || strncmp(command, "//wpd ", 6) == 0)
    {
        char commandEx[128];

        unsigned int fromIndex;
        unsigned int toIndex;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &fromIndex, &toIndex);
        if (result == 3)
        {
            EQAPP_Waypoint_Disconnect(fromIndex, toIndex);
        }

        return;
    }

    // waypoint list print
    if (strcmp(command, "//waypointlist") == 0 || strcmp(command, "//wpl") == 0 || strcmp(command, "//waypointlistprint") == 0 || strcmp(command, "//wplp") == 0)
    {
        EQAPP_WaypointList_Print();
        return;
    }

    // waypoint list clear
    if (strcmp(command, "//waypointlistclear") == 0 || strcmp(command, "//wplc") == 0)
    {
        EQAPP_WaypointList_Clear();
        return;
    }

    // waypoint list load
    if (strcmp(command, "//waypointlistload") == 0 || strcmp(command, "//wpll") == 0)
    {
        EQAPP_WaypointList_Load();
        return;
    }

    // waypoint list save
    if (strcmp(command, "//waypointlistsave") == 0 || strcmp(command, "//wpls") == 0)
    {
        EQAPP_WaypointList_Save();
        return;
    }

    // waypoint list get path
    if (strncmp(command, "//waypointlistgetpath ", 22) == 0 || strncmp(command, "//wplgp ", 8) == 0)
    {
        char commandEx[128];

        unsigned int fromIndex;
        unsigned int toIndex;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &fromIndex, &toIndex);

        if (result == 3)
        {
            EQAPP_WaypointList_GetPath(fromIndex, toIndex);
        }

        return;
    }
}

#endif // EQAPP_INTERPRETCOMMAND_H
