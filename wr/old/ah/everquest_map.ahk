#SingleInstance, Off
#Persistent

#MaxHotkeysPerInterval 999999

#Include, everquest.ahk
#Include, shardsofdalaya.ahk

Global psapi_dll_handle := 0

Global script_process_id := 0

Global script_hwnd := 0

Global script_hdc := 0

Global script_hdc_mem := 0

Global script_hbm_mem := 0

Global script_font := 0

Global script_brush := 0

Global script_pen := 0

Global PI := 3.14159265358979323846

Global font_name := "Small Fonts"

Global FONT_SIZE_DEFAULT := 12

Global font_size := FONT_SIZE_DEFAULT

Global FONT_WEIGHT_NORMAL := 400
Global FONT_WEIGHT_BOLD   := 700

Global OPAQUE      := 0
Global TRANSPARENT := 1

Global SRCCOPY := 0xCC0020

Global BLACKNESS := 0x00000042L
Global WHITENESS := 0x00FF0062L

Global DC_BRUSH := 18
Global DC_PEN   := 19

; blue green red
; COLOR_x := 0xBBGGRR
Global COLOR_BLACK       := 0x000000
Global COLOR_WHITE       := 0xFFFFFF

Global COLOR_GREY        := 0x808080

Global COLOR_GREY_DARK   := 0x404040

Global COLOR_RED         := 0x0000FF
Global COLOR_GREEN       := 0x00FF00
Global COLOR_BLUE        := 0xFF0000

Global COLOR_RED_DARK    := 0x000080
Global COLOR_GREEN_DARK  := 0x008000
Global COLOR_BLUE_DARK   := 0x800000

Global COLOR_PINK        := 0xFF00FF
Global COLOR_TEAL        := 0xFFFF00
Global COLOR_YELLOW      := 0x00FFFF

Global COLOR_PURPLE  := 0x800080
Global COLOR_TEAL_EX := 0x808000
Global COLOR_OLIVE   := 0x008080

Global COLOR_ORANGE      := 0x4080FF

Global window_width  := 1280
Global window_height := 720

Global map_origin_x := window_width  / 2
Global map_origin_y := window_height / 2

Global map_draw_x := 0
Global map_draw_y := 0

Global map_offset_x := 0
Global map_offset_y := 0

Global map_offset_multiplier := 10.0

Global MAP_ZOOM_DEFAULT := 1.0

Global MAP_ZOOM_MIN := 0.1

Global map_zoom := MAP_ZOOM_DEFAULT

Global map_zoom_multiplier := 0.25

Global map_file := "null"

Global map_lines  := Array()
Global map_points := Array()

Global num_lines    := 0
Global num_lines_ex := 0

Global num_spawns    := 0
Global num_spawns_ex := 0

Global num_points    := 0
Global num_points_ex := 0

Global line_max_range  := 0
Global spawn_max_range := 0

Global QueryPerformanceCounter_frequency := 1000

Global QueryPerformanceCounter_before := 0
Global QueryPerformanceCounter_after  := 0

Global zone_short_name_live_global := "null"

Gui, 2: Add, Text, ym, Filter:
Gui, 2: Add, DropDownList, vDropDownListFilter w320 Choose1, All||Named Mobs|Players|NPCs

;Gui, 2: Show, x0 y0

EnableDebugPrivileges()

psapi_dll_handle := DllCall("LoadLibrary", "Str", "Psapi.dll")

script_process_id := DllCall("GetCurrentProcessId")

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

SetTimer, TimerUpdateProcess, 100

Gui, +AlwaysOnTop +Resize
Gui, Show, Center w%window_width% h%window_height%, m

script_hwnd := WinExist("A")

script_hdc := DllCall("GetDC", "UInt", script_hwnd)

script_hdc_mem := DllCall("CreateCompatibleDC", "UInt", script_hdc)

script_hbm_mem := DllCall("CreateCompatibleBitmap", "UInt", script_hdc, "Int", window_width, "Int", window_height)

script_font := DllCall("CreateFont", "Int", font_size, "Int", 0, "Int", 0, "Int", 0, "Int", FONT_WEIGHT_NORMAL, "UInt", 0, "UInt", 0, "UInt", 0, "UInt", 0, "UInt", 0, "UInt", 0, "UInt", 0, "UInt", 0, "Str", font_name)

script_font_bold := DllCall("CreateFont", "Int", font_size, "Int", 0, "Int", 0, "Int", 0, "Int", FONT_WEIGHT_BOLD, "UInt", 0, "UInt", 0, "UInt", 0, "UInt", 0, "UInt", 0, "UInt", 0, "UInt", 0, "UInt", 0, "Str", font_name)

