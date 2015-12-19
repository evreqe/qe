#SingleInstance, Ignore
#Persistent

#Include, C:\AutoHotkey\Lib\Memory.ahk

#Include, everquest.ahk

Global psapi_dll_handle := 0

Global track_spawn_info_address := 0

Global LVM_FIRST         := 0x1000
Global LVM_ENSUREVISIBLE := LVM_FIRST + 19

Global PI := 3.14159265358979323846

Gui, Font, s10, Courier New
Gui, Add, StatusBar,, 
Gui, Add, Text, ym, Search:
Gui, Add, Edit, vEditTrackingSearch gEditTrackingSearch ym w320
Gui, Add, Text, ym, Filter:
Gui, Add, Edit, vEditTrackingFilter gEditTrackingFilter ym w320
Gui, Add, Text, ym, Option:
Gui, Add, DropDownList, vDropDownListTrackingOption gDropDownListTrackingOption ym w320 Choose1, by Name||by Title|by Named mobs|by Players|by NPCs
Gui, Add, Text, ym, 
Gui, Add, Checkbox, vCheckboxTracking gCheckboxTracking ym, Track Enabled
Gui, Add, ListView, vListViewTracking gListViewTracking +HwndListViewHwndTracking VScroll Grid -Multi w1440 h880 xm, Address|ID|Type|#|Name|Title|Location|Distance|Track|
Gui, Add, Button, Default w128 ym, Refresh &Spawn List
Gui, Add, Button, w128, &Track
Gui, Add, Text,, 
Gui, Add, Button, w128, Copy &Location
Gui, Add, Button, w128, Copy &Map Location
Gui, Add, Text,, 
Gui, Add, Button, w64, &Reset
Gui, Add, Text,, 
Gui, Add, Button, w64, E&xit

Gui, 2: Font, s10, Courier New
Gui, 2: Add, Text, vTrackingText gTrackingText w480 h60, ...
Gui, 2: +AlwaysOnTop +Border +Caption +ToolWindow -MaximizeBox -MinimizeBox

UpdateListViewColumnsTracking()
{
    Gui, ListView, ListViewTracking

    LV_ModifyCol()
    LV_ModifyCol(1, "AutoHdr")
    LV_ModifyCol(2, "AutoHdr")
    LV_ModifyCol(3, "AutoHdr")
    LV_ModifyCol(4, "AutoHdr")
    LV_ModifyCol(5, "AutoHdr")
    LV_ModifyCol(6, "AutoHdr")
    LV_ModifyCol(7, "AutoHdr")
    LV_ModifyCol(8, "AutoHdr")
    LV_ModifyCol(9, "AutoHdr")

    LV_ModifyCol(1, "Integer") ; Address
    LV_ModifyCol(1, "Left")

    LV_ModifyCol(2, "Integer") ; ID
    LV_ModifyCol(2, "Left")

    LV_ModifyCol(4, "Integer") ; #
    LV_ModifyCol(4, "Left")

    LV_ModifyCol(8, "Integer") ; Distance
    LV_ModifyCol(8, "Left")

    LV_ModifyCol(8, "Sort") ; Sort by Distance
}

