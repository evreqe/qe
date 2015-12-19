Global WS_CAPTION := 0xC00000

WinWait, EverQuest
IfWinExist
{
    WinSet, Style, -%WS_CAPTION%
    WinMove, , , 0, 0, 1280, 720
    WinSet, AlwaysOnTop, On
    WinSet, AlwaysOnTop, Off
}

;WinWait, m
;IfWinExist
;{
;    WinSet, Style, -%WS_CAPTION%
;    WinMove, , , 0, 0, 1280, 720
;    WinSet, AlwaysOnTop, On
;    WinSet, AlwaysOnTop, Off
;}
Return