script_brush := DllCall("GetStockObject", "Int", DC_BRUSH)

script_pen := DllCall("GetStockObject", "Int", DC_PEN)

DllCall("SetBkColor", "UInt", script_hdc_mem, "Int", COLOR_BLACK)
DllCall("SetBkMode", UInt, script_hdc_mem, UInt, TRANSPARENT)

SetTimer, TimerUpdateMapZone, 100

Hotkey, IfWinActive, ahk_pid %script_process_id%
Hotkey, F1,        ShowOptions
Hotkey, Home,      MapCenter
Hotkey, Up,        MapScrollUp
Hotkey, Down,      MapScrollDown
Hotkey, Left,      MapScrollLeft
Hotkey, Right,     MapScrollRight
Hotkey, MButton,   MapZoomReset
Hotkey, WheelUp,   MapZoomIn
Hotkey, WheelDown, MapZoomOut
Hotkey, ~RButton,  MapClick

;SetTimer, TimerDrawMap, 1

Loop
{
    DrawMap()
}

Return

GuiSize:
If (A_EventInfo = 2) ; maximized
{
    WinSet, AlwaysOnTop, Off, ahk_id %script_hwnd%
}
Else
{
    WinSet, AlwaysOnTop, On, ahk_id %script_hwnd%
}

window_width  := A_GuiWidth
window_height := A_GuiHeight

map_origin_x := window_width  / 2
map_origin_y := window_height / 2

script_hbm_mem := DllCall("CreateCompatibleBitmap", "UInt", script_hdc, "Int", window_width, "Int", window_height)
Return

DrawMap()
{
    If (map_zoom = MAP_ZOOM_MIN)
    {
        font_size := FONT_SIZE_DEFAULT * 2
    }
    Else
    {
        font_size := FONT_SIZE_DEFAULT
    }

    DllCall("SelectObject", "UInt", script_hdc_mem, "UInt", script_hbm_mem)

    DllCall("BitBlt", "UInt", script_hdc_mem, "Int", 0, "Int", 0, "Int", window_width, "Int", window_height, "UInt", 0, "Int", 0, "Int", 0, "Int", BLACKNESS)

    DllCall("QueryPerformanceFrequency", "Int64 *", QueryPerformanceCounter_frequency)
    DllCall("QueryPerformanceCounter", "Int64 *", QueryPerformanceCounter_before)

    DrawMapLines()
    DrawMapSpawns()
    DrawMapZoneLinePoints()

    DllCall("QueryPerformanceCounter", "Int64 *", QueryPerformanceCounter_after)

    DrawMapInfoText()

    DllCall("BitBlt", "UInt", script_hdc, "Int", 0, "Int", 0, "Int", window_width, "Int", window_height, "UInt", script_hdc_mem, "Int", 0, "Int", 0, "Int", SRCCOPY)
}

