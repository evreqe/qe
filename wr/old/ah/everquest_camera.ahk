#SingleInstance, Ignore
#Persistent

#MaxHotkeysPerInterval, 9999

#Include, C:\AutoHotkey\Lib\Memory.ahk
#Include, C:\AutoHotkey\Lib\Utility.ahk

#Include, everquest.ahk

Gui, Add, Checkbox, vCheckboxCameraHack gCheckboxCameraHack, Camera Hack Enabled
Gui, Add, Text,, 
Gui, Add, Text, vTextCameraRegionNumber w360, Camera Region Number: 0
Gui, Add, Text, vTextCameraY w360, Camera Y: 0.000000
Gui, Add, Text, vTextCameraX w360, Camera X: 0.000000
Gui, Add, Text, vTextCameraZ w360, Camera Z: 0.000000
Gui, Add, Text, vTextCameraHeading w360, Camera Heading: 0.000000
Gui, Add, Text, vTextCameraPitch w360, Camera Pitch: 0.000000
Gui, Add, Text, vTextCameraFieldOfView w360, Camera Field of View: 0.000000

EQ_CameraHack_Enable()
{
    Memory_Write(eq_process_handle, EQ_CAMERA_VIEW, EQ_CAMERA_VIEW_FIRST_PERSON)

    Memory_WriteNops(eq_process_handle, EQ_ASM_SET_CAMERA_Y,       EQ_ASM_SET_CAMERA_Y_SIZE)
    Memory_WriteNops(eq_process_handle, EQ_ASM_SET_CAMERA_X,       EQ_ASM_SET_CAMERA_X_SIZE)
    Memory_WriteNops(eq_process_handle, EQ_ASM_SET_CAMERA_Z,       EQ_ASM_SET_CAMERA_Z_SIZE)
    Memory_WriteNops(eq_process_handle, EQ_ASM_SET_CAMERA_HEADING, EQ_ASM_SET_CAMERA_HEADING_SIZE)
    Memory_WriteNops(eq_process_handle, EQ_ASM_SET_CAMERA_PITCH,   EQ_ASM_SET_CAMERA_PITCH_SIZE)
}

EQ_CameraHack_Disable()
{
    Memory_WriteBytes(eq_process_handle, EQ_ASM_SET_CAMERA_Y,       EQ_ASM_SET_CAMERA_Y_BYTES)
    Memory_WriteBytes(eq_process_handle, EQ_ASM_SET_CAMERA_X,       EQ_ASM_SET_CAMERA_X_BYTES)
    Memory_WriteBytes(eq_process_handle, EQ_ASM_SET_CAMERA_Z,       EQ_ASM_SET_CAMERA_Z_BYTES)
    Memory_WriteBytes(eq_process_handle, EQ_ASM_SET_CAMERA_HEADING, EQ_ASM_SET_CAMERA_HEADING_BYTES)
    Memory_WriteBytes(eq_process_handle, EQ_ASM_SET_CAMERA_PITCH,   EQ_ASM_SET_CAMERA_PITCH_BYTES)

    Memory_Write(eq_process_handle, EQ_CAMERA_VIEW, EQ_CAMERA_VIEW_FIRST_PERSON)
}

Memory_EnableDebugPrivileges()

eq_process_id     := Memory_GetProcessID(EQ_CLIENT)
eq_process_handle := Memory_GetProcessHandle(eq_process_id)

EQ_CameraHack_Disable()

SetTimer, TimerCamera, 100
SetTimer, TimerJoystick, 10

Gui, Show, Center, Camera

Return

CheckboxCameraHack:
GuiControlGet, is_checked,, CheckboxCameraHack
If (is_checked = 0)
{
    EQ_CameraHack_Disable()
}
Else
{
    EQ_CameraHack_Enable()
}
Return

Joy8::
; Start button
; toggle the camera hack on/off
GuiControlGet, is_checked,, CheckboxCameraHack
If (is_checked = 0)
{
    GuiControl,, CheckboxCameraHack, 1
    Gosub, CheckboxCameraHack
}
Else
{
    GuiControl,, CheckboxCameraHack, 0
    Gosub, CheckboxCameraHack
}
Return

Joy7::
; Select button
; turn the camera 180 degrees
GuiControlGet, is_checked,, CheckboxCameraHack
If (is_checked = 0)
{
    Return
}

camera := EQ_GetCamera()

camera_heading := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_HEADING)

camera_heading := camera_heading + (EQ_CAMERA_HEADING_MAX / 2)

