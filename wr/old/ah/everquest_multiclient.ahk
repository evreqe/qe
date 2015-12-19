#Persistent

Global WS_CAPTION := 0xC00000

Global resolution_width  := 1680
Global resolution_height := 1050

Global eqclient_ini = "c:\eqdalaya\eqclient.ini"

IniWrite, TRUE, %eqclient_ini%, Defaults, WindowedMode
IniWrite, %resolution_width%,  %eqclient_ini%, VideoMode, Width
IniWrite, %resolution_height%, %eqclient_ini%, VideoMode, Height

Return

; press tilde to switch between client windows
`::
WinActivateBottom, EverQuest
Return

; press control+tilde for borderless fullscreen windowed mode
^`::
WinWait, EverQuest
IfWinExist
{
    WinSet, Style, -%WS_CAPTION%
    WinMove, , , 0, 0, %resolution_width%, %resolution_height%
    WinSet, AlwaysOnTop, Off
}
Return

