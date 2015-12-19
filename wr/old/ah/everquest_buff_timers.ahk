#SingleInstance, Ignore
#Persistent

#Include, everquest.ahk

Global psapi_dll_handle := 0

Global buffs_file := 

Global buff_timers_text = 

Global num_buffs          := 0
Global num_buffs_previous := 0

Gui, Font, s10, Courier New
Gui, Add, StatusBar,,
Gui, Add, ListView, vListViewBuffs gListViewBuffs +HwndListViewHwndBuffs VScroll Grid -Multi w640 h480 xm, #|ID|Name|Time|Type|Icon|

UpdateListViewColumnsBuffs()
{
    Gui, ListView, ListViewBuffs

    LV_ModifyCol()
    LV_ModifyCol(1, "AutoHdr")
    LV_ModifyCol(2, "AutoHdr")
    LV_ModifyCol(3, "AutoHdr")
    LV_ModifyCol(4, "AutoHdr")
    LV_ModifyCol(5, "AutoHdr")
    LV_ModifyCol(6, "AutoHdr")

    ;LV_ModifyCol(2, "Integer")
    ;LV_ModifyCol(2, "Left")

    LV_ModifyCol(4, "Text")
    LV_ModifyCol(4, "Left")
}

UpdateListViewBuffs()
{
    GuiControl, -Redraw, ListViewBuffs

    Gui, ListView, ListViewBuffs
    LV_Delete()

    Loop, %EVERQUEST_BUFFS_MAX%
    {
        Gui, ListView, ListViewBuffs

        LV_Add("", A_Index, "", "", "", "", "")
    }

    GuiControl, +Redraw, ListViewBuffs
}