If (camera_heading > EQ_CAMERA_HEADING_MAX)
{
   camera_heading := camera_heading - EQ_CAMERA_HEADING_MAX
}

Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_HEADING, camera_heading)
Return

Joy5::
; L1 button
; Reset the camera pitch to default
GuiControlGet, is_checked,, CheckboxCameraHack
If (is_checked = 0)
{
    Return
}

camera := EQ_GetCamera()

Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_PITCH, EQ_CAMERA_PITCH_DEFAULT)
Return

Joy4::
; Triangle button
; Move the camera up
GuiControlGet, is_checked,, CheckboxCameraHack
If (is_checked = 0)
{
    Return
}

camera := EQ_GetCamera()

camera_z := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Z)

multiplier := 1

; R1 button
GetKeyState, state, Joy6
If state = D
{
    multiplier := 10
}

camera_z += multiplier

Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Z, camera_z)
Return

Joy3::
; Square button
; Move the camera down
GuiControlGet, is_checked,, CheckboxCameraHack
If (is_checked = 0)
{
    Return
}

camera := EQ_GetCamera()

camera_z := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Z)

multiplier := 1

; R1 button
GetKeyState, state, Joy6
If state = D
{
    multiplier := 10
}

camera_z -= multiplier

Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Z, camera_z)
Return

Joy1::
; Cross button
; Move the camera forward
camera := EQ_GetCamera()

camera_y       := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Y)
camera_x       := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_X)
camera_z       := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Z)
camera_heading := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_HEADING)
camera_pitch   := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_PITCH)

camera_heading := (camera_heading * 360) / 512 ; convert from 512 degrees to 360 degrees

camera_heading -= 90 ; rotate by -90 degrees

If (camera_heading < 0)
{
    camera_heading += 360
}

camera_heading_radians := camera_heading * PI / 180 ; convert degrees to radians

camera_pitch := camera_pitch + Abs(EQ_CAMERA_PITCH_DEFAULT) ; remove negative offset from camera pitch

camera_pitch := (camera_pitch * 360) / 512 ; convert from 512 degrees to 360 degrees

camera_pitch_radians := camera_pitch * PI / 180 ; convert degrees to radians

add_y := Sin(camera_heading_radians)
add_x := Cos(camera_heading_radians)
add_z := Sin(camera_pitch_radians)

multiplier := 1

; R1 button
GetKeyState, state, Joy6
If state = D
{
    multiplier := 10
}

If (Abs(camera_pitch) < 90) ; do not move forward when looking straight up or down
{
    camera_y -= add_y * multiplier
    camera_x += add_x * multiplier
}

camera_z += add_z * multiplier

Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Y, camera_y)
Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_X, camera_x)
Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Z, camera_z)
Return

Joy2::
; Circle button
; Move the camera backward
camera := EQ_GetCamera()

camera_y       := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Y)
camera_x       := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_X)
camera_z       := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Z)
camera_heading := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_HEADING)
camera_pitch   := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_PITCH)

camera_heading := (camera_heading * 360) / 512 ; convert from 512 degrees to 360 degrees

camera_heading -= 90 ; rotate by -90 degrees

If (camera_heading < 0)
{
    camera_heading += 360
}

camera_heading_radians := camera_heading * PI / 180 ; convert degrees to radians

camera_pitch := camera_pitch + Abs(EQ_CAMERA_PITCH_DEFAULT) ; remove negative offset from camera pitch

camera_pitch := (camera_pitch * 360) / 512 ; convert from 512 degrees to 360 degrees

camera_pitch_radians := camera_pitch * PI / 180 ; convert degrees to radians

add_y := Sin(camera_heading_radians)
add_x := Cos(camera_heading_radians)
add_z := Sin(camera_pitch_radians)

multiplier := 1

; R1 button
GetKeyState, state, Joy6
If state = D
{
    multiplier := 10
}

If (Abs(camera_pitch) < 90) ; do not move backward when looking straight up or down
{
    camera_y += add_y * multiplier
    camera_x -= add_x * multiplier
}

camera_z -= add_z * multiplier

Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Y, camera_y)
Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_X, camera_x)
Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Z, camera_z)
Return

TimerCamera:
camera := EQ_GetCamera()

camera_region_number := Memory_Read(eq_process_handle, camera + EQ_OFFSET_CAMERA_REGION_NUMBER)
camera_y             := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Y)
camera_x             := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_X)
camera_z             := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Z)
camera_heading       := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_HEADING)
camera_pitch         := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_PITCH)
camera_field_of_view := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_FIELD_OF_VIEW)

