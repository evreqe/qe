#SingleInstance, Ignore
#Persistent

#Include, C:\AutoHotkey\Lib\Memory.ahk

#Include, everquest.ahk

Global psapi_dll_handle := 0

Gui, Font, s10, Courier New
Gui, Add, StatusBar,, 
Gui, Add, Text,, Inventory
Gui, Add, ListView, vListViewMoneyInventory gListViewMoneyInventory +HwndListViewHwndMoneyInventory VScroll Grid -Multi w320, Platinum|Gold|Silver|Copper|
Gui, Add, Text,, 
Gui, Add, Text,, Bank
Gui, Add, ListView, vListViewMoneyBank gListViewMoneyBank +HwndListViewHwndMoneyBank VScroll Grid -Multi w320, Type|Amount|
Gui, Add, Text,, 
Gui, Add, Text,, Total
Gui, Add, ListView, vListViewMoneyTotal gListViewMoneyTotal +HwndListViewHwndMoneyTotal VScroll Grid -Multi w320, Platinum|Gold|Silver|Copper|
Gui, Add, Text,, 
Gui, Add, Button, w96 Default ym, &Refresh
Gui, Add, Button, w96, E&xit

UpdateListViewColumnsMoneyInventory()
{
    Gui, ListView, ListViewMoneyInventory

    LV_ModifyCol()

    LV_ModifyCol(1, "AutoHdr")
    LV_ModifyCol(2, "AutoHdr")
    LV_ModifyCol(3, "AutoHdr")
    LV_ModifyCol(4, "AutoHdr")

    LV_ModifyCol(1, "Integer")
    LV_ModifyCol(1, "Left")

    LV_ModifyCol(2, "Integer")
    LV_ModifyCol(2, "Left")

    LV_ModifyCol(3, "Integer")
    LV_ModifyCol(3, "Left")

    LV_ModifyCol(4, "Integer")
    LV_ModifyCol(4, "Left")
}

UpdateListViewMoneyInventory()
{
    GuiControl, -Redraw, ListViewMoneyInventory

    Gui, ListView, ListViewMoneyInventory
    LV_Delete()

    char_info := everquest_GetCharInfoPointer()

    platinum := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_PLATINUM)
    gold     := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_GOLD)
    silver   := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_SILVER)
    copper   := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_COPPER)

    LV_Add("", platinum, gold, silver, copper)

    UpdateListViewColumnsMoneyInventory()

    GuiControl, +Redraw, ListViewMoneyInventory
}

UpdateListViewColumnsMoneyBank()
{
    Gui, ListView, ListViewMoneyBank

    LV_ModifyCol()

    LV_ModifyCol(1, "AutoHdr")
    LV_ModifyCol(2, "AutoHdr")

    LV_ModifyCol(2, "Integer")
    LV_ModifyCol(2, "Left")
}

UpdateListViewMoneyBank()
{
    GuiControl, -Redraw, ListViewMoneyBank

    Gui, ListView, ListViewMoneyBank
    LV_Delete()

    char_info := everquest_GetCharInfoPointer()

    bank_platinum := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_BANK_PLATINUM)
    bank_gold     := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_BANK_GOLD)
    bank_silver   := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_BANK_SILVER)
    bank_copper   := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_BANK_COPPER)

    LV_Add("", "Platinum", bank_platinum)
    LV_Add("", "Gold",     bank_gold)
    LV_Add("", "Silver",   bank_silver)
    LV_Add("", "Copper",   bank_copper)

    UpdateListViewColumnsMoneyBank()

    GuiControl, +Redraw, ListViewMoneyBank
}

UpdateListViewColumnsMoneyTotal()
{
    Gui, ListView, ListViewMoneyTotal

    LV_ModifyCol()

    LV_ModifyCol(1, "AutoHdr")
    LV_ModifyCol(2, "AutoHdr")
    LV_ModifyCol(3, "AutoHdr")
    LV_ModifyCol(4, "AutoHdr")

    LV_ModifyCol(1, "Integer")
    LV_ModifyCol(1, "Left")

    LV_ModifyCol(2, "Integer")
    LV_ModifyCol(2, "Left")

    LV_ModifyCol(3, "Integer")
    LV_ModifyCol(3, "Left")

    LV_ModifyCol(4, "Integer")
    LV_ModifyCol(4, "Left")
}

UpdateListViewMoneyTotal()
{
    GuiControl, -Redraw, ListViewMoneyTotal

    Gui, ListView, ListViewMoneyTotal
    LV_Delete()

    char_info := everquest_GetCharInfoPointer()

    platinum := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_PLATINUM)
    gold     := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_GOLD)
    silver   := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_SILVER)
    copper   := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_COPPER)

    bank_platinum := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_BANK_PLATINUM)
    bank_gold     := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_BANK_GOLD)
    bank_silver   := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_BANK_SILVER)
    bank_copper   := Memory_Read(everquest_process_handle, char_info + EVERQUEST_OFFSET_CHAR_INFO_BANK_COPPER)

    platinum += bank_platinum
    gold     += bank_gold
    silver   += bank_silver
    copper   += bank_copper

    LV_Add("", platinum, gold, silver, copper)

    UpdateListViewColumnsMoneyTotal()

    GuiControl, +Redraw, ListViewMoneyTotal
}

UpdateStatusBar()
{
    player_name := everquest_GetZonePlayerName()

    SB_SetText("Player Name: " . player_name)
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

UpdateListViewMoneyInventory()
UpdateListViewMoneyBank()
UpdateListViewMoneyTotal()
UpdateStatusBar()

SetTimer, TimerUpdateProcess, 100

Gui, Show, Center, ...

Return

ListViewMoneyInventory:
Return

ListViewMoneyBank:
Return

ListViewMoneyTotal:
Return

ButtonRefresh:
UpdateListViewMoneyInventory()
UpdateListViewMoneyBank()
UpdateListViewMoneyTotal()

UpdateStatusBar()
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

ButtonExit:
GuiClose:
GuiEscape:

DllCall("FreeLibrary", "Ptr", psapi_dll_handle)
Memory_CloseHandle(everquest_process_handle)

ExitApp

Return