DrawMapInfoText()
{
    If (map_zoom = MAP_ZOOM_MIN)
    {
        Return
    }

    player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

    player_y := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y)
    player_x := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X)
    player_z := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z)

    player_heading := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING)

    DllCall("SelectObject", "UInt", script_hdc_mem, "UInt", script_font)

    DllCall("SetTextColor", "UInt", script_hdc_mem, "Int", COLOR_PINK)

    query_peformance := QueryPerformanceCounter_after - QueryPerformanceCounter_before

    info_text = Lines: %num_lines%`nSpawns: %num_spawns%`nPoints: %num_points%`nQueryPerformanceCounter: %query_peformance%`n`nResolution: %window_width%x%window_height%`nZoom: %map_zoom%`n`nPlayer Location: %player_y%, %player_x%, %player_z%`nPlayer Heading: %player_heading%

    Loop, Parse, info_text, `n, `r
    {
        info_text_line = %A_LoopField%

        DllCall("ExtTextOut", "UInt", script_hdc_mem, "Int", 0, "Int", 0 + ((A_Index - 1) * font_size), "UInt", 0, "UInt", 0, "Ptr", &info_text_line, "Int", StrLen(info_text_line), "Int", 0)
    }
}

DrawMapLines()
{
    player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

    player_y := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y)
    player_x := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X)

    map_draw_x := player_x
    map_draw_y := player_y

    num_lines := 0

    For map_lines_index, map_lines_value In map_lines
    {
        line_start_x := map_lines_value[1]
        line_start_y := map_lines_value[2]

        line_stop_x  := map_lines_value[3]
        line_stop_y  := map_lines_value[4]

        line_map_start_x := ((line_start_x / map_zoom) + map_origin_x) + ((map_draw_x + map_offset_x) / map_zoom)
        line_map_start_y := ((line_start_y / map_zoom) + map_origin_y) + ((map_draw_y + map_offset_y) / map_zoom)

        line_map_stop_x := ((line_stop_x / map_zoom) + map_origin_x) + ((map_draw_x + map_offset_x) / map_zoom)
        line_map_stop_y := ((line_stop_y / map_zoom) + map_origin_y) + ((map_draw_y + map_offset_y) / map_zoom)

        line_map_distance_start := CalculateDistance(map_origin_x, map_origin_y, line_map_start_x, line_map_start_y)
        line_map_distance_stop  := CalculateDistance(map_origin_x, map_origin_y, line_map_stop_x,  line_map_stop_y)

        If (line_map_distance_start > map_origin_x && line_map_distance_stop > map_origin_x && line_map_distance_start > map_origin_y && line_map_distance_stop > map_origin_y)
        {
            Continue
        }

        If (num_lines_ex >= 100)
        {
            line_max_range := 0 ;100
        }
        Else
        {
            line_max_range := 0
        }

        If (line_max_range > 0)
        {
            If (line_map_distance_start >= line_max_range && line_map_distance_stop >= line_max_range)
            {
                Continue
            }
        }

        line_color := map_lines_value[5]

        Critical
        DrawLine(line_map_start_x, line_map_start_y, line_map_stop_x, line_map_stop_y, line_color)
        Critical, Off

        num_lines := num_lines + 1
    }

    num_lines_ex := num_lines
}

DrawMapSpawns()
{
    DllCall("SelectObject", "UInt", script_hdc_mem, "UInt", script_font)

    target_spawn_info := everquest_GetTargetSpawnInfoPointer()

    player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

    player_y := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y)
    player_x := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X)
    player_z := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Z)

    player_map_x := map_origin_x + (map_offset_x / map_zoom)
    player_map_y := map_origin_y + (map_offset_y / map_zoom)

    player_avatar_height := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_AVATAR_HEIGHT)

    player_level := Memory_ReadEx(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_LEVEL, 1)

    player_heading := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING)

    spawn_info_begin := Memory_Read(everquest_process_handle, EVERQUEST_SPAWN_INFO_BEGIN_POINTER)

    spawn_info_address := spawn_info_begin

    num_spawns := 0

    Loop
    {
        spawn_next_spawn_info := Memory_Read(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER)

        If (spawn_next_spawn_info = EVERQUEST_SPAWN_INFO_NULL)
        {
            Break
        }

        spawn_is_gm := Memory_ReadEx(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_IS_GM, 1)

        spawn_y := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Y)
        spawn_x := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_X)
        spawn_z := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_Z)

        If (spawn_max_range > 0)
        {
            spawn_distance := CalculateDistance(player_x, player_y, spawn_x, spawn_y)

            If (spawn_distance >= spawn_max_range)
            {
                spawn_info_address := spawn_next_spawn_info

                Continue
            }
        }

        spawn_y := ReverseSign(spawn_y)
        spawn_x := ReverseSign(spawn_x)

        spawn_map_x := ((spawn_x / map_zoom) + map_origin_x) + (player_x / map_zoom) + (map_offset_x / map_zoom)
        spawn_map_y := ((spawn_y / map_zoom) + map_origin_y) + (player_y / map_zoom) + (map_offset_y / map_zoom)

        spawn_map_distance := CalculateDistance(map_origin_x, map_origin_y, spawn_map_x, spawn_map_y)

        If (spawn_info_address != target_spawn_info)
        {
            If (!spawn_is_gm)
            {
                ;IfNotInString, spawn_name, golden beast
                ;{
                    If (spawn_map_distance > map_origin_x && spawn_map_distance > map_origin_y)
                    {
                        spawn_info_address := spawn_next_spawn_info

                        Continue
                    }
                ;}
            }
        }

        spawn_heading := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HEADING)

        spawn_name      := Memory_ReadString(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_NAME,      0x40)
        spawn_last_name := Memory_ReadString(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LAST_NAME, 0x20)

        spawn_avatar_height := Memory_ReadFloat(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_AVATAR_HEIGHT)

        spawn_level := Memory_ReadEx(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_LEVEL, 1)

        spawn_race  := Memory_Read(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_RACE)

        spawn_class := Memory_ReadEx(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_CLASS, 1)

        spawn_type := Memory_ReadEx(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_TYPE, 1)

        spawn_body_type := Memory_Read(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_BODY_TYPE)

        spawn_guild_id := Memory_Read(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_GUILD_ID)

        spawn_is_afk := Memory_ReadEx(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_IS_AFK, 1)

        spawn_is_lfg := Memory_ReadEx(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_IS_LFG, 1)

        spawn_is_linkdead := Memory_ReadEx(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_IS_LINKDEAD, 1)

        spawn_master_id := Memory_Read(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_MASTER_ID)

        spawn_hp_current := Memory_Read(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_CURRENT)

        spawn_hp_max := Memory_Read(everquest_process_handle, spawn_info_address + EVERQUEST_OFFSET_SPAWN_INFO_HP_MAX)

        z_type := "="

        If (spawn_z = player_z)
        {
            z_type := "="
        }

        If (spawn_z > (player_z + player_avatar_height + 1))
        {
            z_type := "+"
        }

        If ((spawn_z + spawn_avatar_height + 1) < player_z)
        {
            z_type := "-"
        }

        spawn_text_color := COLOR_BLACK

        If (spawn_level < player_level)
        {
            spawn_text_color := COLOR_GREEN_DARK
        }

        If (spawn_level <= (player_level + 4))
        {
            If (spawn_level >= (player_level - 4))
            {
                spawn_text_color := COLOR_BLUE
            }
        }

        If (spawn_level > (player_level + 5))
        {
            spawn_text_color := COLOR_OLIVE
        }

        If (spawn_level > (player_level + 10))
        {
            spawn_text_color := COLOR_RED
        }

        If (spawn_level >= SOD_MAX_LEVEL)
        {
            spawn_text_color := COLOR_RED
        }

        found_special_map_object := False

        found_portal := False

        IfInString, spawn_name, Portal
        {
            found_special_map_object := True

            found_portal := True
        }

        found_trash_barrel := False

        IfInString, spawn_name, Trash Barrel
        {
            found_special_map_object := True

            found_trash_barrel := True
        }

        found_eye_of_dreams := False

        IfInString, spawn_name, Eye of Dreams
        {
            found_special_map_object := True

            found_eye_of_dreams := True
        }

        IfInString, spawn_name, Artificier
        {
            found_special_map_object := True
        }

        IfInString, spawn_name, Healer
        {
            found_special_map_object := True
        }

        IfInString, spawn_name, Priest of War
        {
            found_special_map_object := True
        }

        IfInString, spawn_name, Priest of Harmony
        {
            found_special_map_object := True
        }

        IfInString, spawn_name, Lottery Master
        {
            found_special_map_object := True
        }

        IfInString, spawn_name, Gambler
        {
            found_special_map_object := True
        }

        If spawn_last_name = Shopkeeper
        {
            found_special_map_object := True
        }

        If spawn_last_name = Banker
        {
            found_special_map_object := True
        }

        If spawn_last_name = Adept
        {
            found_special_map_object := True
        }

        IfInString, spawn_last_name, Guildmaster
        {
            found_special_map_object := True
        }

        If (found_special_map_object)
        {
            spawn_text_color := COLOR_ORANGE
        }

        found_golden_beast := False

        IfInString, spawn_name, golden beast
        {
            found_golden_beast := True
        }

        If (found_golden_beast)
        {
            spawn_text_color := COLOR_YELLOW
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
            spawn_text_color := COLOR_TEAL
        }

        spawn_text := ""

        spawn_text := spawn_text . z_type

        spawn_text := spawn_text . " "

        spawn_text = %spawn_text%%spawn_name%

        IfInString, spawn_last_name, *RP*
        {
            spawn_text := spawn_text . " "

            spawn_text = %spawn_text%*RP*
        }

        IfInString, spawn_last_name, *HC*
        {
            spawn_text := spawn_text . " "

            spawn_text = %spawn_text%*HC*
        }

        If (spawn_is_afk)
        {
            spawn_text := spawn_text . " "

            spawn_text = %spawn_text%AFK
        }

        If (spawn_is_lfg)
        {
            spawn_text := spawn_text . " "

            spawn_text = %spawn_text%LFG
        }

        If (spawn_is_linkdead)
        {
            spawn_text := spawn_text . " "

            spawn_text = %spawn_text%LD
        }

        If (spawn_is_gm)
        {
            spawn_text := spawn_text . " "

            spawn_text = %spawn_text%GM
        }

        If (spawn_master_id > 0)
        {
            spawn_text := spawn_text . " "

            spawn_text = %spawn_text%PET
        }

        spawn_text := spawn_text . " "

        spawn_text = %spawn_text%(

        spawn_text = %spawn_text%L%spawn_level%

        If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
        {
            spawn_text := spawn_text . " "

            spawn_race_short_name := everquest_GetRaceShortName(spawn_race)

            spawn_text = %spawn_text%%spawn_race_short_name%

            spawn_text := spawn_text . " "

            spawn_class_short_name := everquest_GetClassShortName(spawn_class)

            spawn_text = %spawn_text%%spawn_class_short_name%
        }

        spawn_text = %spawn_text%)

        spawn_text_ex := ""

        If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
        {
            If (spawn_guild_id != EVERQUEST_GUILD_ID_NULL)
            {
                spawn_guild_name := everquest_GetGuildName(spawn_guild_id)

                spawn_text_ex = <%spawn_guild_name%>
            }
        }

        If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_NPC)
        {
            If (spawn_class > EVERQUEST_CLASS_LAST)
            {
                spawn_text_ex = (%spawn_last_name%)
            }
            Else
            {
                IfInString, spawn_last_name, Adept
                {
                    spawn_text_ex = (Adept)

                    spawn_text_ex := spawn_text_ex . " "
                }

                spawn_text_ex = %spawn_text_ex%[HP: %spawn_hp_current%`%]
            }
        }

        draw_spawn_name := True

        If (num_spawns_ex > 100)
        {
            draw_spawn_name := False
        }

        If (spawn_info_address = target_spawn_info)
        {
            draw_spawn_name := True
        }

        If (found_portal || found_golden_beast || found_admin)
        {
            draw_spawn_name := True
        }

        If (draw_spawn_name)
        {
            DllCall("SetTextColor", "UInt", script_hdc_mem, "Int", spawn_text_color)
            DllCall("ExtTextOut", "UInt", script_hdc_mem, "Int", spawn_map_x, "Int", spawn_map_y,             "UInt", 0, "UInt", 0, "Ptr", &spawn_text,    "Int", StrLen(spawn_text),    "Int", 0)
            DllCall("ExtTextOut", "UInt", script_hdc_mem, "Int", spawn_map_x, "Int", spawn_map_y + font_size, "UInt", 0, "UInt", 0, "Ptr", &spawn_text_ex, "Int", StrLen(spawn_text_ex), "Int", 0)
        }

        If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_PLAYER)
        {
            DrawSquare(spawn_map_x, spawn_map_y, spawn_text_color)
        }
        Else
        {
            If (spawn_type = EVERQUEST_SPAWN_INFO_TYPE_PLAYER_CORPSE || spawn_type = EVERQUEST_SPAWN_INFO_TYPE_NPC_CORPSE)
            {
                DrawCross(spawn_map_x, spawn_map_y, spawn_text_color)
            }
            Else
            {
                DrawCircle(spawn_map_x, spawn_map_y, spawn_text_color)
            }
        }

        If (spawn_info_address = target_spawn_info)
        {
            DrawArrow(player_map_x, player_map_y, spawn_map_x, spawn_map_y, COLOR_GREEN)

            spawn_arrow_size := 16

            DrawArrowByHeading(spawn_map_x, spawn_map_y, spawn_heading, spawn_text_color, spawn_arrow_size)
        }

        If (found_portal)
        {
            DrawArrow(player_map_x, player_map_y, spawn_map_x, spawn_map_y, COLOR_ORANGE)
        }

        If (found_golden_beast)
        {
            DrawArrow(player_map_x, player_map_y, spawn_map_x, spawn_map_y, COLOR_YELLOW)
        }

        If (found_admin)
        {
            DrawArrow(player_map_x, player_map_y, spawn_map_x, spawn_map_y, COLOR_TEAL)
        }

        num_spawns := num_spawns + 1

        spawn_info_address := spawn_next_spawn_info
    }

    num_spawns_ex := num_spawns

    DllCall("SelectObject", "UInt", script_hdc_mem, "UInt", script_font_bold)

    player_text = @ Player ;(%player_level%)

    player_text_color := COLOR_PINK

    DllCall("SetTextColor", "UInt", script_hdc_mem, "Int", player_text_color)
    DllCall("ExtTextOut", "UInt", script_hdc_mem, "Int", player_map_x, "Int", player_map_y, "UInt", 0, "UInt", 0, "Ptr", &player_text, "Int", StrLen(player_text), "Int", 0)

    DrawSquare(player_map_x, player_map_y, player_text_color)

    player_arrow_size := 16

    DrawArrowByHeading(player_map_x, player_map_y, player_heading, player_text_color, player_arrow_size)
}