GuiControl,, TextCameraRegionNumber, Camera Region Number: %camera_region_number%
GuiControl,, TextCameraY, Camera Y: %camera_y%
GuiControl,, TextCameraX, Camera X: %camera_x%
GuiControl,, TextCameraZ, Camera Z: %camera_z%
GuiControl,, TextCameraHeading, Camera Heading: %camera_heading%
GuiControl,, TextCameraPitch, Camera Pitch: %camera_pitch%
GuiControl,, TextCameraFieldOfView, Camera Field of View: %camera_field_of_view%
Return

TimerJoystick:
GuiControlGet, is_checked,, CheckboxCameraHack
If (is_checked = 0)
{
    Return
}

joy_x_default := 50
joy_y_default := 50
joy_z_default := 50
joy_r_default := 50

joy_threshold := 10

joy_sensitivity := 0.1

joy_x_threshold_upper := joy_x_default + joy_threshold
joy_x_threshold_lower := joy_x_default - joy_threshold

joy_y_threshold_upper := joy_y_default + joy_threshold
joy_y_threshold_lower := joy_y_default - joy_threshold

joy_z_threshold_upper := joy_z_default + joy_threshold
joy_z_threshold_lower := joy_z_default - joy_threshold

joy_r_threshold_upper := joy_r_default + joy_threshold
joy_r_threshold_lower := joy_r_default - joy_threshold

GetKeyState, joy_x, 1JoyX
GetKeyState, joy_y, 1JoyY
GetKeyState, joy_z, 1JoyZ
GetKeyState, joy_r, 1JoyR

joystick_analog_left_moved  := False
joystick_analog_right_moved := False


If (joy_x > joy_x_threshold_upper)
{
    joystick_analog_left_moved := true

    delta_x := ((joy_x - joy_x_default) * (50 / (100 - joy_x_default)))
}
Else If (joy_x < joy_x_threshold_lower)
{
    joystick_analog_left_moved := true

    delta_x := (joy_x * 50 / joy_x_default) - 50
}
Else
{
    delta_x := 0
}


If (joy_y > joy_y_threshold_upper)
{
    joystick_analog_left_moved := true

    delta_y := ((joy_y - joy_y_default) * (50 / (100 - joy_y_default)))
}
Else If (joy_y < joy_y_threshold_lower)
{
    joystick_analog_left_moved := true

    delta_y := (joy_y * 50 / joy_y_default) - 50
}
Else
{
    delta_y := 0
}


If (joy_z > joy_z_threshold_upper)
{
    joystick_analog_right_moved := true

    delta_z := ((joy_z - joy_z_default) * (50 / (100 - joy_z_default)))
}
Else If (joy_z < joy_z_threshold_lower)
{
    joystick_analog_right_moved := true

    delta_z := (joy_z * 50 / joy_z_default) - 50
}
Else
{
    delta_z := 0
}


If (joy_r > joy_r_threshold_upper)
{
    joystick_analog_right_moved := true

    delta_r := ((joy_r - joy_r_default) * (50 / (100 - joy_r_default)))
}
Else If (joy_r < joy_r_threshold_lower)
{
    joystick_analog_right_moved := true

    delta_r := (joy_r * 50 / joy_r_default) - 50
}
Else
{
    delta_r := 0
}