UpdateListViewTracking()
{
    GuiControl, -Redraw, ListViewTracking

    Gui, ListView, ListViewTracking
    LV_Delete()

    zone_short_name := everquest_GetZoneShortName()
    zone_long_name  := everquest_GetZoneLongName()

    zone_short_name_live := everquest_GetZoneShortNameLive()

    player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

    player_name := Memory_ReadString(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_NAME, 0x40)

    player_avatar_height := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_AVATAR_HEIGHT)

    player_y := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y)
    player_x := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X)
    player_z := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z)

    ; shorten to 2 decimal places
    StringTrimRight, player_y, player_y, 4
    StringTrimRight, player_x, player_x, 4
    StringTrimRight, player_z, player_z, 4

    player_location = %player_y%, %player_x%, %player_z%

    ; remove empty decimal places
    StringReplace, player_location, player_location, .00,, All

    player_heading := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING)

    ; remove all decimal places
    StringTrimRight, player_heading, player_heading, 7

    player_cardinal_direction := GetCardinalDirectionFromHeading(player_heading)

    spawn_info_begin := Memory_Read(everquest_process_handle, EVERQUEST_SPAWN_INFO_BEGIN_POINTER)

    SetFormat, IntegerFast, H

    spawn_info_address := spawn_info_begin

    Loop
    {
        Gui, ListView, ListViewTracking

        spawn_next_spawn_info := Memory_Read(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER)

        If (spawn_next_spawn_info = EVERQUEST_SPAWN_INFO_NULL)
        {
            Break
        }

        spawn_id := Memory_Read(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_SPAWN_ID)

        spawn_master_id := Memory_Read(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_MASTER_ID)

        spawn_type := Memory_ReadEx(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_TYPE, 1)

        spawn_name      := Memory_ReadString(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NAME,      0x40)
        spawn_last_name := Memory_ReadString(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LAST_NAME, 0x20)

        spawn_y := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Y)
        spawn_x := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_X)
        spawn_z := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Z)

        ; shorten to 2 decimal places
        StringTrimRight, spawn_y, spawn_y, 4
        StringTrimRight, spawn_x, spawn_x, 4
        StringTrimRight, spawn_z, spawn_z, 4

        spawn_location = %spawn_y%, %spawn_x%, %spawn_z%

        ; remove empty decimal places
        StringReplace, spawn_location, spawn_location, .00,, All

        spawn_distance := CalculateDistance(player_x, player_y, spawn_x, spawn_y)

        ; remove all decimal places
        StringTrimRight, spawn_distance, spawn_distance, 7

        delta_y := spawn_y - player_y
        delta_x := spawn_x - player_x

        spawn_angle := ATan(delta_y / delta_x) * 180 / PI

        ; remove all decimal places
        StringTrimRight, spawn_angle, spawn_angle, 7

        spawn_track := GetTrack(player_cardinal_direction, player_avatar_height, player_y, player_x, player_z, spawn_distance, spawn_y, spawn_x, spawn_z)

        spawn_number := 

        ; find spawn number in spawn name
        found_match := RegExMatch(spawn_name, "O)\d+", match)

        spawn_number := match.Value(0)

        ; remove spawn number from spawn name
        StringTrimRight, spawn_name, spawn_name, match.Len(0)

        ; replace underscores with spaces
        StringReplace, spawn_name, spawn_name, _, %A_Space%, All
        StringReplace, spawn_last_name, spawn_last_name, _, %A_Space%, All

        GuiControlGet, filter_text,, EditTrackingFilter
        GuiControlGet, option_text,, DropDownListTrackingOption

        apply_filter := 1

        StringLen, filter_text_length, filter_text
        If (filter_text_length > 0)
        {
            apply_filter := InStr(spawn_name, filter_text)

            If option_text = by Name
            {
                apply_filter := InStr(spawn_name, filter_text)
            }

            If option_text = by Title
            {
                apply_filter := InStr(spawn_last_name, filter_text)
            }
        }

        If option_text = by Named mobs
        {
            apply_filter := 0

            FileRead, named_mobs, %A_ScriptDir%\named_mobs\%zone_short_name%.txt
            If ErrorLevel
            {
                apply_filter := 0
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
                        apply_filter := 1

                        Break
                    }
                }
            }
        }

        If option_text = by Players
        {
            apply_filter := 0

            If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
            {
                apply_filter := 1
            }
        }

        If option_text = by NPCs
        {
            apply_filter := 0

            If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_NPC)
            {
                If (spawn_master_id = 0) ; skip pets
                {
                    apply_filter := 1
                }
            }
        }

        If (apply_filter = 0)
        {
            spawn_info_address := spawn_next_spawn_info

            Continue
        }

        If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
        {
            spawn_type = Player
        }

        If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_NPC)
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

        If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_NPC_CORPSE || spawn_type = EVERQUEST_SPAWN_INFO_TYPE_PLAYER_CORPSE)
        {
            spawn_type = Corpse
        }

        LV_Add("", spawn_info_address, spawn_id, spawn_type, spawn_number, spawn_name, spawn_last_name, spawn_location, spawn_distance, spawn_track)

        spawn_info_address := spawn_next_spawn_info
    }

    UpdateListViewColumnsTracking()

    GuiControl, +Redraw, ListViewTracking

    SetFormat, IntegerFast, D

    SB_SetParts(160, 240, 240, 240, 240, 320)
    SB_SetText("Found: "                . LV_GetCount(),        1, 0)
    SB_SetText("Zone Name: "            . zone_long_name,       2, 0)
    SB_SetText("Short Zone Name: "      . zone_short_name,      3, 0)
    SB_SetText("Short Zone Name Live: " . zone_short_name_live, 4, 0)
    SB_SetText("Player Name: "          . player_name,          5, 0)
    SB_SetText("Player Location: "      . player_location,      6, 0)
}

