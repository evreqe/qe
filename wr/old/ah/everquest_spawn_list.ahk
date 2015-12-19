#SingleInstance, Ignore
#Persistent

#Include, C:\AutoHotkey\Lib\Memory.ahk
#Include, C:\AutoHotkey\Lib\Utility.ahk

#Include, everquest.ahk

Global LVM_FIRST         := 0x1000
Global LVM_ENSUREVISIBLE := LVM_FIRST + 19

Gui, Font, s10, Courier New
Gui, Add, StatusBar,, 
Gui, Add, Text, ym, Search:
Gui, Add, Edit, vEditSearch gEditSearch ym w320
Gui, Add, Text, ym, Filter:
Gui, Add, Edit, vEditSearchFilter gEditSearchFilter ym w320
Gui, Add, Text, ym, Option:
Gui, Add, DropDownList, vDropDownListFilterOption gDropDownListFilterOption ym w320 Choose1, by Name||by Title|by Named mobs|by Players|by NPCs
Gui, Add, Text, ym, 
Gui, Add, ListView, vListViewSpawnList gListViewSpawnList +HwndListViewSpawnListHwnd VScroll Grid -Multi w1440 h880 xm, Level|Type|#|Name|Title|Location|Distance|Default Name|
Gui, Add, Button, Default w128 ym, Refresh &Spawn List
Gui, Add, Button, w128, Copy &Location
Gui, Add, Button, w128, Copy &Map Location
Gui, Add, Text,, 
Gui, Add, Button, w64, &Reset
Gui, Add, Text,, 
Gui, Add, Button, w64, E&xit

UpdateListViewSpawnListColumns()
{
    Gui, ListView, ListViewSpawnList

    LV_ModifyCol()
    LV_ModifyCol(1, "AutoHdr")
    LV_ModifyCol(2, "AutoHdr")
    LV_ModifyCol(3, "AutoHdr")
    LV_ModifyCol(4, "AutoHdr")
    LV_ModifyCol(5, "AutoHdr")
    LV_ModifyCol(6, "AutoHdr")
    LV_ModifyCol(7, "AutoHdr")
    LV_ModifyCol(8, "AutoHdr")

    LV_ModifyCol(1, "Integer") ; Level
    LV_ModifyCol(1, "Left")

    LV_ModifyCol(3, "Integer") ; #
    LV_ModifyCol(3, "Left")

    LV_ModifyCol(7, "Integer") ; Distance
    LV_ModifyCol(7, "Left")

    LV_ModifyCol(7, "Sort") ; Sort by Distance
}

