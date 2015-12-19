#SingleInstance, On
#Persistent

#Include, C:\AutoHotkey\Lib\Memory.ahk

#Include, everquest.ahk

Global psapi_dll_handle := 0

Global ini_file := "everquest_options.ini"

Global buffs_file := 

Gui, Add, Text,, Process:
Gui, Add, DropDownList, vDropDownListProcesses w320 Choose1, Dynamic||
Gui, Add, Button, vButtonRefreshProcesses gButtonRefreshProcesses Default, Refresh &Processes
Gui, Add, Text,,
Gui, Add, Checkbox, vCheckboxFieldOfView gCheckboxFieldOfView, Field of View
Gui, Add, Text, vTextFirstPersonFov w320, First Person:%A_Tab%%EVERQUEST_CAMERA_FIELD_OF_VIEW_FIRST_PERSON_DEFAULT%
Gui, Add, Slider, vSliderFirstPersonFov gSliderFirstPersonFov AltSubmit Line1 Page10 TickInterval5 Range%EVERQUEST_FIELD_OF_VIEW_MIN%-%EVERQUEST_FIELD_OF_VIEW_MAX% ToolTip w320, %EVERQUEST_CAMERA_FIELD_OF_VIEW_FIRST_PERSON_DEFAULT%
Gui, Add, Text, vTextThirdPersonFov w320, Third Person:%A_Tab%%EVERQUEST_CAMERA_FIELD_OF_VIEW_THIRD_PERSON_DEFAULT%
Gui, Add, Slider, vSliderThirdPersonFov gSliderThirdPersonFov AltSubmit Line1 Page10 TickInterval5 Range%EVERQUEST_FIELD_OF_VIEW_MIN%-%EVERQUEST_FIELD_OF_VIEW_MAX% ToolTip w320, %EVERQUEST_CAMERA_FIELD_OF_VIEW_THIRD_PERSON_DEFAULT%
Gui, Add, Text,,
Gui, Add, Checkbox, vCheckboxDrawDistance gCheckboxDrawDistance, Draw Distance
Gui, Add, Text, vTextDrawDistance w320, Draw Distance:%A_Tab%%EVERQUEST_DRAW_DISTANCE_MAX%
Gui, Add, Slider, vSliderDrawDistance gSliderDrawDistance AltSubmit Line1 Page100 TickInterval1000 Range%EVERQUEST_DRAW_DISTANCE_MIN%-%EVERQUEST_DRAW_DISTANCE_MAX% ToolTip w320, %EVERQUEST_DRAW_DISTANCE_MAX%
Gui, Add, Text,,
Gui, Add, Checkbox, vCheckboxTurnAround, Press END to Turn 180 Degrees
Gui, Add, Checkbox, vCheckboxRemoveRoot, Press DEL to Remove Root Debuffs
Gui, Add, Text,,
Gui, Add, Checkbox, vCheckboxSwimFast gCheckboxSwimFast, Swim Fast
Gui, Add, Text,,
Gui, Add, Checkbox, vCheckboxSpeedHack, Speed Hack
Gui, Add, Text,, Speed Multiplier:
Gui, Add, DropDownList, vDropDownListSpeedModifier w320 Choose1, 1x||1.1x|1.25x|1.5x|1.75x|2x|4x|6x|8x|10x ;|100x|1000x|9999x
Gui, Add, Checkbox, vCheckboxScripts ym, Scripts
Gui, Add, ListView, vListViewScripts +HwndListViewHwndScripts VScroll Grid -Multi Checked NoSortHdr w640 h480, Enabled|Name|Description|Enable|Disable|

FieldOfView_Enable()
{
    Memory_WriteNops(everquest_process_handle, EVERQUEST_ASM_SET_CAMERA_FIELD_OF_VIEW, EVERQUEST_ASM_SET_CAMERA_FIELD_OF_VIEW_SIZE)
}

FieldOfView_Disable()
{
    Memory_WriteBytes(everquest_process_handle, EVERQUEST_ASM_SET_CAMERA_FIELD_OF_VIEW, EVERQUEST_ASM_SET_CAMERA_FIELD_OF_VIEW_BYTES)
}

DrawDistance_Enable()
{
    Memory_WriteNops(everquest_process_handle, EVERQUEST_ASM_SET_CAMERA_DRAW_DISTANCE, EVERQUEST_ASM_SET_CAMERA_DRAW_DISTANCE_SIZE)
}