UpdateBuffs()
{
    ;GuiControl, -Redraw, ListViewBuffs

    buff_timers_text = 

    char_info := everquest_GetCharInfoPointer()

    buffs_structure := char_info + EVERQUEST_OFFSET_CHAR_INFO_BUFFS_STRUCTURE

    buffs_structure_address := buffs_structure

    Loop, %EVERQUEST_BUFFS_MAX%
    {
        buff_id := Memory_Read(everquest_process_handle, buffs_structure_address + EVERQUEST_OFFSET_BUFFS_STRUCTURE_SPELL_ID)

        If (buff_id = EVERQUEST_BUFF_SPELL_ID_NULL)
        {
            buff_id = 
        }

        buff_name := (Unknown Buff)

        buff_type := 

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
                buff_name = %buffs_file_data2%
                buff_type = %buffs_file_data3%
                buff_icon = %buffs_file_data4%

                Break
            }
        }

        If (buff_type = EVERQUEST_BUFF_TYPE_DETRIMENTAL)
        {
            buff_type = Detrimental
        }

        If (buff_type = EVERQUEST_BUFF_TYPE_BENEFICIAL)
        {
            buff_type = Beneficial
        }

        If (buff_type = EVERQUEST_BUFF_TYPE_BENEFICIAL_GROUP_ONLY)
        {
            buff_type = Beneficial, Group Only
        }

        buff_ticks := Memory_Read(everquest_process_handle, buffs_structure_address + EVERQUEST_OFFSET_BUFFS_STRUCTURE_DURATION)

        total_seconds := buff_ticks * 6

        buff_hours := total_seconds / 3600

        total_seconds := Mod(total_seconds, 3600)

        buff_minutes := total_seconds / 60

        buff_seconds := Mod(total_seconds, 60)

        buff_hours   := Floor(buff_hours)
        buff_minutes := Floor(buff_minutes)
        buff_seconds := Floor(buff_seconds)

        If (buff_hours < 10)
        {
            buff_hours = 0%buff_hours%
        }

        If (buff_minutes < 10)
        {
            buff_minutes = 0%buff_minutes%
        }

        If (buff_seconds < 10)
        {
            buff_seconds = 0%buff_seconds%
        }

        buff_time := buff_hours . "h" . " " . buff_minutes . "m" . " " . buff_seconds . "s"

        If (buff_ticks = 0)
        {
            buff_id   = 
            buff_name = 
            buff_time = 
            buff_type = 
            buff_icon = 
        }

        If (buff_ticks > 0)
        {
            buff_timers_text = %buff_timers_text%%buff_time%

            buff_timers_text .= "    "

            If buff_type = Detrimental
            {
                buff_timers_text = %buff_timers_text%-
                buff_timers_text .= " "
            }

            buff_timers_text = %buff_timers_text%%buff_name%`n
        }

        buffs_structure_address += EVERQUEST_BUFFS_STRUCTURE_BUFF_SIZE

        Gui, ListView, ListViewBuffs

        LV_Modify(A_Index, "", A_Index, buff_id, buff_name, buff_time, buff_type, buff_icon)

        ;LV_ModifyCol(4, "SortDesc")

        SB_SetText("Number of Buffs: " . num_buffs)
    }

    UpdateListViewColumnsBuffs()

    ;GuiControl, +Redraw, ListViewBuffs
}

CheckBuffs()
{
    char_info := everquest_GetCharInfoPointer()

    buffs_structure := char_info + EVERQUEST_OFFSET_CHAR_INFO_BUFFS_STRUCTURE

    buffs_structure_address := buffs_structure

    num_buffs := 0

    Loop, %EVERQUEST_BUFFS_MAX%
    {
        buff_ticks := Memory_Read(everquest_process_handle, buffs_structure_address + EVERQUEST_OFFSET_BUFFS_STRUCTURE_DURATION)

        If (buff_ticks > 0)
        {
            num_buffs++
        }

        buffs_structure_address += EVERQUEST_BUFFS_STRUCTURE_BUFF_SIZE
    }

    If (num_buffs_previous != num_buffs)
    {
        UpdateBuffs()
    }

    num_buffs_previous := num_buffs
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

FileRead, buffs_file, %A_ScriptDir%\data\buffs.txt

UpdateListViewBuffs()
UpdateListViewColumnsBuffs()

SetTimer, TimerCheckBuffs, 1000

SetTimer, TimerUpdateBuffs, 6000
Gosub, TimerUpdateBuffs

GuiControl, Focus, ListViewBuffs

Gui, Show, Center, bt

SetTimer, TimerUpdateProcess, 100

Return

ListViewBuffs:
Return

TimerUpdateBuffs:
UpdateBuffs()
Return

TimerCheckBuffs:
CheckBuffs()
Return

XButton1::
If buff_timers_text = 
{
    Return
}

StringRight, find_empty_line, buff_timers_text, 1
If find_empty_line = `n
{
    StringTrimRight, buff_timers_text, buff_timers_text, 1
}

StringReplace, buff_timers_text, buff_timers_text, 00h%A_Space%, %A_Space%%A_Space%%A_Space%%A_Space%, All
StringReplace, buff_timers_text, buff_timers_text, 00m%A_Space%, %A_Space%%A_Space%%A_Space%%A_Space%, All
StringReplace, buff_timers_text, buff_timers_text, 00s, %A_Space%%A_Space%%A_Space%, All

Sort, buff_timers_text

mouse_x := Memory_Read(everquest_process_handle, EVERQUEST_MOUSE_X)
mouse_y := Memory_Read(everquest_process_handle, EVERQUEST_MOUSE_Y)

window_x := Memory_Read(everquest_process_handle, EVERQUEST_WINDOW_X)
window_y := Memory_Read(everquest_process_handle, EVERQUEST_WINDOW_Y)

show_x := window_x + mouse_x
show_y := window_y + mouse_y

Gui, 2: Font, s10, Courier New
Gui, 2: Add, Text,, %buff_timers_text%
Gui, 2: +AlwaysOnTop +Border +Caption +ToolWindow -MaximizeBox -MinimizeBox
Gui, 2: Show, x%show_x% y%show_y% AutoSize NoActivate, Buff Timers - %num_buffs% effect(s)

KeyWait, XButton1
Gui, 2: Destroy
Return

XButton2::
char_info := everquest_GetCharInfoPointer()

player_name := Memory_ReadString(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_NAME, 0x40)

FileRead, buffs_needed_file, %A_ScriptDir%\buffs_needed\%player_name%.txt
If ErrorLevel
{
    Return
}

buffs_active := 

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
            buff_name = %buffs_file_data2%

            buffs_active = %buffs_active%%buff_name%`n

            Break
        }
    }

    buffs_structure_address +=  EVERQUEST_BUFFS_STRUCTURE_BUFF_SIZE
}

;MsgBox, buffs_active = %buffs_active%

buffs_needed_text = No buffs are needed!

Loop, Parse, buffs_needed_file, `n, `r
{
    ;MsgBox, %A_LoopField%

    If A_LoopField = 
    {
        Continue
    }

    buff_needed_name = %A_LoopField%

    buff_is_needed := True

    Loop, Parse, buffs_active, `n
    {
        ;MsgBox, %A_LoopField%

        If A_LoopField = 
        {
            Continue
        }

        buff_active = %A_LoopField%

        If buff_active = %buff_needed_name%
        {
            buff_is_needed := False

            Break
        }
    }

    If buff_is_needed
    {
        StringReplace, buffs_needed_text, buffs_needed_text, No buffs are needed!,, All

        buffs_needed_text = %buffs_needed_text%%buff_needed_name%`n
    }
}

StringRight, find_empty_line, buffs_needed_text, 1
If find_empty_line = `n
{
    StringTrimRight, buffs_needed_text, buffs_needed_text, 1
}

Sort, buffs_needed_text

mouse_x := Memory_Read(everquest_process_handle, EVERQUEST_MOUSE_X)
mouse_y := Memory_Read(everquest_process_handle, EVERQUEST_MOUSE_Y)

window_x := Memory_Read(everquest_process_handle, EVERQUEST_WINDOW_X)
window_y := Memory_Read(everquest_process_handle, EVERQUEST_WINDOW_Y)

show_x := window_x + mouse_x
show_y := window_y + mouse_y

Gui, 3: Font, s10, Courier New
Gui, 3: Add, Text,, %buffs_needed_text%
Gui, 3: +AlwaysOnTop +Border +Caption +ToolWindow -MaximizeBox -MinimizeBox
Gui, 3: Show, x%show_x% y%show_y% AutoSize NoActivate, Buffs Needed ; - %player_name%

KeyWait, XButton2
Gui, 3: Destroy
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

GuiClose:
GuiEscape:

DllCall("FreeLibrary", "Ptr", psapi_dll_handle)
Memory_CloseHandle(everquest_process_handle)

ExitApp

Return
