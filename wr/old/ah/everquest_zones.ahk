#SingleInstance, Ignore
#Persistent

#Include, C:\AutoHotkey\Lib\Memory.ahk

#Include, everquest.ahk

Gui, Font, s10, Courier New
Gui, Add, ListView, vListViewZones +HwndListViewHwndZones VScroll Grid -Multi w640 h480, Expansion|ID|Short|Long
Gui, Add, Button, w96 ym, E&xit

UpdateListViewColumnsZones()
{
    Gui, ListView, ListViewZones

    LV_ModifyCol()

    LV_ModifyCol(1, "AutoHdr")
    LV_ModifyCol(2, "AutoHdr")
    LV_ModifyCol(3, "AutoHdr")
    LV_ModifyCol(4, "AutoHdr")
}

UpdateListViewZones()
{
    GuiControl, -Redraw, ListViewZones

    Gui, ListView, ListViewZones
    LV_Delete()

    FileDelete, %A_ScriptDir%\everquest_zones.txt

    zone_pointer := 0x03662A50 ;0x036639F8

    Loop, 512
    {
        zone_address := Memory_Read(everquest_process_handle, zone_pointer)

        zone_expansion := Memory_Read(everquest_process_handle, zone_address + 0x04)

        zone_id := Memory_ReadEx(everquest_process_handle, zone_address + 0x08, 2)

        zone_name_short := Memory_ReadString(everquest_process_handle, zone_address + 0x0C, 0x81)

        zone_name_long := Memory_ReadString(everquest_process_handle, zone_address + 0x8D, 0x103)

        LV_Add("", zone_expansion, zone_id, zone_name_short, zone_name_long)

        If zone_name_short And zone_name_long
        {
            zone_information = %zone_name_short%=%zone_name_long%

            FileAppend, %zone_information%`n, %A_ScriptDir%\everquest_zones.txt
        }

        zone_pointer := zone_pointer + 0x04
    }

    UpdateListViewColumnsZones()

    GuiControl, +Redraw, ListViewZones
}

Memory_EnableDebugPrivileges()

everquest_process_id := Memory_GetProcessID(EVERQUEST_CLIENT)

everquest_process_handle := Memory_GetProcessHandle(everquest_process_id)

UpdateListViewZones()

Gui, Show, Center, Zones

Return

ButtonExit:
GuiClose:
GuiEscape:

Memory_CloseHandle(everquest_process_handle)

ExitApp

Return