If joystick_analog_left_moved
{
    camera := EQ_GetCamera()

    camera_y       := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Y)
    camera_x       := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_X)
    camera_z       := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Z)
    camera_heading := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_HEADING)
    camera_pitch   := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_PITCH)

    camera_heading := (camera_heading * 360) / 512 ; convert from 512 degrees to 360 degrees

    camera_heading -= 90 ; rotate by -90 degrees

    is_looking_straight_up_or_down := False

    If (camera_pitch <= EQ_CAMERA_PITCH_MIN Or camera_pitch >= EQ_CAMERA_PITCH_MAX)
    {
        is_looking_straight_up_or_down := True
    }

    is_strafing := False

    If (delta_x = 0)
    {
        is_strafing := False
    }
    Else
    {
        is_strafing := True
    }

    If (delta_y > 0)
    {
        If (delta_x < 0)
        {
            camera_heading -= Abs((90 * delta_x) / joy_x_default)
        }
        Else If (delta_x > 0)
        {
            camera_heading += (90 * delta_x) / joy_x_default
        }
    }
    Else
    {
        If (delta_x < 0)
        {
            camera_heading += Abs((90 * delta_x) / joy_x_default)
        }
        Else If (delta_x > 0)
        {
            camera_heading -= (90 * delta_x) / joy_x_default
        }
    }

    If (camera_heading < 0)
    {
        camera_heading += 360
    }

    camera_heading_radians := camera_heading * PI / 180 ; convert degrees to radians

    camera_pitch := camera_pitch + Abs(EQ_CAMERA_PITCH_DEFAULT) ; remove negative offset from camera pitch

    camera_pitch := (camera_pitch * 360) / 512 ; convert from 512 degrees to 360 degrees

    camera_pitch_radians := camera_pitch * PI / 180 ; convert degrees to radians

    add_y := Sin(camera_heading_radians)
    add_x := Cos(camera_heading_radians)
    add_z := Sin(camera_pitch_radians)

    multiplier := 1.0

    ; R1 button
    GetKeyState, state, Joy6
    If state = D
    {
        multiplier := 2.0
    }

    ; R2 button
    GetKeyState, state, Joy12
    If state = D
    {
        multiplier := 0.5
    }

    If (delta_y < 0)
    {
        delta_y := Abs(delta_y)

        If (is_looking_straight_up_or_down = False)
        {
        camera_y -= add_y * (delta_y / 10) * multiplier
        camera_x += add_x * (delta_y / 10) * multiplier
        }

        camera_z += add_z * (delta_y / 10) * multiplier
    }
    Else If (delta_y > 0)
    {
        If (is_looking_straight_up_or_down = False)
        {
        camera_y += add_y * (delta_y / 10) * multiplier
        camera_x -= add_x * (delta_y / 10) * multiplier
        }

        camera_z -= add_z * (delta_y / 10) * multiplier
    }
    Else
    {
        If (is_looking_straight_up_or_down = False)
        {
        camera_y -= add_y * multiplier
        camera_x += add_x * multiplier
        }

        camera_z += add_z * multiplier
    }

    Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Y, camera_y)
    Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_X, camera_x)
    Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_Z, camera_z)

    ;ToolTip, add_y = %add_y%`nadd_x = %add_x%`nadd_z = %add_z%`n`ndelta_x = %delta_x%`ndelta_y = %delta_y%`ndelta_z = %delta_z%`ndelta_r = %delta_r%
}

If joystick_analog_right_moved
{
    ;ToolTip, delta_x = %delta_x%`ndelta_y = %delta_y%`ndelta_z = %delta_z%`ndelta_r = %delta_r%

    camera := EQ_GetCamera()

    camera_heading := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_HEADING)
    camera_pitch   := Memory_ReadFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_PITCH)

    multiplier := 1.0

    ; R1 button
    GetKeyState, state, Joy6
    If state = D
    {
        multiplier := 2.0
    }

    ; R2 button
    GetKeyState, state, Joy12
    If state = D
    {
        multiplier := 0.5
    }

    camera_heading_sub := delta_z * joy_sensitivity * multiplier

    If ((camera_heading - camera_heading_sub) >= EQ_CAMERA_HEADING_MAX)
    {
        camera_heading := EQ_CAMERA_HEADING_MIN
    }

    If ((camera_heading - camera_heading_sub) <= EQ_CAMERA_HEADING_MIN)
    {
        camera_heading := EQ_CAMERA_HEADING_MAX
    }

    camera_heading := camera_heading - camera_heading_sub


    camera_pitch_sub := delta_r * joy_sensitivity * multiplier

    If ((camera_pitch - camera_pitch_sub) > EQ_CAMERA_PITCH_MAX)
    {
        camera_pitch := EQ_CAMERA_PITCH_MAX
    }

    If ((camera_pitch - camera_pitch_sub) < EQ_CAMERA_PITCH_MIN)
    {
        camera_pitch := EQ_CAMERA_PITCH_MIN
    }

    camera_pitch := camera_pitch - camera_pitch_sub

    Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_HEADING, camera_heading)
    Memory_WriteFloat(eq_process_handle, camera + EQ_OFFSET_CAMERA_PITCH,   camera_pitch)
}
Return

GuiClose:
GuiEscape:

SetTimer, TimerCamera, Off
SetTimer, TimerJoystick, Off

EQ_CameraHack_Disable()

Memory_CloseHandle(eq_process_handle)

ExitApp

Return