GetCardinalDirectionFromHeading(heading)
{
    If (heading = 0)
    {
        Return, "North"
    }

    If (heading = 64)
    {
        Return, "North West"
    }

    If (heading = 128)
    {
        Return, "West"
    }

    If (heading = 192)
    {
        Return, "South West"
    }

    If (heading = 256)
    {
        Return, "South"
    }

    If (heading = 320)
    {
        Return, "South East"
    }

    If (heading = 384)
    {
        Return, "East"
    }

    If (heading = 448)
    {
        Return, "North East"
    }

    ;If (heading >= 0 && heading <= 31)
    If heading Between 0 And 31
    {
        Return, "North"
    }

    If (heading >= 32 && heading <= 95)
    {
        Return, "North West"
    }

    If (heading >= 96 && heading <= 159)
    {
        Return, "West"
    }

    If (heading >= 160 && heading <= 223)
    {
        Return, "South West"
    }

    If (heading >= 224 && heading <= 287)
    {
        Return, "South"
    }

    If (heading >= 288 && heading <= 351)
    {
        Return, "South East"
    }

    If (heading >= 352 && heading <= 415)
    {
        Return, "East"
    }

    If (heading >= 416 && heading <= 479)
    {
        Return, "North East"
    }

    If (heading >= 480 && heading <= 511)
    {
        Return, "North"
    }

    Return, "Null"
}