DrawMapZoneLinePoints()
{
    DllCall("SelectObject", "UInt", script_hdc_mem, "UInt", script_font)

    player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

    player_y := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_Y)
    player_x := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_X)

    map_draw_x := player_x
    map_draw_y := player_y

    num_points := 0

    For map_points_index, map_points_value In map_points
    {
        point_x := map_points_value[1]
        point_y := map_points_value[2]
        ;point_z := map_points_value[3]

        point_map_x := ((point_x / map_zoom) + map_origin_x) + ((map_draw_x + map_offset_x) / map_zoom)
        point_map_y := ((point_y / map_zoom) + map_origin_y) + ((map_draw_y + map_offset_y) / map_zoom)

        point_map_distance := CalculateDistance(map_origin_x, map_origin_y, point_map_x, point_map_y)

        If (point_map_distance > map_origin_x && point_map_distance > map_origin_x)
        {
            Continue
        }

        point_color := COLOR_PURPLE ;map_points_value[4]

        point_text := map_points_value[5]

        StringLeft, point_text_first_characters, point_text, 2

        If point_text_first_characters = to
        {
            DllCall("SetTextColor", "UInt", script_hdc_mem, "Int", point_color)
            DllCall("ExtTextOut", "UInt", script_hdc_mem, "Int", point_map_x, "Int", point_map_y, "UInt", 0, "UInt", 0, "Ptr", &point_text, "Int", StrLen(point_text), "Int", 0)

            num_points := num_points + 1
        }
    }

    num_points_ex := num_points
}