UpdateListViewSpawnList()
{
    GuiControl, -Redraw, ListViewSpawnList

    Gui, ListView, ListViewSpawnList
    LV_Delete()

    zone_short_name := EQ_GetZoneShortName()
    zone_long_name  := EQ_GetZoneLongName()

    zone_short_name_live := EQ_GetZoneShortNameLive()

    player_spawn_info := EQ_GetPlayerSpawnInfo()

    player_name := Memory_ReadString(eq_process_handle, player_spawn_info + EQ_OFFSET_SPAWN_INFO_NAME, EQ_NAME_SIZE)

    player_y := Memory_ReadFloat(eq_process_handle, player_spawn_info + EQ_OFFSET_SPAWN_INFO_Y)
    player_x := Memory_ReadFloat(eq_process_handle, player_spawn_info + EQ_OFFSET_SPAWN_INFO_X)
    player_z := Memory_ReadFloat(eq_process_handle, player_spawn_info + EQ_OFFSET_SPAWN_INFO_Z)

    ; shorten to 2 decimal places
    StringTrimRight, player_y, player_y, 4
    StringTrimRight, player_x, player_x, 4
    StringTrimRight, player_z, player_z, 4

    player_location = %player_y%, %player_x%, %player_z%

    ; remove empty decimal places
    StringReplace, player_location, player_location, .00,, All

    player_heading := Memory_ReadFloat(eq_process_handle, player_spawn_info + EQ_OFFSET_SPAWN_INFO_HEADING)

    ; remove all decimal places
    StringTrimRight, player_heading, player_heading, 7

    spawn_info_begin := Memory_Read(eq_process_handle, EQ_SPAWN_INFO_BEGIN_POINTER)

    spawn_info_address := spawn_info_begin

    Loop
    {
        Gui, ListView, ListViewSpawnList

        spawn_next_spawn_info := Memory_Read(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER)

        If (spawn_next_spawn_info = EQ_SPAWN_INFO_NULL)
        {
            Break
        }

        spawn_master_id := Memory_Read(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_MASTER_ID)

        spawn_level := Memory_ReadEx(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_LEVEL, 1)

        spawn_type := Memory_ReadEx(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_TYPE, 1)

        spawn_name      := Memory_ReadString(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_NAME,      EQ_NAME_SIZE)
        spawn_last_name := Memory_ReadString(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_LAST_NAME, EQ_LAST_NAME_SIZE)

        spawn_default_name = %spawn_name%

        spawn_y := Memory_ReadFloat(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_Y)
        spawn_x := Memory_ReadFloat(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_X)
        spawn_z := Memory_ReadFloat(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_Z)

        ; shorten to 2 decimal places
        StringTrimRight, spawn_y, spawn_y, 4
        StringTrimRight, spawn_x, spawn_x, 4
        StringTrimRight, spawn_z, spawn_z, 4

        spawn_location = %spawn_y%, %spawn_x%, %spawn_z%

        ; remove empty decimal places
        StringReplace, spawn_location, spawn_location, .00,, All

        spawn_distance := Utility_CalculateDistance(player_x, player_y, spawn_x, spawn_y)

        ; remove all decimal places
        StringTrimRight, spawn_distance, spawn_distance, 7

        spawn_number := 

        ; find spawn number in spawn name
        find_match := RegExMatch(spawn_name, "O)\d+", match)

        spawn_number := match.Value(0)

        ; remove spawn number from spawn name
        StringTrimRight, spawn_name, spawn_name, match.Len(0)

        ; replace underscores with spaces
        StringReplace, spawn_name, spawn_name, _, %A_Space%, All
        StringReplace, spawn_last_name, spawn_last_name, _, %A_Space%, All

        GuiControlGet, search_filter_text,, EditSearchFilter
        GuiControlGet, filter_option_text,, DropDownListFilterOption

        should_apply_filter := 1

        StringLen, search_filter_text_length, search_filter_text
        If (search_filter_text_length > 0)
        {
            should_apply_filter := InStr(spawn_name, search_filter_text)

            If filter_option_text = by Name
            {
                should_apply_filter := InStr(spawn_name, search_filter_text)
            }

            If option_text = by Title
            {
                should_apply_filter := InStr(spawn_last_name, search_filter_text)
            }
        }

        If filter_option_text = by Named mobs
        {
            should_apply_filter := 0

            FileRead, named_mobs, %A_ScriptDir%\named_mobs\%zone_short_name%.txt
            If ErrorLevel
            {
                should_apply_filter := 0
            }
            Else
            {
                Loop, Parse, named_mobs, `n, `r
                {
                    ;MsgBox, %A_LoopField%

                    If A_LoopField = 
                    {
                        Continue
                    }

                    IfInString, A_LoopField, #
                    {
                        Continue
                    }

                    IfInString, A_LoopField, //
                    {
                        Continue
                    }

                    named_mob = %A_LoopField%

                    If spawn_name = %named_mob%
                    {
                        should_apply_filter := 1

                        Break
                    }
                }
            }
        }

        If filter_option_text = by Players
        {
            should_apply_filter := 0

            If (spawn_type = EQ_SPAWN_INFO_TYPE_PLAYER)
            {
                should_apply_filter := 1
            }
        }

        If filter_option_text = by NPCs
        {
            apply_filter := 0

            If (spawn_type = EQ_SPAWN_INFO_TYPE_NPC)
            {
                If (spawn_master_id = 0) ; skip pets
                {
                    should_apply_filter := 1
                }
            }
        }

        If (should_apply_filter = 0)
        {
            spawn_info_address := spawn_next_spawn_info

            Continue
        }

        If (spawn_type = EQ_SPAWN_INFO_TYPE_PLAYER)
        {
            spawn_type = Player
        }

        If (spawn_type = EQ_SPAWN_INFO_TYPE_NPC)
        {
            If (spawn_master_id = 0) ; no pet owner
            {
                spawn_type = NPC
            }
            Else
            {
                spawn_type = Pet
            }
        }

        If (spawn_type = EQ_SPAWN_INFO_TYPE_NPC_CORPSE || spawn_type = EQ_SPAWN_INFO_TYPE_PLAYER_CORPSE)
        {
            spawn_type = Corpse
        }

        LV_Add("", spawn_level, spawn_type, spawn_number, spawn_name, spawn_last_name, spawn_location, spawn_distance, spawn_default_name)

        spawn_info_address := spawn_next_spawn_info
    }

    UpdateListViewSpawnListColumns()

    GuiControl, +Redraw, ListViewSpawnList

    SB_SetParts(160, 240, 240, 240, 240, 320)
    SB_SetText("Found: "                . LV_GetCount(),        1, 0)
    SB_SetText("Zone Name: "            . zone_long_name,       2, 0)
    SB_SetText("Short Zone Name: "      . zone_short_name,      3, 0)
    SB_SetText("Short Zone Name Live: " . zone_short_name_live, 4, 0)
    SB_SetText("Player Name: "          . player_name,          5, 0)
    SB_SetText("Player Location: "      . player_location,      6, 0)
}