GetTrack(player_cardinal_direction, player_avatar_height, player_y, player_x, player_z, spawn_distance, spawn_y, spawn_x, spawn_z)
{
    spawn_track = 

    If (player_y > spawn_y && player_x < spawn_x)
    {
        spawn_track = You are North East of me!

        If player_cardinal_direction = North
        {
            spawn_track = Behind and to the Left
        }

        If player_cardinal_direction = North West
        {
            spawn_track = Left
        }

        If player_cardinal_direction = West
        {
            spawn_track = Ahead and to the Left
        }

        If player_cardinal_direction = South West
        {
            spawn_track = Straight Ahead
        }

        If player_cardinal_direction = South
        {
            spawn_track = Ahead and to the Right
        }

        If player_cardinal_direction = South East
        {
            spawn_track = Right
        }

        If player_cardinal_direction = East
        {
            spawn_track = Behind and to the Right
        }

        If player_cardinal_direction = North East
        {
            spawn_track = Behind You
        }
    }

    If (player_y > spawn_y && player_x > spawn_x)
    {
        spawn_track = You are North West of me!

        If player_cardinal_direction = North
        {
            spawn_track = Behind and to the Right
        }

        If player_cardinal_direction = North West
        {
            spawn_track = Behind You
        }

        If player_cardinal_direction = West
        {
            spawn_track = Behind and to the Left
        }

        If player_cardinal_direction = South West
        {
            spawn_track = Left
        }

        If player_cardinal_direction = South
        {
            spawn_track = Ahead and to the Left
        }

        If player_cardinal_direction = South East
        {
            spawn_track = Straight Ahead
        }

        If player_cardinal_direction = East
        {
            spawn_track = Ahead and to the Right
        }

        If player_cardinal_direction = North East
        {
            spawn_track = Right
        }
    }

    If (player_y < spawn_y && player_x < spawn_x)
    {
        spawn_track = You are South East of me!

        If player_cardinal_direction = North
        {
            spawn_track = Ahead and to the Left
        }

        If player_cardinal_direction = North West
        {
            spawn_track = Straight Ahead
        }

        If player_cardinal_direction = West
        {
            spawn_track = Ahead and to the Right
        }

        If player_cardinal_direction = South West
        {
            spawn_track = Right
        }

        If player_cardinal_direction = South
        {
            spawn_track = Behind and to the Right
        }

        If player_cardinal_direction = South East
        {
            spawn_track = Behind You
        }

        If player_cardinal_direction = East
        {
            spawn_track = Behind and to the Left
        }

        If player_cardinal_direction = North East
        {
            spawn_track = Left
        }
    }

    If (player_y < spawn_y && player_x > spawn_x)
    {
        spawn_track = You are South West of me!

        If player_cardinal_direction = North
        {
            spawn_track = Ahead and to the Right
        }

        If player_cardinal_direction = North West
        {
            spawn_track = Right
        }

        If player_cardinal_direction = West
        {
            spawn_track = Behind and to the Right
        }

        If player_cardinal_direction = South West
        {
            spawn_track = Behind You
        }

        If player_cardinal_direction = South
        {
            spawn_track = Behind and to the Left
        }

        If player_cardinal_direction = South East
        {
            spawn_track = Left
        }

        If player_cardinal_direction = East
        {
            spawn_track = Ahead and to the Left
        }

        If player_cardinal_direction = North East
        {
            spawn_track = Straight Ahead
        }
    }

    If (player_y > spawn_y)
    {
        If IsInRange(player_x, spawn_x, 16)
        {
            spawn_track = You are North of me!

            IfInString, player_cardinal_direction, North
            {
                spawn_track = Behind You
            }

            If player_cardinal_direction = West
            {
                spawn_track = Left
            }

            IfInString, player_cardinal_direction, South
            {
                spawn_track = Straight Ahead
            }

            If player_cardinal_direction = East
            {
                spawn_track = Right
            }
        }
    }

    If (player_y < spawn_y)
    {
        If IsInRange(player_x, spawn_x, 16)
        {
            spawn_track = You are South of me!

            IfInString, player_cardinal_direction, North
            {
                spawn_track = Straight Ahead
            }

            If player_cardinal_direction = West
            {
                spawn_track = Right
            }

            IfInString, player_cardinal_direction, South
            {
                spawn_track = Behind You
            }

            If player_cardinal_direction = East
            {
                spawn_track = Left
            }
        }
    }

    If (player_x > spawn_x)
    {
        If IsInRange(player_y, spawn_y, 16)
        {
            spawn_track = You are West of me!

            If player_cardinal_direction = North
            {
                spawn_track = Right
            }

            IfInString, player_cardinal_direction, West
            {
                spawn_track = Behind You
            }

            If player_cardinal_direction = South
            {
                spawn_track = Left
            }

            IfInString, player_cardinal_direction, East
            {
                spawn_track = Straight Ahead
            }
        }
    }

    If (player_x < spawn_x)
    {
        If IsInRange(player_y, spawn_y, 16)
        {
            spawn_track = You are East of me!

            If player_cardinal_direction = North
            {
                spawn_track = Left
            }

            IfInString, player_cardinal_direction, West
            {
                spawn_track = Straight Ahead
            }

            If player_cardinal_direction = South
            {
                spawn_track = Right
            }

            IfInString, player_cardinal_direction, East
            {
                spawn_track = Behind You
            }
        }
    }

    If (spawn_distance <= 16)
    {
        spawn_track = Near You
    }

    If ((spawn_z - player_z) > (player_avatar_height * 2))
    {
        If (spawn_distance <= 32)
        {
            spawn_track = %spawn_track% and Directly Above You
        }
        Else
        {
            spawn_track = %spawn_track% and Above You
        }
    }

    If ((player_z - spawn_z) > (player_avatar_height * 2))
    {
        If (spawn_distance <= 32)
        {
            spawn_track = %spawn_track% and Directly Below You
        }
        Else
        {
            spawn_track = %spawn_track% and Below You
        }
    }

    Return, spawn_track
}

