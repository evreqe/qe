Global PROCESS_ALL_ACCESS        := 0x001F0FFF
Global PROCESS_VM_READ           := 0x10
Global PROCESS_QUERY_INFORMATION := 0x400

Global TOKEN_ALL_ACCESS        := 0xF01FF
Global TOKEN_QUERY             := 0x8
Global TOKEN_ADJUST_PRIVILEGES := 0x20

Global SE_PRIVILEGE_ENABLED := 2

Global PAGE_READWRITE         := 0x04
Global PAGE_EXECUTE_READWRITE := 0x40

Global TH32CS_SNAPMODULE := 0x00000008

Global INVALID_HANDLE_VALUE := -1

Global MODULEENTRY32_SIZE := 548

Global MODULEENTRY32_modBaseAddr := 20

Global MODULEENTRY32_hModule := 29

Global MODULEENTRY32_szModule := 32

Memory_EnableDebugPrivileges()
{
    Process, Exist
    h := DllCall("OpenProcess", "UInt", PROCESS_QUERY_INFORMATION, "Int", false, "UInt", ErrorLevel, "Ptr")

    DllCall("Advapi32.dll\OpenProcessToken", "Ptr", h, "UInt", TOKEN_ADJUST_PRIVILEGES, "PtrP", t)

    VarSetCapacity(ti, 16, 0)
    NumPut(1, ti, 0, "UInt")

    DllCall("Advapi32.dll\LookupPrivilegeValue", "Ptr", 0, "Str", "SeDebugPrivilege", "Int64P", luid)

    NumPut(luid, ti, 4, "Int64")
    NumPut(SE_PRIVILEGE_ENABLED, ti, 12, "UInt")

    r := DllCall("Advapi32.dll\AdjustTokenPrivileges", "Ptr", t, "Int", false, "Ptr", &ti, "UInt", 0, "Ptr", 0, "Ptr", 0)

    DllCall("CloseHandle", "Ptr", t)
    DllCall("CloseHandle", "Ptr", h)
}

Memory_FloatToHex(value)
{
   format := A_FormatInteger
   SetFormat, Integer, H
   result := DllCall("MulDiv", Float, value, Int, 1, Int, 1, UInt)
   SetFormat, Integer, %format%
   Return, result
}

Memory_HexToFloat(value)
{
    Return, (1 - 2 * (value >> 31)) * (2 ** ((value >> 23 & 255) - 150)) * (0x800000 | value & 0x7FFFFF)
}

Memory_GetProcessID(process_name)
{
    Process, Exist, %process_name%
    process_id = %ErrorLevel%

    Return, process_id
}

Memory_GetProcessHandle(process_id)
{
    process_handle := DllCall("OpenProcess", "UInt", PROCESS_ALL_ACCESS, "Int", false, "UInt", process_id, "Ptr")

    Return, process_handle
}

Memory_GetModuleBase(process_id, module_name)
{
    snapshot_handle := DllCall("CreateToolhelp32Snapshot", "UInt", TH32CS_SNAPMODULE, "UInt", process_id)

    If (snapshot_handle = INVALID_HANDLE_VALUE)
    {
        Return, False
    }

    VarSetCapacity(me32, MODULEENTRY32_SIZE, 0)

    NumPut(MODULEENTRY32_SIZE, me32)

    If (DllCall("Module32First", "UInt", snapshot_handle, "UInt", &me32))
    {
        While (DllCall("Module32Next", "UInt", snapshot_handle, "UInt", &me32))
        {
            If (module_name == StrGet(&me32 + MODULEENTRY32_szModule, 256, "CP0"))
            ;If (DllCall("lstrcmpi", "Str", module_name, "UInt", &me32 + MODULEENTRY32_szModule) = -1)
            {
                DllCall("CloseHandle", "UInt", snapshot_handle)

                Return, NumGet(&me32, MODULEENTRY32_modBaseAddr)
            }
        }
    }

    DllCall("CloseHandle", "UInt", snapshot_handle)

    Return, False
}