DrawDistance_Disable()
{
    Memory_WriteBytes(everquest_process_handle, EVERQUEST_ASM_SET_CAMERA_DRAW_DISTANCE, EVERQUEST_ASM_SET_CAMERA_DRAW_DISTANCE_BYTES)
}

UpdateListViewColumnsScripts()
{
    Gui, ListView, ListViewScripts

    LV_ModifyCol()
    LV_ModifyCol(1, "AutoHdr")
    LV_ModifyCol(2, "AutoHdr")
    LV_ModifyCol(3, "AutoHdr")
    LV_ModifyCol(4, "AutoHdr")
    LV_ModifyCol(5, "AutoHdr")
}

UpdateListViewScripts()
{
    GuiControl, -Redraw, ListViewScripts

    Gui, ListView, ListViewScripts
    LV_Delete()

    FileRead, options_file, %ini_file%

    found_scripts := False

    Loop, Parse, options_file, `n, `r
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

        IfInString, A_LoopField, [Scripts]
        {
            found_scripts := True
            
            Continue
        }

        IfInString, A_LoopField, [/Scripts]
        {
            found_scripts := False
            
            Continue
        }

        If (found_scripts)
        {
            StringSplit, script_data, A_LoopField, :

            script_file_path = %A_ScriptDir%\scripts\%script_data2%.ini

            IfExist, %script_file_path%
            {
                script_is_enabled = %script_data1%

                IniRead, script_name, %script_file_path%, Script, Name

                IniRead, script_description, %script_file_path%, Script, Description

                FileRead, script_file, %script_file_path%

                found_enable  := False
                found_disable := False

                instructions_enable = 
                instructions_disable = 

                Loop, Parse, script_file, `n, `r
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

                    IfInString, A_LoopField, [Enable]
                    {
                        found_enable  := True
                        found_disable := False

                        Continue
                    }

                    IfInString, A_LoopField, [Disable]
                    {
                        found_enable  := False
                        found_disable := True

                        Continue
                    }

                    If (found_enable)
                    {
                        instructions_enable = %instructions_enable%%A_LoopField%^
                    }

                    If (found_disable)
                    {
                        instructions_disable = %instructions_disable%%A_LoopField%^
                    }
                }

                StringRight, find_text, instructions_enable, 1
                If find_text = ^
                {
                    StringTrimRight, instructions_enable, instructions_enable, 1
                }

                StringRight, find_text, instructions_disable, 1
                If find_text = ^
                {
                    StringTrimRight, instructions_disable, instructions_disable, 1
                }

                LV_Add("Check" . %script_is_enabled%, "", script_name, script_description, instructions_enable, instructions_disable)
            }
        }
    }

    GuiControl, +Redraw, ListViewScripts
}

EnableDebugPrivileges()

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

SetTimer, TimerUpdateProcess, 100

IniRead, scripts, %ini_file%, Options, Scripts
If (scripts > -1)
{
    GuiControl,, CheckboxScripts, %scripts%
}

IniRead, turn_around, %ini_file%, Options, TurnAround
If (turn_around > -1)
{
    GuiControl,, CheckboxTurnAround, %turn_around%
}

IniRead, remove_root, %ini_file%, Options, RemoveRoot
If (remove_root > -1)
{
    GuiControl,, CheckboxRemoveRoot, %remove_root%
}

IniRead, swim_fast, %ini_file%, Options, SwimFast
If (swim_fast > -1)
{
    GuiControl,, CheckboxSwimFast, %swim_fast%
}

IniRead, field_of_view, %ini_file%, FieldOfView, Enabled
If (field_of_view > -1)
{
    GuiControl,, CheckboxFieldOfView, %field_of_view%

    If (field_of_view > 0)
    {
        FieldOfView_Enable()
    }
}

IniRead, first_person_field_of_view, %ini_file%, FieldOfView, FirstPerson
If (first_person_field_of_view > -1)
{
    If (first_person_field_of_view >= EVERQUEST_FIELD_OF_VIEW_MIN)
    {
        If (first_person_field_of_view <= EVERQUEST_FIELD_OF_VIEW_MAX)
        {
            GuiControl,, TextFirstPersonFov, First Person:%A_Tab%%first_person_field_of_view%
            GuiControl,, SliderFirstPersonFov, %first_person_field_of_view%
        }
    }
}