StopTracking()
{
    GuiControl, 1:, CheckboxTracking, 0
    track_spawn_info_address := 0
    Gui, 2: Cancel
}

Memory_EnableDebugPrivileges()

psapi_dll_handle := DllCall("LoadLibrary", "Str", "Psapi.dll")

everquest_process_id := Memory_GetProcessID(EVERQUEST_CLIENT)

;MsgBox, everquest_process_id = %everquest_process_id%

If (everquest_process_id = 0)
{
    MsgBox, Error: everquest_process_id = 0
    ExitApp
    Return
}

everquest_process_handle := Memory_GetProcessHandle(everquest_process_id)

;MsgBox, everquest_process_handle = %everquest_process_handle%

If (everquest_process_handle = 0)
{
    MsgBox, Error: OpenProcess failed! Result: %A_LastError%
    ExitApp
    Return
}

UpdateListViewTracking()
UpdateListViewColumnsTracking()

GuiControl, Focus, Track

Gui, Show, Center, t

SetTimer, TimerTrack, 1000

SetTimer, TimerUpdateProcess, 100

Return

CheckboxTracking:
GuiControlGet, is_checked,, CheckboxTracking
If (is_checked = 0)
{
    StopTracking()
}
Return

ButtonTrack:
{
    Gui, ListView, ListViewTracking

    LV_GetText(track_spawn_info_address, LV_GetNext(0), 1) ; column = Address

    GuiControlGet, is_checked,, CheckboxTracking
    If (is_checked = 0)
    {
        GuiControl,, CheckboxTracking, 1
    }
}

ListViewTracking:
If A_GuiEvent = DoubleClick
{
    LV_GetText(track_spawn_info_address, A_EventInfo, 1) ; column = Address

    GuiControlGet, is_checked,, CheckboxTracking
    If (is_checked = 0)
    {
        GuiControl,, CheckboxTracking, 1
    }
}
Return

EditTrackingSearch:
Gui, ListView, ListViewTracking

GuiControlGet, search_text,, EditTrackingSearch

If search_text = 
{
    LV_Modify(0, "-Select")
    Return
}

StringLen, search_text_length, search_text
If (search_text_length < 3)
{
    LV_Modify(0, "-Select")
    Return
}

GuiControlGet, option_text,, DropDownListTrackingOption

search_column := 5 ; column = Name

If option_text = by Name
{
    search_column := 5 ; column = Name
}

If option_text = by Title
{
    search_column := 6 ; column = Title
}

Loop % LV_GetCount()
{
    LV_GetText(row_text, A_Index, search_column)

    If InStr(row_text, search_text)
    {
        LV_Modify(0, "-Select")
        LV_Modify(A_Index, "Select")

        SendMessage, LVM_ENSUREVISIBLE, A_Index - 1, 0,, ahk_id %ListViewHwndTracking%

        Break
    }
}
Return

EditTrackingFilter:
Return

DropDownListTrackingOption:
Return

ButtonRefreshSpawnList:
UpdateListViewTracking()
Return

ButtonCopyLocation:
Gui, ListView, ListViewTracking

LV_GetText(location, LV_GetNext(0), 7) ; column = Location

Clipboard = %location%
Return

ButtonCopyMapLocation:
Gui, ListView, ListViewTracking

LV_GetText(spawn_name,     LV_GetNext(0), 5) ; column = Name
LV_GetText(spawn_location, LV_GetNext(0), 7) ; column = Location

