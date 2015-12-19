#Include, C:\AutoHotkey\Lib\Memory.ahk
#Include, C:\AutoHotkey\Lib\Utility.ahk

#Include, everquest.ahk

Global ini_file := "everquest_map_locations.ini"

Global target_only := 0

Memory_EnableDebugPrivileges()

eq_process_id     := Memory_GetProcessID(EQ_CLIENT)
eq_process_handle := Memory_GetProcessHandle(eq_process_id)

zone_short_name_live := EQ_GetZoneShortNameLive()

;FileDelete, %A_ScriptDir%\map_locations\%zone_short_name_live%_1.txt

IniRead, target_only, %ini_file%, Options, TargetOnly

spawn_info_begin := Memory_Read(eq_process_handle, EQ_SPAWN_INFO_BEGIN_POINTER)

spawn_info_address := spawn_info_begin

Loop
{
    spawn_next_spawn_info := Memory_Read(eq_process_handle,  spawn_info_address + EQ_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER)

    If (spawn_next_spawn_info = EQ_SPAWN_INFO_NULL)
    {
        Break
    }

    If (target_only != 0)
    {
        target_spawn_info := EQ_GetTargetSpawnInfo()

        If (spawn_info_address != target_spawn_info)
        {
            spawn_info_address := spawn_next_spawn_info

            Continue
        }
    }

    spawn_type := Memory_ReadEx(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_TYPE, 1)

    ;If (spawn_type != EQ_SPAWN_INFO_TYPE_NPC)
    ;{
        ;spawn_info_address := spawn_next_spawn_info

        ;Continue
    ;}

    spawn_name      := Memory_ReadString(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_NAME,      EQ_NAME_SIZE)
    spawn_last_name := Memory_ReadString(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_LAST_NAME, EQ_LAST_NAME_SIZE)

    StringReplace, spawn_name, spawn_name, %A_Space%, _, All
    StringReplace, spawn_last_name, spawn_last_name, %A_Space%, _, All

    ;spawn_name = %spawn_name%%spawn_last_name%

    spawn_y := Memory_ReadFloat(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_Y)
    spawn_x := Memory_ReadFloat(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_X)
    spawn_z := Memory_ReadFloat(eq_process_handle, spawn_info_address + EQ_OFFSET_SPAWN_INFO_Z)

    spawn_location = %spawn_y%, %spawn_x%, %spawn_z%

    find_match := RegExMatch(spawn_name, "O)\d+", match)

    StringTrimRight, spawn_name, spawn_name, match.Len(0)

    spawn_y_reversed := Utility_ReverseSign(spawn_y)
    spawn_x_reversed := Utility_ReverseSign(spawn_x)

    text_red   := 196
    text_green := 0
    text_blue  := 0

    text_size := 2

    spawn_y_reversed := Round(spawn_y_reversed, 4)
    spawn_x_reversed := Round(spawn_x_reversed, 4)

    spawn_z := Round(spawn_z, 4)

    map_location = P %spawn_x_reversed%, %spawn_y_reversed%, %spawn_z%,  %text_red%, %text_green%, %text_blue%,  %text_size%,  %spawn_name%

    ;MsgBox, zone_short_name_live = %zone_short_name_live%`nspawn_name = %spawn_name%`nspawn_location = %spawn_location%`nmap_location = %map_location%

    FileAppend, %map_location%`n, %A_ScriptDir%\map_locations\%zone_short_name_live%_1.txt

    spawn_info_address := spawn_next_spawn_info
}

Return