IniRead, third_person_field_of_view, %ini_file%, FieldOfView, ThirdPerson
If (third_person_field_of_view > -1)
{
    If (third_person_field_of_view >= EVERQUEST_FIELD_OF_VIEW_MIN)
    {
        If (third_person_field_of_view <= EVERQUEST_FIELD_OF_VIEW_MAX)
        {
            GuiControl,, TextThirdPersonFov, Third Person:%A_Tab%%third_person_field_of_view%
            GuiControl,, SliderThirdPersonFov, %third_person_field_of_view%
        }
    }
}

IniRead, draw_distance, %ini_file%, DrawDistance, Enabled
If (draw_distance > -1)
{
    GuiControl,, CheckboxDrawDistance, %draw_distance%

    If (draw_distance > 0)
    {
        DrawDistance_Enable()
    }
}

IniRead, draw_distance, %ini_file%, DrawDistance, Distance
If (draw_distance > -1)
{
    If (draw_distance >= EVERQUEST_DRAW_DISTANCE_MIN)
    {
        If (draw_distance <= EVERQUEST_DRAW_DISTANCE_MAX)
        {
            GuiControl,, TextDrawDistance, Draw Distance:%A_Tab%%draw_distance%
            GuiControl,, SliderDrawDistance, %draw_distance%
        }
    }
}

IniRead, speed_modifier, %ini_file%, SpeedHack, Modifier
If (speed_modifier > -1)
{
    GuiControl, ChooseString, DropDownListSpeedModifier, %speed_modifier%
}

UpdateListViewScripts()
UpdateListViewColumnsScripts()

SetTimer, TimerFieldOfView, 1
SetTimer, TimerDrawDistance, 1

SetTimer, TimerSwimFast, 100 ; settings to 1 causes checkbox is_checked problem

SetTimer, TimerSpeedHack, 1

SetTimer, TimerScripts, 1

SetTimer, TimerUpdateTitle, 100

Gui, Show, Center, o

Return

SliderFirstPersonFov:
GuiControl,, TextFirstPersonFov, First Person:%A_Tab%%SliderFirstPersonFov%
Return

SliderThirdPersonFov:
GuiControl,, TextThirdPersonFov, Third Person:%A_Tab%%SliderThirdPersonFov%
Return

SliderDrawDistance:
GuiControl,, TextDrawDistance, Draw Distance:%A_Tab%%SliderDrawDistance%
Return

CheckboxFieldOfView:
GuiControlGet, is_checked,, CheckboxFieldOfView

If (is_checked = 0)
{
    FieldOfView_Disable()
}
Else
{
    FieldOfView_Enable()
}
Return

CheckboxDrawDistance:
GuiControlGet, is_checked,, CheckboxDrawDistance

If (is_checked = 0)
{
    DrawDistance_Disable()
}
Else
{
    DrawDistance_Enable()
}
Return

Delete::
FileRead, buffs_file, %A_ScriptDir%\data\buffs.txt

char_info := everquest_GetCharInfoPointer()

buffs_structure := char_info + EVERQUEST_OFFSET_CHAR_INFO_BUFFS_STRUCTURE

buffs_structure_address := buffs_structure

Loop, %EVERQUEST_BUFFS_MAX%
{
    buff_id := Memory_Read(everquest_process_handle, buffs_structure_address + EVERQUEST_OFFSET_BUFFS_STRUCTURE_SPELL_ID)

    If (buff_id = EVERQUEST_BUFF_SPELL_ID_NULL)
    {
        buffs_structure_address += EVERQUEST_BUFFS_STRUCTURE_BUFF_SIZE

        Continue
    }

    buff_type := -1

    Loop, Parse, buffs_file, `n, `r
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

        StringSplit, buffs_file_data, A_LoopField, ^

        If buff_id = %buffs_file_data1%
        {
            buff_type = %buffs_file_data3%

            Break
        }
    }

    If (buff_type = EVERQUEST_BUFF_TYPE_DETRIMENTAL)
    {
        Memory_Write(everquest_process_handle, buffs_structure_address + EVERQUEST_OFFSET_BUFFS_STRUCTURE_DURATION, 1)
    }

    buffs_structure_address += EVERQUEST_BUFFS_STRUCTURE_BUFF_SIZE
}
Return

End::
GuiControlGet, is_checked,, CheckboxTurnAround
If (is_checked = 0)
{
    Return
}

player_spawn_info := everquest_GetPlayerSpawnInfoPointer()

player_heading := Memory_ReadFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING)

player_heading := player_heading + (EVERQUEST_CAMERA_HEADING_MAX / 2)

If (player_heading > EVERQUEST_CAMERA_HEADING_MAX)
{
    player_heading := player_heading - EVERQUEST_CAMERA_HEADING_MAX
}