StringReplace, spawn_name, spawn_name, %A_Space%, _, All

StringSplit, spawn_location_array, spawn_location, `,

spawn_y = %spawn_location_array1%
spawn_x = %spawn_location_array2%
spawn_z = %spawn_location_array3%

spawn_y_reversed := ReverseSign(spawn_y)
spawn_x_reversed := ReverseSign(spawn_x)

text_red   := 196
text_green := 0
text_blue  := 0

text_size := 2

Clipboard = P %spawn_x_reversed%, %spawn_y_reversed%, %spawn_z%,  %text_red%, %text_green%, %text_blue%,  %text_size%,  %spawn_name%
Return

ButtonReset:
GuiControl,, EditTrackingSearch, 
GuiControl,, EditTrackingFilter, 
GuiControl, ChooseString, DropDownListTrackingOption, by Name
StopTracking()
Return

TimerTrack:
If (track_spawn_info_address = 0)
{
    Return
}

GuiControlGet, is_checked,, CheckboxTracking
If (is_checked = 0)
{
    Return
}

;MsgBox, track_spawn_info_address = %track_spawn_info_address%

player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

player_avatar_height := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_AVATAR_HEIGHT)

player_y := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y)
player_x := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X)
player_z := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z)

; shorten to 2 decimal places
StringTrimRight, player_y, player_y, 4
StringTrimRight, player_x, player_x, 4
StringTrimRight, player_z, player_z, 4

player_heading := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING)

; remove all decimal places
StringTrimRight, player_heading, player_heading, 7

player_cardinal_direction := GetCardinalDirectionFromHeading(player_heading)

spawn_info_address := track_spawn_info_address

spawn_name      := Memory_ReadString(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NAME,      0x40)
spawn_last_name := Memory_ReadString(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LAST_NAME, 0x20)

spawn_y := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Y)
spawn_x := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_X)
spawn_z := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Z)

; shorten to 2 decimal places
StringTrimRight, spawn_y, spawn_y, 4
StringTrimRight, spawn_x, spawn_x, 4
StringTrimRight, spawn_z, spawn_z, 4

spawn_distance := CalculateDistance(player_x, player_y, spawn_x, spawn_y)

; remove all decimal places
StringTrimRight, spawn_distance, spawn_distance, 7

spawn_track := GetTrack(player_cardinal_direction, player_avatar_height, player_y, player_x, player_z, spawn_distance, spawn_y, spawn_x, spawn_z)

;mouse_x := Memory_Read(everquest_process_handle, EVERQUEST_MOUSE_X)
;mouse_y := Memory_Read(everquest_process_handle, EVERQUEST_MOUSE_Y)

window_x := Memory_Read(everquest_process_handle, EVERQUEST_WINDOW_X)
window_y := Memory_Read(everquest_process_handle, EVERQUEST_WINDOW_Y)

show_x := window_x ; + mouse_x
show_y := window_y ; + mouse_y

GuiControl, 2:, TrackingText, Name: %spawn_name%`nDistance: %spawn_distance%`n%spawn_track%

Gui, 2: Show, x%show_x% y%show_y% AutoSize NoActivate, Tracking...
Return

TrackingText:
Return

XButton1::

UpdateListViewTracking() 

Gui, ListView, ListViewTracking

spawn_names := 

Loop % LV_GetCount()
{
    LV_GetText(spawn_type,     A_Index, 3) ; Type
    LV_GetText(spawn_name,     A_Index, 5) ; Name
    LV_GetText(spawn_title,    A_Index, 6) ; Title
    LV_GetText(spawn_distance, A_Index, 8) ; Distance

    If spawn_title = 
    {
        ;
    }
    Else
    {
        If spawn_type = NPC
        {
            spawn_title = (%spawn_title%)
        }
    }

    found_admin := False

    IfInString, spawn_name, Admin-
    {
        found_admin := True
    }

    IfInString, spawn_name, Dev-
    {
        found_admin := True
    }

    IfInString, spawn_name, GM-
    {
        found_admin := True
    }

    If (found_admin)
    {
        spawn_name = ***%A_Space%%spawn_name%
    }

    spawn_names = %spawn_names%%spawn_distance%%A_Tab%%spawn_name%%A_Space%%spawn_title%`n
}