Memory_EnableDebugPrivileges()

eq_process_id     := Memory_GetProcessID(EQ_CLIENT)
eq_process_handle := Memory_GetProcessHandle(eq_process_id)

UpdateListViewSpawnList()
UpdateListViewSpawnListColumns()

Gui, Show, Center, Spawn List

Return

EditSearch:
Gui, ListView, ListViewSpawnList

GuiControlGet, search_text,, EditSearch

If search_text = 
{
    LV_Modify(0, "-Select")
    Return
}

StringLen, search_text_length, search_text
If (search_text_length < 3) ; need at least 3 characters of text to start selecting
{
    LV_Modify(0, "-Select")
    Return
}

GuiControlGet, filter_option_text,, DropDownListFilterOption

search_column := 4 ; column = Name

If filter_option_text = by Name
{
    search_column := 4 ; column = Name
}

If filter_option_text = by Title
{
    search_column := 5 ; column = Title
}

Loop % LV_GetCount()
{
    LV_GetText(row_text, A_Index, search_column)

    If InStr(row_text, search_text)
    {
        LV_Modify(0, "-Select")
        LV_Modify(A_Index, "Select")

        SendMessage, LVM_ENSUREVISIBLE, A_Index - 1, 0,, ahk_id %ListViewSpawnListHwnd%

        Break
    }
}
Return

EditSearchFilter:
Return

DropDownListFilterOption:
Return

ListViewSpawnList:
If A_GuiEvent = DoubleClick
{
    LV_GetText(spawn_name, A_EventInfo, 4) ; column = Name

    Clipboard = %spawn_name%
}
Return

ButtonRefreshSpawnList:
UpdateListViewSpawnList()
Return

ButtonCopyLocation:
Gui, ListView, ListViewSpawnList

LV_GetText(location, LV_GetNext(0), 6) ; column = Location

Clipboard = %location%
Return

ButtonCopyMapLocation:
Gui, ListView, ListViewSpawnList

LV_GetText(spawn_name,     LV_GetNext(0), 4) ; column = Name
LV_GetText(spawn_location, LV_GetNext(0), 6) ; column = Location

StringReplace, spawn_name, spawn_name, %A_Space%, _, All

StringSplit, spawn_location_array, spawn_location, `,

spawn_y = %spawn_location_array1%
spawn_x = %spawn_location_array2%
spawn_z = %spawn_location_array3%

spawn_y_reversed := Utility_ReverseSign(spawn_y)
spawn_x_reversed := Utility_ReverseSign(spawn_x)

text_red   := 196
text_green := 0
text_blue  := 0

text_size := 2

Clipboard = P %spawn_x_reversed%, %spawn_y_reversed%, %spawn_z%,  %text_red%, %text_green%, %text_blue%,  %text_size%,  %spawn_name%
Return

ButtonReset:
GuiControl,, EditSearch, 
GuiControl,, EditSearchFilter, 
GuiControl, ChooseString, DropDownListFilterOption, by Name
Return

ButtonExit:
GuiClose:
GuiEscape:

Memory_CloseHandle(eq_process_handle)
ExitApp

Return