Memory_WriteFloat(everquest_process_handle, player_spawn_info + EVERQUEST_OFFSET_SPAWN_INFO_HEADING, player_heading)

Sleep, 1000

Return

CheckboxSwimFast:
GuiControlGet, is_checked,, CheckboxSwimFast
If (is_checked = 0)
{
    player_actor_info := everquest_GetPlayerActorInfoPointer()

    Memory_WriteEx(everquest_process_handle, player_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_AFFECTED_BY_GRAVITY, 0x01, 1)
}
Return

TimerSwimFast:
GuiControlGet, is_checked,, CheckboxSwimFast
If (is_checked = 0)
{
    Return
}

window_is_focused := Memory_ReadEx(everquest_process_handle, EVERQUEST_WINDOW_IS_FOCUSED, 1)

If (window_is_focused = 0)
{
    Return
}

player_actor_info := everquest_GetPlayerActorInfoPointer()

player_is_swimming := Memory_ReadEx(everquest_process_handle, player_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_SWIMMING, 1)

If (player_is_swimming = EVERQUEST_IS_SWIMMING_TRUE)
{
    Memory_WriteEx(everquest_process_handle, player_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_AFFECTED_BY_GRAVITY, 0x00, 1)
}
Else
{
    Memory_WriteEx(everquest_process_handle, player_actor_info + EVERQUEST_OFFSET_ACTOR_INFO_IS_AFFECTED_BY_GRAVITY, 0x01, 1)
}

Return

TimerFieldOfView:
GuiControlGet, is_checked,, CheckboxFieldOfView
If (is_checked = 0)
{
    FieldOfView_Disable()

    Return
}

FieldOfView_Enable()

camera_view := Memory_Read(everquest_process_handle, EVERQUEST_CAMERA_VIEW)

camera := everquest_GetCameraPointer()

If (camera_view = EVERQUEST_CAMERA_VIEW_FIRST_PERSON)
{
    GuiControlGet, field_of_view,, SliderFirstPersonFov

    Memory_WriteFloat(everquest_process_handle, camera + EVERQUEST_OFFSET_CAMERA_FIELD_OF_VIEW, field_of_view)
}
Else
{
    GuiControlGet, field_of_view,, SliderThirdPersonFov

    Memory_WriteFloat(everquest_process_handle, camera + EVERQUEST_OFFSET_CAMERA_FIELD_OF_VIEW, field_of_view)
}
Return

TimerDrawDistance:
GuiControlGet, is_checked,, CheckboxDrawDistance
If (is_checked = 0)
{
    DrawDistance_Disable()

    Return
}

DrawDistance_Enable()

camera := everquest_GetCameraPointer()

GuiControlGet, draw_distance,, SliderDrawDistance

Memory_WriteFloat(everquest_process_handle, camera + EVERQUEST_OFFSET_CAMERA_DRAW_DISTANCE, draw_distance)

Return

TimerSpeedHack:
GuiControlGet, is_checked,, CheckboxSpeedHack
If (is_checked = 0)
{
    Memory_WriteFloat(everquest_process_handle, EVERQUEST_SPEED_HACK_SPEED_MODIFIER, 1.0)

    Return
}

GuiControlGet, speed_modifier_text,, DropDownListSpeedModifier

StringTrimRight, speed_modifier, speed_modifier_text, 1

Memory_WriteFloat(everquest_process_handle, EVERQUEST_SPEED_HACK_SPEED_MODIFIER, speed_modifier)

Return

TimerScripts:
GuiControlGet, is_checked,, CheckboxScripts
If (is_checked = 0)
{
    Return
}

Gui, ListView, ListViewScripts