mouse_x := Memory_Read(everquest_process_handle, EVERQUEST_MOUSE_X)
mouse_y := Memory_Read(everquest_process_handle, EVERQUEST_MOUSE_Y)

window_x := Memory_Read(everquest_process_handle, EVERQUEST_WINDOW_X)
window_y := Memory_Read(everquest_process_handle, EVERQUEST_WINDOW_Y)

show_x := window_x + mouse_x
show_y := window_y + mouse_y

Gui, 3: Font, s10, Courier New
Gui, 3: Add, Text,, %spawn_names%
Gui, 3: +AlwaysOnTop +Border +Caption +ToolWindow -MaximizeBox -MinimizeBox
Gui, 3: Show, x%show_x% y%show_y% AutoSize NoActivate, Tracker

KeyWait, XButton1
Gui, 3: Destroy
Return

XButton2::
StopTracking()
Return

2GuiClose:
2GuiEscape:
StopTracking()
Return

TimerUpdateProcess:
If WinNotActive, EverQuest
{
    Return
}

WinGet, active_process_id, PID, A

script_process_id := DllCall("GetCurrentProcessId")

If (active_process_id = script_process_id)
{
    Return
}

If (active_process_id = everquest_process_id)
{
    Return
}

psapi_size := 4096

psapi_size := VarSetCapacity(psapi_processes, psapi_size)
DllCall("Psapi.dll\EnumProcesses", "Ptr", &psapi_processes, "UInt", psapi_size, "UIntP", psapi_process)

Loop, % psapi_process // 4
{
    psapi_process_id := NumGet(psapi_processes, A_Index * 4, "UInt")

    psapi_process_handle := DllCall("OpenProcess", "UInt", 0x0010 | 0x0400, "Int", false, "UInt", psapi_process_id, "Ptr")

    If (!psapi_process_handle)
    {
        Continue
    }

    VarSetCapacity(psapi_process_name, psapi_size, 0)
    psapi_result := DllCall("Psapi.dll\GetModuleBaseName", "Ptr", psapi_process_handle, "Ptr", 0, "Str", psapi_process_name, "UInt", A_IsUnicode ? psapi_size // 2 : psapi_size)

    If (!psapi_result)
    {
        If psapi_result := DllCall("Psapi.dll\GetProcessImageFileName", "Ptr", psapi_process_handle, "Str", psapi_process_name, "UInt", A_IsUnicode ? psapi_size // 2 : psapi_size)
        {
            SplitPath, psapi_process_name, psapi_process_name
        }
    }

    DllCall("CloseHandle", "Ptr", psapi_process_handle)

    If (psapi_result && psapi_process_name)
    {
        If psapi_process_name = %EVERQUEST_CLIENT%
        {
            If (psapi_process_id = active_process_id)
            {
                everquest_process_id := psapi_process_id

                everquest_process_handle := Memory_GetProcessHandle(everquest_process_id)

                Break
            }
        }
    }
}

; TOO SLOW
;
;For winmgmts_process In ComObjGet("winmgmts:").ExecQuery("Select * from Win32_Process")
;{
;    winmgmts_process_name := winmgmts_process.Name
;
;    If winmgmts_process_name = %EVERQUEST_CLIENT%
;    {
;        winmgmts_process_id := winmgmts_process.ProcessId
;
;        If winmgmts_process_id = %active_process_id%
;        {
;            everquest_process_id := active_process_id
;
;            everquest_process_handle := Memory_GetProcessHandle(everquest_process_id)
;        }
;    }
;}

Return

ButtonExit:
GuiClose:
GuiEscape:

DllCall("FreeLibrary", "Ptr", psapi_dll_handle)
Memory_CloseHandle(everquest_process_handle)

ExitApp

Return