DrawLine(from_x, from_y, to_x, to_y, color)
{
    DllCall("SelectObject", "UInt", script_hdc_mem, "UInt", script_pen)

    DllCall("SetDCPenColor", "UInt", script_hdc_mem, "Int", color)

    DllCall("MoveToEx", "UInt", script_hdc_mem, "UInt", from_x, "UInt", from_y, "UInt", lp_point)
    DllCall("LineTo",   "UInt", script_hdc_mem, "UInt", to_x,   "UInt", to_y)
}

DrawSquare(x, y, color)
{
    square_size := 4

    DllCall("SelectObject", "UInt", script_hdc_mem, "UInt", script_brush)

    DllCall("SetDCBrushColor", "UInt", script_hdc_mem, "Int", color)
    DllCall("SetDCPenColor",   "UInt", script_hdc_mem, "Int", color)

    DllCall("Rectangle", "UInt", script_hdc_mem, "UInt", x - (square_size / 2), "UInt", y - (square_size / 2), "UInt", x + (square_size / 2), "UInt", y + (square_size / 2))
}

DrawCircle(x, y, color)
{
    circle_size := 4

    DllCall("SelectObject", "UInt", script_hdc_mem, "UInt", script_brush)

    DllCall("SetDCBrushColor", "UInt", script_hdc_mem, "Int", color)
    DllCall("SetDCPenColor",   "UInt", script_hdc_mem, "Int", color)

    DllCall("Ellipse", "UInt", script_hdc_mem, "UInt", x - (circle_size / 2), "UInt", y - (circle_size / 2), "UInt", x + (circle_size / 2), "UInt", y + (circle_size / 2))
}