Loop % LV_GetCount()
{
    ; http://l.autohotkey.net/docs/commands/ListView.htm#bifRow
    Gui +LastFound
    SendMessage, 4140, A_Index - 1, 0xF000, SysListView321
    is_checked := (ErrorLevel >> 12) - 1

    LV_GetText(script_name,        A_Index, 2) ; Name
    LV_GetText(script_description, A_Index, 3) ; Description

    If (is_checked = 1)
    {
        script_instructions_column := 4 ; Enable
    }
    Else
    {
        script_instructions_column := 5 ; Disable
    }

    LV_GetText(script_instructions, A_Index, script_instructions_column)

    ;MsgBox, row_number = %A_Index%`nis_checked = %is_checked%`nscript_name = %script_name%`nscript_description = %script_description%`nscript_instructions = %script_instructions%

    StringSplit, script_instructions, script_instructions, ^

    Loop, %script_instructions0%
    {
        script_instruction := script_instructions%A_Index%
    
        ;MsgBox, script_instruction = %script_instruction%
    
        StringSplit, script_instruction_data, script_instruction, :

        IfEqual, script_instruction_data1, pointer
        {
            script_instruction_pointer = %script_instruction_data2%

            script_instruction_pointer = 0x%script_instruction_pointer% ; pre-append 0x

            ;MsgBox, script_instruction_pointer = %script_instruction_pointer%

            script_instruction_address := Memory_Read(everquest_process_handle, script_instruction_pointer)

            ;MsgBox, script_instruction_address = %script_instruction_address%

            script_instruction_offsets = %script_instruction_data4%

            num_offsets := 0
            Loop, Parse, script_instruction_offsets, `,
            {
                num_offsets += 1
            }

            ;MsgBox, num_offsets = %num_offsets%

            StringSplit, script_instruction_offsets, script_instruction_offsets, `,

            current_offset := 1
            Loop, %script_instruction_offsets0%
            {
                script_instruction_offset := script_instruction_offsets%A_Index%

                script_instruction_offset = 0x%script_instruction_offset% ; pre-append 0x

                script_instruction_address += script_instruction_offset

                ;MsgBox, script_instruction_address = %script_instruction_address%

                If (current_offset = num_offsets)
                {
                    Break
                }

                script_instruction_address := Memory_Read(everquest_process_handle, script_instruction_address)

                ;MsgBox, script_instruction_address = %script_instruction_address%

                current_offset := current_offset + 1
            }

            script_instruction_type    = %script_instruction_data5%
            script_instruction_value   = %script_instruction_data6%

            ;Break
        }
        Else ; no pointers
        {
            script_instruction_address = %script_instruction_data1%
            script_instruction_type    = %script_instruction_data2%
            script_instruction_value   = %script_instruction_data3%

            script_instruction_address = 0x%script_instruction_address% ; pre-append 0x
        }

        ;MsgBox, script_instruction_address = %script_instruction_address%`nscript_instruction_type = %script_instruction_type%`nscript_instruction_value = %script_instruction_value%

        If script_instruction_type = nops
        {
            Memory_WriteNops(everquest_process_handle, script_instruction_address, script_instruction_value)
        }

        If script_instruction_type = bytes
        {
            Memory_WriteBytes(everquest_process_handle, script_instruction_address, script_instruction_value)
        }

        If script_instruction_type = byte
        {
            Memory_WriteEx(everquest_process_handle, script_instruction_address, script_instruction_value, 1)
        }

        If script_instruction_type = word
        {
            Memory_WriteEx(everquest_process_handle, script_instruction_address, script_instruction_value, 2)
        }

        If script_instruction_type = dword
        {
            Memory_Write(everquest_process_handle, script_instruction_address, script_instruction_value)
        }

        If script_instruction_type = float
        {
            Memory_WriteFloat(everquest_process_handle, script_instruction_address, script_instruction_value)
        }
    }
}

Return

ButtonRefreshProcesses:
GuiControl,, DropDownListProcesses, |

GuiControl,, DropDownListProcesses, Dynamic

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
            GuiControl,, DropDownListProcesses, %psapi_process_id%:%psapi_process_name%
        }
    }
}

GuiControl, ChooseString, DropDownListProcesses, Dynamic
Return

TimerUpdateTitle:
script_process_id := DllCall("GetCurrentProcessId")

player_name := everquest_GetZonePlayerName()

WinSetTitle, ahk_pid %script_process_id%,,%player_name%
Return

TimerUpdateProcess:
GuiControlGet, processes_text,, DropDownListProcesses

IfNotEqual, processes_text, Dynamic
{
    StringSplit, processes_text_vars, processes_text, :

    selected_process_id = %processes_text_vars1%

    everquest_process_id := selected_process_id

    everquest_process_handle := Memory_GetProcessHandle(everquest_process_id)

    Return
}

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

GuiClose:
GuiEscape:

FieldOfView_Disable()
DrawDistance_Disable()

Gui, ListView, ListViewScripts

Loop % LV_GetCount()
{
    LV_Modify(A_Index, "-Check") ; disable all scripts
}

Gosub, TimerScripts

DllCall("FreeLibrary", "Ptr", psapi_dll_handle)
Memory_CloseHandle(everquest_process_handle)

ExitApp

Return