Memory_CloseHandle(process_handle)
{
    DllCall("CloseHandle", "Ptr", process_handle)
}

Memory_Read(process_handle, address)
{
    DllCall("ReadProcessMemory", "UInt", process_handle, "UInt", address, "UInt *", value, "UInt", 4, "UInt *", 0)

    Return, value
}

Memory_ReadEx(process_handle, address, size)
{
    DllCall("ReadProcessMemory", "UInt", process_handle, "UInt", address, "UInt *", value, "UInt", size, "UInt *", 0)

    Return, value
}

Memory_ReadFloat(process_handle, address)
{
    DllCall("ReadProcessMemory", "UInt", process_handle, "UInt", address, "Float *", value, "UInt", 4, "UInt *", 0)

    Return, value
}

Memory_ReadReverse(process_handle, address)
{
    VarSetCapacity(value, 4, 0)
    DllCall("ReadProcessMemory", "UInt", process_handle, "UInt", address, "Str", value, "UInt", 4, "UInt *", 0)

    Return, *(&value + 3) | *(&value + 2) << 8 | *(&value + 1) << 16 | *(&value) << 24
}

Memory_ReadString(process_handle, address, size)
{
    VarSetCapacity(value, size)
    DllCall("ReadProcessMemory", "UInt", process_handle, "UInt", address, "Ptr", &value, "UInt", size, "UInt *", 0)

    result := StrGet(&value, "CP0")

    Return, result
}

Memory_Write(process_handle, address, value)
{
    ;DllCall("VirtualProtectEx", "UInt", process_handle, "UInt", address, "UInt", 4, "UInt", PAGE_READWRITE, "UInt *", 0)

    DllCall("WriteProcessMemory", "UInt", process_handle, "UInt", address, "UInt *", value, "UInt", 4, "UInt *", 0)
}

Memory_WriteEx(process_handle, address, value, size)
{
    ;DllCall("VirtualProtectEx", "UInt", process_handle, "UInt", address, "UInt", size, "UInt", PAGE_READWRITE, "UInt *", 0)

    DllCall("WriteProcessMemory", "UInt", process_handle, "UInt", address, "UInt *", value, "UInt", size, "UInt *", 0)
}

Memory_WriteFloat(process_handle, address, value)
{
    ;DllCall("VirtualProtectEx", "UInt", process_handle, "UInt", address, "UInt", 4, "UInt", PAGE_READWRITE, "UInt *", 0)

    DllCall("WriteProcessMemory", "UInt", process_handle, "UInt", address, "Float *", value, "UInt", 4, "UInt *", 0)
}

Memory_WriteNops(process_handle, address, size)
{
    If (size <= 0)
    {
        Return
    }

    VarSetCapacity(value, size)

    Loop, %size%
    {
        NumPut(0x90, value, A_Index - 1, "UChar")
    }

    ;DllCall("VirtualProtectEx", "UInt", process_handle, "UInt", address, "UInt", size, "UInt", PAGE_READWRITE, "UInt *", 0)

    DllCall("WriteProcessMemory", "UInt", process_handle, "UInt", address, "UInt", &value, "UInt", size, "UInt *", 0)
}

Memory_WriteBytes(process_handle, address, bytes)
{
    bytes_size := 0
    Loop, Parse, bytes, `,
    {
        bytes_size += 1
    }

    ;Sort, bytes, D`, N R ; bytes in reverse order

    VarSetCapacity(value, bytes_size)

    Loop, Parse, bytes, `,
    {
        byte = 0x%A_LoopField% ; pre-append 0x

        NumPut(byte, value, A_Index - 1, "UChar")
    }

    ;DllCall("VirtualProtectEx", "UInt", process_handle, "UInt", address, "UInt", bytes_size, "UInt", PAGE_READWRITE, "UInt *", 0)

    DllCall("WriteProcessMemory", "UInt", process_handle, "UInt", address, "UInt", &value, "UInt", bytes_size, "UInt *", 0)
}