DrawCross(x, y, color)
{
    cross_size := 2

    DllCall("SelectObject", "UInt", script_hdc_mem, "UInt", script_pen)

    DllCall("SetDCPenColor", "UInt", script_hdc_mem, "Int", color)

    DllCall("MoveToEx", "UInt", script_hdc_mem, "UInt", x - cross_size,     "UInt", y - cross_size, "UInt", lp_point)
    DllCall("LineTo",   "UInt", script_hdc_mem, "UInt", x + cross_size + 1, "UInt", y + cross_size + 1)

    DllCall("MoveToEx", "UInt", script_hdc_mem, "UInt", x + cross_size,     "UInt", y - cross_size, "UInt", lp_point)
    DllCall("LineTo",   "UInt", script_hdc_mem, "UInt", x - cross_size - 1, "UInt", y + cross_size + 1)
}

DrawArrow(from_x, from_y, to_x, to_y, color)
{
    DllCall("SelectObject", "UInt", script_hdc_mem, "UInt", script_pen)

    DllCall("SetDCPenColor", "UInt", script_hdc_mem, "Int", color)

    DllCall("MoveToEx", "UInt", script_hdc_mem, "UInt", from_x, "UInt", from_y, "UInt", lp_point)
    DllCall("LineTo",   "UInt", script_hdc_mem, "UInt", to_x,   "UInt", to_y)

    arrow_head_size := 4

    DllCall("MoveToEx", "UInt", script_hdc_mem, "UInt", to_x, "UInt", to_y - arrow_head_size, "UInt", lp_point)
    DllCall("LineTo",   "UInt", script_hdc_mem, "UInt", to_x, "UInt", to_y + arrow_head_size)

    DllCall("MoveToEx", "UInt", script_hdc_mem, "UInt", to_x - arrow_head_size, "UInt", to_y, "UInt", lp_point)
    DllCall("LineTo",   "UInt", script_hdc_mem, "UInt", to_x + arrow_head_size, "UInt", to_y)
}

