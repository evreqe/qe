FileDelete, %A_ScriptDir%\data\buffs.txt

FileAppend, #id^name^good_effect^icon`n, %A_ScriptDir%\data\buffs.txt

FileRead, spells, %A_ScriptDir%\data\spells_us.txt

Loop, parse, spells, `n, `r
{
    ;MsgBox, %A_LoopField%

    If A_LoopField = 
    {
        Continue
    }

    StringSplit, spells_data, A_LoopField, ^

    id          = %spells_data1%

    name        = %spells_data2%

    duration    = %spells_data18%

    good_effect = %spells_data84%

    icon        = %spells_data145%

    ;MsgBox, id: %id%`nname: %name%`nduration: %duration%`ngood_effect: %good_effect%`nicon: %icon%

    If (duration = 0)
    {
        Continue
    }

    FileAppend, %id%^%name%^%good_effect%^%icon%`n, %A_ScriptDir%\data\buffs.txt
}
