#ifndef EQAPP_INTERPRETCOMMAND_H
#define EQAPP_INTERPRETCOMMAND_H

// TODO: update the list
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
        std::cout << "Command List:" << std::endl;

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

    // start of in game check
    if (EQ_IsInGame() == false)
    {
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

    // hud
    if (strcmp(command, "//hud") == 0)
    {
        EQ_ToggleBool(g_hudIsEnabled);
        EQAPP_PrintBool("HUD", g_hudIsEnabled);
        return;
    }

    // print loot debug information
    if (strcmp(command, "//lootdebug") == 0)
    {
        EQAPP_LootDebugInformation_Print();
        return;
    }

    // loot item by name
    if (strncmp(command, "//loot ", 7) == 0)
    {
        char commandEx[128];

        char itemName[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), itemName, sizeof(itemName));
        if (result == 2)
        {
            EQ_String_ReplaceUnderscoresWithSpaces(itemName);

            bool lootResult = EQ_LootItemByName(itemName);

            std::cout << "Loot Item: " << itemName << " | Result: " << std::boolalpha << lootResult << std::noboolalpha << std::endl;
        }
    }

    // toggle auto loot
    if (strcmp(command, "//autoloot") == 0 || strcmp(command, "//al") == 0)
    {
        EQAPP_AutoLoot_Toggle();
        return;
    }

    // print auto loot list
    if (strcmp(command, "//autolootlist") == 0 || strcmp(command, "//all") == 0)
    {
        EQAPP_AutoLootList_Print();
        return;
    }

    // reset auto loot
    if (strcmp(command, "//autolootreset") == 0)
    {
        EQAPP_AutoLoot_Reset();
        return;
    }

    // add to auto loot list
    if (strncmp(command, "//autolootadd ", 14) == 0 || strncmp(command, "//ala ", 6) == 0)
    {
        char commandEx[128];

        char itemName[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), itemName, sizeof(itemName));
        if (result == 2)
        {
            EQ_String_ReplaceUnderscoresWithSpaces(itemName);

            EQAPP_AutoLootList_Add(itemName);

            g_autoLootIsEnabled = true;
        }
    }

    // remove from auto loot list
    if (strncmp(command, "//autolootremove ", 17) == 0 || strncmp(command, "//alr ", 6) == 0)
    {
        char commandEx[128];

        char itemName[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), itemName, sizeof(itemName));
        if (result == 2)
        {
            EQ_String_ReplaceUnderscoresWithSpaces(itemName);

            EQAPP_AutoLootList_Remove(itemName);
        }
    }

    // print cast ray to target
    if (strcmp(command, "//castray") == 0)
    {
        EQAPP_CastRayToTarget_Print();
        return;
    }

    // print melee range to target
    if (strcmp(command, "//getmeleerange") == 0)
    {
        EQAPP_MeleeRangeToTarget_Print();
        return;
    }

    // get spell name
    if (strncmp(command, "//getspellname ", 15) == 0)
    {
        char commandEx[128];

        unsigned int spellId = 0;

        int result = sscanf_s(command, "%s %d", commandEx, sizeof(commandEx), &spellId);
        if (result == 2)
        {
            std::string spellName = EQ_GetSpellNameById(spellId);

            std::cout << "Spell Name by ID: " << spellId << " == " << spellName << std::endl;
        }

        return;
    }

    // print target info
    if (strcmp(command, "//targetinfo") == 0)
    {
        DWORD targetSpawn = EQ_GetTargetSpawn();
        EQAPP_SpawnInformation_Print(targetSpawn);
        return;
    }

    // print bank currency
    if (strcmp(command, "//bank") == 0)
    {
        EQAPP_BankCurrency_Print();
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
            EQ_String_ReplaceUnderscoresWithSpaces(name);

            EQAPP_BankList_Print(name);
        }

        return;
    }

    // print inventory list
    if (strcmp(command, "//inventorylist") == 0 || strcmp(command, "//il") == 0)
    {
        EQAPP_InventoryList_Print(NULL);
        return;
    }

    // search inventory list
    if (strncmp(command, "//inventorylist ", 16) == 0 || strncmp(command, "//il ", 5) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));
        if (result == 2)
        {
            EQ_String_ReplaceUnderscoresWithSpaces(name);

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
            EQ_String_ReplaceUnderscoresWithSpaces(name);

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
    if (strcmp(command, "//espgroundspawn") == 0 || strcmp(command, "//espgs") == 0)
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
    if (strcmp(command, "//loadespcustom") == 0 || strcmp(command, "//loadespc") == 0)
    {
        EQAPP_ESP_Custom_Load();
        return;
    }

    // set esp spawn distance
    if (strncmp(command, "//setespspawndistance ", 22) == 0 || strncmp(command, "//setesps ", 10) == 0)
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
    if (strncmp(command, "//setespgroundspawndistance ", 28) == 0 || strncmp(command, "//setespgs ", 11) == 0)
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

    // copy custom esp location to the clipboard
    if (strcmp(command, "//getespcustomlocation") == 0 || strcmp(command, "//getecl") == 0)
    {
        // try target first, then player
        DWORD spawnInfo = EQ_GetTargetSpawn();
        if (spawnInfo == NULL)
        {
            spawnInfo = EQ_GetPlayerSpawn();
            if (spawnInfo == NULL)
            {
                std::cout << "Player or Target not found!" << std::endl;
                return;
            }
        }

        EQ_CopySpawnEspCustomLocationToClipboard(spawnInfo);
        std::cout << "Copying custom ESP location to the clipboard..." << std::endl;
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
        std::cout << "Speed Modifier: Spirit of Wolf" << std::endl;
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
    if (strncmp(command, "//setspeed ", 11) == 0 || strncmp(command, "//ss ", 5) == 0)
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

    // get locator
    if (strcmp(command, "//getlocator") == 0 || strcmp(command, "//getloc") == 0)
    {
        EQAPP_ESP_Locator_Print();
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

            EQAPP_ESP_Locator_Print();

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

    // get find
    if (strcmp(command, "//getfind") == 0)
    {
        EQAPP_ESP_Find_Print();
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
            EQ_String_ReplaceUnderscoresWithSpaces(name);

            g_espFindSpawnName = name;

            EQAPP_ESP_Find_Print();

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

    // print memory list
    if (strcmp(command, "//memorylist") == 0 || strcmp(command, "//ml") == 0)
    {
        EQAPP_MemoryList_Print();
        return;
    }

    // toggle memory by index
    if (strncmp(command, "//memory ", 9) == 0)
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

    // toggle always attack
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
    if (strncmp(command, "//setcombathotbutton ", 21) == 0 || strncmp(command, "//setchb ", 9) == 0)
    {
        char commandEx[128];

        unsigned int buttonNumber        = 0;
        unsigned int timerDelayinSeconds = 1;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &buttonNumber, &timerDelayinSeconds);
        if (result == 3)
        {
            EQAPP_CombatHotbutton_Set(buttonNumber, timerDelayinSeconds);

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
    if (strncmp(command, "//setalwayshotbutton ", 21) == 0 || strncmp(command, "//setahb ", 9) == 0)
    {
        char commandEx[128];

        unsigned int buttonNumber        = 0;
        unsigned int timerDelayinSeconds = 1;

        int result = sscanf_s(command, "%s %d %d", commandEx, sizeof(commandEx), &buttonNumber, &timerDelayinSeconds);
        if (result == 3)
        {
            EQAPP_AlwaysHotbutton_Set(buttonNumber, timerDelayinSeconds);

            g_alwaysHotbuttonIsEnabled = true;
        }

        return;
    }

    // set collision radius
    if (strncmp(command, "//setcollisionradius ", 21) == 0 || strncmp(command, "//setcoll ", 10) == 0)
    {
        char commandEx[128];

        float radius = 1.0f;

        int result = sscanf_s(command, "%s %f", commandEx, sizeof(commandEx), &radius);
        if (result == 2)
        {
            // try target first, then player
            DWORD spawnInfo = EQ_GetTargetSpawn();
            if (spawnInfo == NULL)
            {
                spawnInfo = EQ_GetPlayerSpawn();
                if (spawnInfo == NULL)
                {
                    std::cout << "Player or Target not found!" << std::endl;
                    return;
                }
            }

            if (radius < 0.1f)
            {
                std::cout << "Specified collision radius too low: " << radius << std::endl;
                return;
            }

            EQ_SetSpawnCollisionRadius(spawnInfo, radius);

            std::cout << "Set Player or Target Collision Radius: " << radius << std::endl;
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

    // get draw distance
    if (strcmp(command, "//getdrawdistance") == 0 || strcmp(command, "//getdd") == 0)
    {
        EQAPP_DrawDistance_Print();
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
            if (distance < EQ_DRAW_DISTANCE_MINIMUM)
            {
                std::cout << "Specified draw distance too low: " << distance << std::endl;
                return;
            }

            g_drawDistance = distance;

            EQAPP_DrawDistance_Print();

            g_drawDistanceIsEnabled = true;
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

    // print target beep
    if (strcmp(command, "//gettargetbeep") == 0 || strcmp(command, "//gettbeep") == 0 || strcmp(command, "//targetbeepprint") == 0)
    {
        EQAPP_TargetBeep_Print();
        return;
    }

    // set target beep
    if (strncmp(command, "//settargetbeep ", 16) == 0 || strncmp(command, "//settbeep ", 11) == 0 || strncmp(command, "//stb ", 6) == 0)
    {
        char commandEx[128];

        char name[1024];

        unsigned int timerDelayInSeconds = 1;

        int result = sscanf_s(command, "%s %s %d", commandEx, sizeof(commandEx), name, sizeof(name), &timerDelayInSeconds);
        if (result == 3)
        {
            EQ_String_ReplaceUnderscoresWithSpaces(name);

            EQAPP_TargetBeep_Set(name, timerDelayInSeconds);

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
            EQ_String_ReplaceUnderscoresWithSpaces(name);

            EQAPP_SpawnBeep_Set(name);

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
    if (strncmp(command, "//setheight ", 12) == 0)
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
        }

        return;
    }

    // toggle map labels
    if (strcmp(command, "//maplabels") == 0)
    {
        EQAPP_MapLabels_Toggle();
        return;
    }

    // add map labels
    if (strcmp(command, "//addmaplabels") == 0)
    {
        EQAPP_MapLabels_Execute();
        std::cout << "Adding map labels..." << std::endl;
        return;
    }

    // remove map labels
    if (strcmp(command, "//removemaplabels") == 0)
    {
        EQAPP_MapLabels_Remove();
        std::cout << "Removing map labels..." << std::endl;
        return;
    }

    // print map labels
    if (strcmp(command, "//getmaplabels") == 0)
    {
        EQAPP_MapLabels_Print();
        return;
    }

    // toggle filter map labels
    if (strcmp(command, "//filtermaplabels") == 0)
    {
        EQ_ToggleBool(g_mapLabelsFilterIsEnabled);
        EQAPP_PrintBool("Map Labels Filter", g_mapLabelsFilterIsEnabled);
        return;
    }

    // get filter map labels
    if (strcmp(command, "//getfiltermaplabels") == 0)
    {
        EQAPP_MapLabels_Filter_Print();
        return;
    }

    // set filter map labels
    if (strncmp(command, "//setfiltermaplabels ", 21) == 0)
    {
        char commandEx[128];

        char name[1024];

        int result = sscanf_s(command, "%s %s", commandEx, sizeof(commandEx), name, sizeof(name));
        if (result == 2)
        {
            EQ_String_ReplaceUnderscoresWithSpaces(name);

            g_mapLabelsFilterName = name;

            EQAPP_MapLabels_Filter_Print();

            g_mapLabelsFilterIsEnabled = true;
        }

        return;
    }

    // get zone info
    if (strcmp(command, "//getzoneinfo") == 0 || strcmp(command, "//getzi"))
    {
        EQAPP_ZoneInformation_Print();
        return;
    }

    // load memory
    if (strcmp(command, "//loadmemory") == 0)
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
    if (strcmp(command, "//loadsounds") == 0)
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
    if (strcmp(command, "//loadnamedspawns") == 0 || strcmp(command, "//loadns") == 0)
    {
        EQAPP_NamedSpawns_Load();
        return;
    }

    // print named spawns list
    if (strcmp(command, "//namedspawnslist") == 0 || strcmp(command, "//nsl") == 0 || strcmp(command, "//getnamedspawns") == 0)
    {
        EQAPP_NamedSpawnsList_Print();
        return;
    }

    // write character file
    if (strcmp(command, "//writecharacterfile") == 0 || strcmp(command, "//writechar"))
    {
        EQAPP_CharacterFile_Write();
        return;
    }

    // load text overlay chat text
    if (strcmp(command, "//loadtextoverlaychattext") == 0 || strcmp(command, "//loadtoct") == 0)
    {
        EQAPP_TextOverlayChatText_Load();
        return;
    }

    // load no beep
    if (strcmp(command, "//loadnobeep") == 0 || strcmp(command, "//loadnb") == 0)
    {
        EQAPP_NoBeep_Load();
        return;
    }

    // load zone short names
    if (strcmp(command, "//loadzoneshortnames") == 0 || strcmp(command, "//loadzsn") == 0)
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
    if (strcmp(command, "//openalldoors") == 0 || strcmp(command, "//opendoors") == 0)
    {
        EQAPP_Doors_OpenAll(true);

        return;
    }

    // close all doors
    if (strcmp(command, "//closealldoors") == 0 || strcmp(command, "//closedoors") == 0)
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
    if (strncmp(command, "//setviewactor ", 15) == 0 || strncmp(command, "//setva ", 8) == 0)
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
    if (strcmp(command, "//setviewactortarget") == 0 || strcmp(command, "//setvat") == 0)
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
    if (strcmp(command, "//resetviewactor") == 0 || strcmp(command, "//resetva"))
    {
        EQ_ResetViewActor();
        std::cout << "Reset View Actor." << std::endl;
        return;
    }

    // map locations
    if (strcmp(command, "//maplocations") == 0 || strcmp(command, "//maploc") == 0)
    {
        EQAPP_MapLocations_WriteToFile();
        return;
    }

    // get map location
    if (strcmp(command, "//getmaplocation") == 0 || strcmp(command, "//getmaploc") == 0)
    {
        // try target first, then player
        DWORD spawnInfo = EQ_GetTargetSpawn();
        if (spawnInfo == NULL)
        {
            spawnInfo = EQ_GetPlayerSpawn();
            if (spawnInfo == NULL)
            {
                std::cout << "Player or Target not found!" << std::endl;
                return;
            }
        }

        EQ_CopySpawnMapLocationToClipboard(spawnInfo);
        std::cout << "Copying player or target map location to the clipboard..." << std::endl;
        return;
    }

    // open zone map file
    if (strcmp(command, "//openzonemapfile") == 0 || strcmp(command, "//ozmf") == 0)
    {
        EQAPP_OpenZoneMapFile();
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
    if (strcmp(command, "//waypointlist") == 0 || strcmp(command, "//wpl") == 0)
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
            EQAPPWaypointPathList pathList = EQAPP_WaypointList_GetPath(fromIndex, toIndex);

            EQAPP_WaypointList_PrintPath(pathList);
        }

        return;
    }
}

#endif // EQAPP_INTERPRETCOMMAND_H