DrawArrowByHeading(x, y, heading, color, size)
{
    If (map_zoom < MAP_ZOOM_DEFAULT)
    {
        zoom := map_zoom
    }
    Else
    {
        zoom := MAP_ZOOM_DEFAULT
    }




    heading := (heading * 360) / 512 ; convert from 512 degrees to 360 degrees

    heading -= 90 ; rotate by -90 degrees

    If (heading < 0)
    {
        heading += 360
    }

    heading_radians := heading * PI / 180 ; convert degrees to radians

    arrow_add_y := Sin(heading_radians)
    arrow_add_x := Cos(heading_radians)

    arrow_y := y
    arrow_x := x

    arrow_length := size / zoom

    arrow_y += arrow_add_y * arrow_length
    arrow_x -= arrow_add_x * arrow_length

    DrawLine(x, y, arrow_x, arrow_y, color)




    heading_ex := heading

    heading_ex -= 30

    If (heading_ex < 0)
    {
        heading_ex += 360
    }

    heading_radians_ex := heading_ex * PI / 180 ; convert degrees to radians

    arrow_add_y_ex := Sin(heading_radians_ex)
    arrow_add_x_ex := Cos(heading_radians_ex)

    arrow_y_ex := arrow_y
    arrow_x_ex := arrow_x

    arrow_length_ex := (size / 2) / zoom

    arrow_y_ex -= arrow_add_y_ex * arrow_length_ex
    arrow_x_ex += arrow_add_x_ex * arrow_length_ex

    DrawLine(arrow_x, arrow_y, arrow_x_ex, arrow_y_ex, color)




    heading_ex := heading

    heading_ex += 30

    If (heading_ex > 360)
    {
        heading_ex := heading_ex - 360
    }

    heading_radians_ex := heading_ex * PI / 180 ; convert degrees to radians

    arrow_add_y_ex := Sin(heading_radians_ex)
    arrow_add_x_ex := Cos(heading_radians_ex)

    arrow_y_ex := arrow_y
    arrow_x_ex := arrow_x

    arrow_length_ex := (size / 2) / zoom

    arrow_y_ex -= arrow_add_y_ex * arrow_length_ex
    arrow_x_ex += arrow_add_x_ex * arrow_length_ex

    DrawLine(arrow_x, arrow_y, arrow_x_ex, arrow_y_ex, color)
}

ShowOptions:
Gui, 2: Show
Return

MapCenter:
map_offset_x := 0
map_offset_y := 0
Return

MapScrollUp:
map_offset_y += map_offset_multiplier * map_zoom
Return

MapScrollDown:
map_offset_y -= map_offset_multiplier * map_zoom
Return

MapScrollLeft:
map_offset_x += map_offset_multiplier * map_zoom
Return

MapScrollRight:
map_offset_x -= map_offset_multiplier * map_zoom
Return

MapZoomReset:
map_zoom := MAP_ZOOM_DEFAULT
Return

MapZoomIn:
If ((map_zoom - map_zoom_multiplier) < map_zoom_multiplier)
{
    map_zoom := MAP_ZOOM_MIN

    Return
}

If (map_zoom > MAP_ZOOM_DEFAULT)
{
    map_zoom := map_zoom - map_zoom_multiplier * map_zoom

    If (map_zoom < MAP_ZOOM_DEFAULT)
    {
        map_zoom := Round(map_zoom, map_zoom_multiplier)
    }
}
Else
{
    map_zoom := map_zoom - map_zoom_multiplier
}
Return

MapZoomOut:
If (map_zoom = MAP_ZOOM_MIN)
{
    map_zoom := map_zoom_multiplier

    Return
}

If (map_zoom > MAP_ZOOM_DEFAULT)
{
    map_zoom := map_zoom + map_zoom_multiplier * map_zoom
}
Else
{
    map_zoom := map_zoom + map_zoom_multiplier
}
Return

MapClick:
MouseGetPos, mouse_x, mouse_y

If (mouse_x < 0 || mouse_x > window_width)
{
    Return
}

If (mouse_y < 0 || mouse_y > window_height)
{
    Return
}

map_offset_x := map_offset_x + (map_origin_x - mouse_x) * map_zoom
map_offset_y := map_offset_y + (map_origin_y - mouse_y) * map_zoom

Return

TimerDrawMap:
DrawMap()
Return

TimerUpdateMapZone:
zone_short_name := everquest_GetZoneShortName()

zone_long_name := everquest_GetZoneLongName()

zone_short_name_live := everquest_GetZoneShortNameLive()

script_process_id := DllCall("GetCurrentProcessId")

WinSetTitle, ahk_pid %script_process_id%,,maps\%zone_short_name_live%.txt%A_Space%%zone_short_name%%A_Space%(%zone_long_name%)

If zone_short_name_live = %zone_short_name_live_global%
{
    Return
}

zone_short_name_live_global := zone_short_name_live

map_lines := ""

map_lines := Array()

