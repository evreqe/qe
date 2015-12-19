#SingleInstance, Ignore
#Persistent

#Include, C:\AutoHotkey\Lib\Memory.ahk

#Include, everquest.ahk

Gui, Font, s10, Courier New
Gui, Add, ListView, vListViewGuilds +HwndListViewHwndGuilds VScroll Grid -Multi w640 h480, ID|Guild|

UpdateListViewColumnsGuilds()
{
    Gui, ListView, ListViewGuilds

    LV_ModifyCol()

    LV_ModifyCol(1, "AutoHdr")
    LV_ModifyCol(2, "AutoHdr")

    LV_ModifyCol(1, "Integer")
    LV_ModifyCol(1, "Left")
}

UpdateListViewGuilds()
{
    GuiControl, -Redraw, ListViewGuilds

    Gui, ListView, ListViewGuilds
    LV_Delete()

    guild_name_address := EQ_GUILD_NAMES_BEGIN

    Loop, %EQ_GUILD_NAMES_MAX%
    {
        guild_index = %A_Index%

        guild_name := Memory_ReadString(eq_process_handle, guild_name_address, EQ_GUILD_NAME_SIZE)

        ;If guild_name = 
        ;{
            ;Break
        ;}

        LV_Add("", guild_index, guild_name)

        guild_name_address := guild_name_address + EQ_GUILD_NAME_SIZE
    }

    UpdateListViewColumnsGuilds()

    GuiControl, +Redraw, ListViewGuilds
}

Memory_EnableDebugPrivileges()

eq_process_id     := Memory_GetProcessID(EQ_CLIENT)
eq_process_handle := Memory_GetProcessHandle(eq_process_id)

UpdateListViewGuilds()

Gui, Show, Center, Guilds

Return

GuiClose:
GuiEscape:

Memory_CloseHandle(eq_process_handle)
ExitApp

Return