map_points := ""

map_points := Array()

FileRead, map_file, c:\everquest\maps\%zone_short_name_live%.txt

If Not ErrorLevel
{
    Loop, Parse, map_file, `n, `r
    {
        ;MsgBox, %A_LoopField%

        If A_LoopField = 
        {
            Continue
        }

        map_file_line := A_LoopField

        ;MsgBox, map_file_line = %map_file_line%

        StringLeft, map_file_line_first_character, map_file_line, 1

        StringTrimLeft, map_file_line, map_file_line, 2

        StringReplace, map_file_line, map_file_line, %A_Space%,,All

        StringSplit, line_data, map_file_line, `,

        IfEqual, map_file_line_first_character, L
        {
            line_start_x = %line_data1%
            line_start_y = %line_data2%

            line_stop_x  = %line_data4%
            line_stop_y  = %line_data5%

            ; line points to itself
            If (line_start_x = line_stop_x && line_start_y = line_stop_y)
            {
                Continue
            }

            line_red   = %line_data7%
            line_green = %line_data8%
            line_blue  = %line_data9%

            SetFormat, IntegerFast, H

            line_blue  += 0
            line_green += 0
            line_red   += 0

            StringReplace, line_blue,  line_blue,  0x,,All
            StringReplace, line_green, line_green, 0x,,All
            StringReplace, line_red,   line_red,   0x,,All

            If line_blue = 0
            {
                line_blue = 00
            }

            If line_green = 0
            {
                line_green = 00
            }

            If line_red = 0
            {
                line_red = 00
            }

            line_color = 0x%line_blue%%line_green%%line_red%

            If (line_color = COLOR_BLACK)
            {
                line_color := COLOR_WHITE
            }

            ;line_color := 0xBBGGRR

            ;MsgBox, line_color = %line_color%

            SetFormat, IntegerFast, D

            map_line := Array()

            map_line := [line_start_x, line_start_y, line_stop_x, line_stop_y, line_color]

            map_lines.Insert(map_line)
        }

        IfEqual, map_file_line_first_character, P
        {
            point_x = %line_data1%
            point_y = %line_data2%
            point_z  = %line_data3%

            point_red   = %line_data4%
            point_green = %line_data5%
            point_blue  = %line_data6%

            point_size  = %line_data7%

            point_text = %line_data8%

            StringReplace, point_text, point_text,_,%A_Space%,All

            SetFormat, IntegerFast, H

            point_blue  += 0
            point_green += 0
            point_red   += 0

            StringReplace, point_blue,  point_blue,  0x,,All
            StringReplace, point_green, point_green, 0x,,All
            StringReplace, point_red,   point_red,   0x,,All

            If point_blue = 0
            {
                point_blue = 00
            }

            If point_green = 0
            {
                point_green = 00
            }

            If point_red = 0
            {
                point_red = 00
            }

            point_color = 0x%point_blue%%point_green%%point_red%

            If (point_color = COLOR_BLACK)
            {
                point_color := COLOR_WHITE
            }

            ;point_color := 0xBBGGRR

            ;MsgBox, point_color = %point_color%

            SetFormat, IntegerFast, D

            map_point := Array()

            map_point := [point_x, point_y, point_z, point_color, point_text]

            map_points.Insert(map_point)
        }
    }
}

;For map_lines_index, map_lines_value In map_lines
;{
;    MsgBox, % "1: " map_lines_value[1] ", 5: " map_lines_value[5]
;}

;For map_lines_index, map_lines_value In map_lines
;{
;    For map_line_index, map_line_value In map_lines_value
;    {
;        MsgBox % "Item " map_line_index " is '" map_line_value "'"
;    }
;}
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
;        If (winmgmts_process_id = active_process_id)
;        {
;            everquest_process_id := active_process_id
;
;            everquest_process_handle := Memory_GetProcessHandle(everquest_process_id)
;        }
;    }
;}

Return

GuiClose:
GuiEscape:

DllCall("DeleteObject", "UInt", script_brush)
DllCall("DeleteObject", "UInt", script_pen)
DllCall("DeleteObject", "UInt", script_font)
DllCall("DeleteObject", "UInt", script_font_bold)
DllCall("DeleteObject", "UInt", script_hbm_mem)

DllCall("ReleaseDC", "UInt", script_hwnd, "UInt", script_hdc_mem)
DllCall("ReleaseDC", "UInt", script_hwnd, "UInt", script_hdc)

DllCall("DeleteDC", "UInt", script_hdc_mem)
DllCall("DeleteDC", "UInt", script_hdc)

DllCall("FreeLibrary", "Ptr", psapi_dll_handle)

Memory_CloseHandle(everquest_process_handle)

ExitApp

Return
