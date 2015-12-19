#ifndef EQSOD_FUNCTIONS_H
#define EQSOD_FUNCTIONS_H

#include <string>

#include <cstdint>
#include <cstring>
#include <cmath>

#include "eqdalaya.h"

#define EQ_FUNCTION_AT_ADDRESS(function,offset) __declspec(naked) function\
{\
    __asm{mov eax, offset};\
    __asm{jmp eax};\
}

#define EQ_FUNCTION_AT_VARIABLE_ADDRESS(function,variable) __declspec(naked) function\
{\
    __asm{mov eax, [variable]};\
    __asm{jmp eax};\
}

#define EQ_FUNCTION_AT_VIRTUAL_ADDRESS(function,virtualoffset) __declspec(naked) function\
{\
    __asm{mov eax, [ecx]};\
    __asm{lea eax, [eax+virtualoffset]};\
    __asm{mov eax, [eax]};\
    __asm{jmp eax};\
}

template <class T>
T EQ_ReadMemory(DWORD address)
{
    DWORD oldProtect;
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, sizeof(T), PAGE_READWRITE, &oldProtect);

    T* buffer = (T*)address;

    VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, sizeof(T), oldProtect, &oldProtect);

    return (*buffer);
}

template <class T>
void EQ_WriteMemory(DWORD address, T value)
{
    DWORD oldProtect;
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, sizeof(value), PAGE_READWRITE, &oldProtect);

    T* buffer = (T*)address;
    *buffer = value;

    VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, sizeof(value), oldProtect, &oldProtect);
}

// direct input

//IDirectInput8** EQ_ppIDirectInput8 = (IDirectInput8**)EQ_DINPUT_ROOT;
//#define EQ_IDirectInput8 (*EQ_ppIDirectInput8)

//IDirectInputDevice8** EQ_ppIDirectInputDevice8_Keyboard = (IDirectInputDevice8**)EQ_DINPUT_DEVICE_KEYBOARD;
//#define EQ_IDirectInputDevice8_Keyboard (*EQ_ppIDirectInputDevice8_Keyboard)

//IDirectInputDevice8** EQ_ppIDirectInputDevice8_Mouse = (IDirectInputDevice8**)EQ_DINPUT_DEVICE_MOUSE;
//#define EQ_IDirectInputDevice8_Mouse (*EQ_ppIDirectInputDevice8_Mouse)

class EQGraphicsDLL;

class CXStr;
class CSidlScreenWnd;
class CXWndManager;
class CXWnd;
class CXPoint;
class CXRect;

class CDisplay;
class CEverQuest;
class CHotButtonWnd;
class CTextEntryWnd;
class CTextOverlay;
class EQPlayer;
class EQ_Character;

class EQGraphicsDLL
{
public:
    int EQGraphicsDLL::DrawLine(EQLINE* lineBegin, EQLINE* lineEnd, DWORD argbColor);
    int EQGraphicsDLL::DrawQuad(EQRECT* rect, DWORD argbColor);
};

EQGraphicsDLL** EQ_CLASS_ppEQGraphicsDLL = (EQGraphicsDLL**)EQ_POINTER_EQGraphicsDLL;
#define EQ_CLASS_EQGraphicsDLL (*EQ_CLASS_ppEQGraphicsDLL)

class CXStr
{
public:
    CXStr::~CXStr(void);
    CXStr::CXStr(char const *);
    void CXStr::operator+=(char const *);
    void CXStr::operator=(char const *);
    class CXStr& CXStr::operator=(class CXStr const&);
};

class CSidlScreenWnd
{
public:
    //
};

class CDisplay
{
public:
    static int __cdecl CDisplay::WriteTextHD2(const char* text, int x, int y, int color);
};

CDisplay** EQ_CLASS_ppCDisplay = (CDisplay**)EQ_POINTER_CDisplay;
#define EQ_CLASS_CDisplay (*EQ_CLASS_ppCDisplay)

class CButtonWnd
{
public:
    //
};

class CEverQuest
{
public:
    void CEverQuest::dsp_chat(const char* text, int color, bool filtered);
    void CEverQuest::dsp_chat(const char* text);
    void CEverQuest::EnterZone(struct HWND__*);
    char* CEverQuest::GetBodyTypeDesc(int bodyType);
    void CEverQuest::InterpretCmd(class EQPlayer* spawn, char* text);
    int __cdecl CEverQuest::LMouseUp(int x, int y);
    int __cdecl CEverQuest::RMouseUp(int x, int y);
};

CEverQuest** EQ_CLASS_ppCEverQuest = (CEverQuest**)EQ_POINTER_CEverQuest;
#define EQ_CLASS_CEverQuest (*EQ_CLASS_ppCEverQuest)

class CHotButtonWnd : public CSidlScreenWnd
{
public:
    void CHotButtonWnd::DoHotButton(unsigned int buttonIndex, bool allowAutoRightClick);
};

CHotButtonWnd** EQ_CLASS_ppCHotButtonWnd = (CHotButtonWnd**)EQ_POINTER_CHotButtonWnd;
#define EQ_CLASS_CHotButtonWnd (*EQ_CLASS_ppCHotButtonWnd)

class CTextEntryWnd
{
public:
    void CTextEntryWnd::Activate(void* parentWindow, int a2, char* displayText, int a4, int a5, char* defaultText);
};

CTextEntryWnd** EQ_CLASS_ppCTextEntryWnd = (CTextEntryWnd**)EQ_POINTER_CTextEntryWnd;
#define EQ_CLASS_CTextEntryWnd (*EQ_CLASS_ppCTextEntryWnd)

class CTextOverlay
{
public:
    void CTextOverlay::DisplayText(const char* text, int textColor, int font, int alpha, int x,int y, int duration); // alpha is 0-255, duration is in milliseconds
};

CTextOverlay** EQ_CLASS_ppCTextOverlay = (CTextOverlay**)EQ_POINTER_CTextOverlay;
#define EQ_CLASS_CTextOverlay (*EQ_CLASS_ppCTextOverlay)

EQ_Character** EQ_CLASS_ppEQ_Character = (EQ_Character**)EQ_POINTER_EQ_Character;
#define EQ_CLASS_EQ_Character (*EQ_CLASS_ppEQ_Character)

class EQ_Character
{
public:
    void EQ_Character::eqspa_movement_rate(int unknown);
    int EQ_Character::UseSkill(unsigned char skill, class EQPlayer* targetSpawn);
};

/* EQGraphicsDLL */

typedef int (__thiscall* EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawLine)(void* pThis, EQLINE* lineBegin, EQLINE* lineEnd, DWORD argbColor);
EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawLine EQGraphicsDLL__DrawLine = NULL;

typedef int (__thiscall* EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawQuad)(void* pThis, EQRECT* rect, DWORD argbColor);
EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawQuad EQGraphicsDLL__DrawQuad = NULL;

/* CDisplay */

#ifdef EQ_FUNCTION_CDisplay__WriteTextHD2
typedef int (__cdecl* EQ_FUNCTION_TYPE_CDisplay__WriteTextHD2)(const char* text, int x, int y, int color);
EQ_FUNCTION_AT_ADDRESS(int CDisplay::WriteTextHD2(const char* text, int x, int y, int color), EQ_FUNCTION_CDisplay__WriteTextHD2);
#endif

/* CEverQuest */

#ifdef EQ_FUNCTION_CEverQuest__dsp_chat
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__dsp_chat)(void* pThis, const char* text, int textColor, bool filtered);
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::dsp_chat(const char* text, int textColor, bool filtered), EQ_FUNCTION_CEverQuest__dsp_chat);
#endif

#ifdef EQ_FUNCTION_CEverQuest__dsp_chat__2
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::dsp_chat(const char* text), EQ_FUNCTION_CEverQuest__dsp_chat__2);
#endif

#ifdef EQ_FUNCTION_CEverQuest__EnterZone
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__EnterZone)(void* pThis, struct HWND__*);
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::EnterZone(struct HWND__ *), EQ_FUNCTION_CEverQuest__EnterZone);
#endif

#ifdef EQ_FUNCTION_CEverQuest__GetBodyTypeDesc
EQ_FUNCTION_AT_ADDRESS(char* CEverQuest::GetBodyTypeDesc(int bodyType), EQ_FUNCTION_CEverQuest__GetBodyTypeDesc);
#endif

#ifdef EQ_FUNCTION_CEverQuest__InterpretCmd
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__InterpretCmd)(void* pThis, class EQPlayer* spawn, char* text);
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::InterpretCmd(class EQPlayer*, char*), EQ_FUNCTION_CEverQuest__InterpretCmd);
#endif

#ifdef EQ_FUNCTION_CEverQuest__LMouseUp
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__LMouseUp)(void* pThis, int x, int y);
EQ_FUNCTION_AT_ADDRESS(int __cdecl CEverQuest::LMouseUp(int, int), EQ_FUNCTION_CEverQuest__LMouseUp);
#endif

#ifdef EQ_FUNCTION_CEverQuest__RMouseUp
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__RMouseUp)(void* pThis, int x, int y);
EQ_FUNCTION_AT_ADDRESS(int __cdecl CEverQuest::RMouseUp(int, int), EQ_FUNCTION_CEverQuest__RMouseUp);
#endif

/* CHotButtonWnd */

#ifdef EQ_FUNCTION_CHotButtonWnd__DoHotButton
EQ_FUNCTION_AT_ADDRESS(void CHotButtonWnd::DoHotButton(unsigned int buttonIndex, bool allowAutoRightClick), EQ_FUNCTION_CHotButtonWnd__DoHotButton);
#endif

/* CTextEntryWnd */

#ifdef EQ_FUNCTION_CTextEntryWnd__Activate
typedef int (__thiscall* EQ_FUNCTION_TYPE_CTextEntryWnd__Activate)(void* pThis, void*, int, char*, int, int, char*);
EQ_FUNCTION_AT_ADDRESS(void CTextEntryWnd::Activate(void*, int, char*, int, int, char*), EQ_FUNCTION_CTextEntryWnd__Activate);
#endif

/* CTextOverlay */

#ifdef EQ_FUNCTION_CTextOverlay__DisplayText
typedef int (__thiscall* EQ_FUNCTION_TYPE_CTextOverlay__DisplayText)(void* pThis, const char* text, int textColor, int font, int alpha, int x, int y, int duration);
EQ_FUNCTION_AT_ADDRESS(void CTextOverlay::DisplayText(const char* text, int textColor, int font, int alpha, int x, int y, int duration), EQ_FUNCTION_CTextOverlay__DisplayText);
#endif

/* EQ_Character */

#ifdef EQ_FUNCTION_EQ_Character__eqspa_movement_rate
typedef int (__thiscall* EQ_FUNCTION_TYPE_EQ_Character__eqspa_movement_rate)(void* pThis, int unknown);
EQ_FUNCTION_AT_ADDRESS(void EQ_Character::eqspa_movement_rate(int), EQ_FUNCTION_EQ_Character__eqspa_movement_rate);
#endif

#ifdef EQ_FUNCTION_EQ_Character__UseSkill
typedef int (__thiscall* EQ_FUNCTION_TYPE_EQ_Character__UseSkill)(void* pThis, unsigned char skill, class EQPlayer* targetSpawn);
EQ_FUNCTION_AT_ADDRESS(int EQ_Character::UseSkill(unsigned char, class EQPlayer*), EQ_FUNCTION_EQ_Character__UseSkill);
#endif

/* other */

#ifdef EQ_FUNCTION_CastRay
typedef int (__cdecl* EQ_FUNCTION_TYPE_CastRay)(DWORD originSpawn, float y, float x, float z);
EQ_FUNCTION_AT_ADDRESS(int __cdecl EQ_CastRay(DWORD originSpawn, float y, float x, float z), EQ_FUNCTION_CastRay);
#endif

#ifdef EQ_FUNCTION_CXWnd_DrawColoredRect
typedef int (__cdecl* EQ_FUNCTION_TYPE_CXWnd_DrawColoredRect)(PEQCXRECT rect, DWORD argbColor);
EQ_FUNCTION_AT_ADDRESS(int __cdecl EQ_CXWnd_DrawColoredRect(PEQCXRECT rect, DWORD argbColor), EQ_FUNCTION_CXWnd_DrawColoredRect);
#endif

#ifdef EQ_FUNCTION_DrawNetStatus
typedef int (__cdecl* EQ_FUNCTION_TYPE_DrawNetStatus)(int, unsigned short, unsigned short, unsigned short x, unsigned short y, int, unsigned short, unsigned long, long, unsigned long);
#endif

#ifdef EQ_FUNCTION_get_melee_range
EQ_FUNCTION_AT_ADDRESS(float __cdecl EQ_get_melee_range(DWORD spawn1, DWORD spawn2), EQ_FUNCTION_get_melee_range);
#endif

/* functions */

void EQ_ToggleBool(bool& b)
{
    b = !b;
}

float EQ_CalculateDistance(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

float EQ_CalculateDistance3d(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));
}

void EQ_Rotate2d(float cx, float cy, float& x, float& y, float angle)
{
    float radians = angle * (EQ_PI / 256.0f); // 512 / 2 = 256

    float c = cosf(radians);
    float s = sinf(radians);

    float nx = (c * (x - cx)) - (s * (y - cy)) + cx;
    float ny = (s * (x - cx)) + (c * (y - cy)) + cy;

    x = nx;
    y = ny;
}

bool EQ_IsInGame()
{
    DWORD everquest = EQ_ReadMemory<DWORD>(EQ_POINTER_CEverQuest);

    if (everquest == NULL)
    {
        return false;
    }

    DWORD gameState = EQ_ReadMemory<DWORD>(everquest + 0x5C4);

    if (gameState != EQ_GAME_STATE_IN_GAME)
    {
        return false;
    }

    return true;
}

bool EQ_IsAutoAttackEnabled()
{
    DWORD isAutoAttackEnabled = EQ_ReadMemory<BYTE>(0x00AC1196);

    return (isAutoAttackEnabled == 1);
}

void EQ_WriteToChat(const char* text)
{
    if (EQ_IsInGame() == false)
    {
        return;
    }

    EQ_CLASS_CEverQuest->dsp_chat(text);
}

void EQ_WriteToChat(const char* text, int textColor)
{
    if (EQ_IsInGame() == false)
    {
        return;
    }

    EQ_CLASS_CEverQuest->dsp_chat(text, textColor, false);
}

DWORD EQ_GetCharInfo2()
{
    DWORD charInfo = EQ_ReadMemory<DWORD>(EQ_POINTER_CHAR_INFO);

    if (charInfo == NULL)
    {
        return NULL;
    }

    DWORD charInfo2Info = EQ_ReadMemory<DWORD>(charInfo + 0xF1D8);

    if (charInfo2Info == NULL)
    {
        return NULL;
    }

    DWORD charInfo2 = EQ_ReadMemory<DWORD>(charInfo2Info + 0x04);

    if (charInfo2 == NULL)
    {
        return NULL;
    }

    return charInfo2;
}

DWORD EQ_GetTimer()
{
    DWORD display = EQ_ReadMemory<DWORD>(EQ_POINTER_CDisplay);

    if (display == NULL)
    {
        return NULL;
    }

    DWORD time = EQ_ReadMemory<DWORD>(display + 0x154);

    return time;
}

void EQ_DrawText(const char* text, int x, int y, unsigned int color, unsigned int size)
{
    // color is ARGB format: 0xAARRGGBB

    if (size == 0)
    {
        return;
    }

    DWORD xWndManager = EQ_ReadMemory<DWORD>(EQ_POINTER_CXWndManager);
    if (xWndManager == NULL)
    {
        return;
    }

    DWORD fontManager = EQ_ReadMemory<DWORD>(xWndManager + 0xF8);
    if (fontManager == NULL)
    {
        return;
    }

    DWORD font2 = EQ_ReadMemory<DWORD>(fontManager + 0x08);
    if (font2 == NULL)
    {
        return;
    }
    
    EQ_WriteMemory<DWORD>(0x0046D8FF, color); // write new color

    EQ_WriteMemory<DWORD>(font2 + 0x04, size); // write new font size

    EQ_CLASS_CDisplay->WriteTextHD2(text, x, y, 0); // 0 is the color index that gets overwritten

    EQ_WriteMemory<DWORD>(0x0046D8FF, 0xFF000000); // restore old color

    EQ_WriteMemory<DWORD>(font2 + 0x04, 2); // restore old font size
}


void EQ_DrawColoredRectangle(int x, int y, unsigned int width, unsigned int height, unsigned int argbColor)
{
    EQCXRECT rect;

    // top left
    rect.X1 = x;
    rect.Y1 = y;

    // bottom right
    rect.X2 = x + width;
    rect.Y2 = y + height;

    EQ_CXWnd_DrawColoredRect(&rect, argbColor);
}

void EQ_DrawQuad(float x, float y, float width, float height, unsigned int argbColor)
{
    EQRECT rect;

    // top left
    rect.X1 = x;
    rect.Y1 = y;
    rect.Z1 = 0.0f;

    // top right
    rect.X2 = x + width;
    rect.Y2 = y;
    rect.Z2 = 0.0f;

    // bottom right
    rect.X3 = x + width;
    rect.Y3 = y + height;
    rect.Z3 = 0.0f;

    // bottom left
    rect.X4 = x;
    rect.Y4 = y + height;
    rect.Z4 = 0.0f;

    EQGraphicsDLL__DrawQuad(EQ_CLASS_EQGraphicsDLL, &rect, argbColor);
}

bool EQ_DrawLine(float x1, float y1, float z1, float x2, float y2, float z2, unsigned int argbColor)
{
    if (EQGraphicsDLL__DrawLine == NULL)
    {
        return false;
    }

    EQLINE lineBegin;
    lineBegin.X = x1;
    lineBegin.Y = y1;
    lineBegin.Z = z1;

    EQLINE lineEnd;
    lineEnd.X = x2;
    lineEnd.Y = y2;
    lineEnd.Z = z2;

    EQGraphicsDLL__DrawLine(EQ_CLASS_EQGraphicsDLL, &lineBegin, &lineEnd, argbColor);

    return true;
}

bool EQ_WorldSpaceToScreenSpace(float worldX, float worldY, float worldZ, int& screenX, int& screenY)
{
    DWORD display = EQ_ReadMemory<DWORD>(EQ_POINTER_CDisplay);
    if (display == NULL)
    {
        return false;
    }

    DWORD cameraData = EQ_ReadMemory<DWORD>(display + 0x118);
    if (cameraData == NULL)
    {
        return false;
    }

    FLOAT cameraData_0x1A0 = EQ_ReadMemory<FLOAT>(cameraData + 0x0C); // boolean for if the world should be drawn
    FLOAT cameraData_0x1A4 = EQ_ReadMemory<FLOAT>(cameraData + 0x10);
    FLOAT cameraData_0x1A8 = EQ_ReadMemory<FLOAT>(cameraData + 0x14);
    FLOAT cameraData_0x198 = EQ_ReadMemory<FLOAT>(cameraData + 0x38); // screen width  divided by two
    FLOAT cameraData_0x19C = EQ_ReadMemory<FLOAT>(cameraData + 0x3C); // screen height divided by two
    FLOAT cameraData_0x22C = EQ_ReadMemory<FLOAT>(cameraData + 0x48);
    FLOAT cameraData_0x238 = EQ_ReadMemory<FLOAT>(cameraData + 0x54);
    FLOAT cameraData_0x244 = EQ_ReadMemory<FLOAT>(cameraData + 0x60);
    FLOAT cameraData_0x260 = EQ_ReadMemory<FLOAT>(cameraData + 0x84);
    FLOAT cameraData_0x264 = EQ_ReadMemory<FLOAT>(cameraData + 0x88);
    FLOAT cameraData_0x268 = EQ_ReadMemory<FLOAT>(cameraData + 0x8C); // always zero?
    FLOAT cameraData_0x26C = EQ_ReadMemory<FLOAT>(cameraData + 0x90);
    FLOAT cameraData_0x270 = EQ_ReadMemory<FLOAT>(cameraData + 0x94);
    FLOAT cameraData_0x274 = EQ_ReadMemory<FLOAT>(cameraData + 0x98);
    FLOAT cameraData_0x1AC = EQ_ReadMemory<FLOAT>(cameraData + 0xBC);
    FLOAT cameraData_0x1B0 = EQ_ReadMemory<FLOAT>(cameraData + 0xC0);
    FLOAT cameraData_0x1B4 = EQ_ReadMemory<FLOAT>(cameraData + 0xC4);

    float v4 = cameraData_0x1AC - worldY; // changed add to substract because Seeds of Destruction has backwards coordinates
    float v5 = cameraData_0x1B0 - worldX; // changed add to substract because Seeds of Destruction has backwards coordinates
    float v6 = cameraData_0x1B4 - worldZ; // changed add to substract because Seeds of Destruction has backwards coordinates

    float v10 = v6 * cameraData_0x268 + v5 * cameraData_0x264 + v4 * cameraData_0x260;
    float v11 = v6 * cameraData_0x274 + v5 * cameraData_0x270 + v4 * cameraData_0x26C;
    float v7  = v6 * cameraData_0x244 + v5 * cameraData_0x238 + v4 * cameraData_0x22C;

    if (v7 >= cameraData_0x1A0) // changed <= to >= because Seeds of Destruction has backwards coordinates
    {
        screenX = -1;
        screenY = -1;

        return false;
    }
    
    float v8 = 1.0f / v7;

    float a3 = v10 * v8 + cameraData_0x198;
    float a4 = cameraData_0x19C - v8 * v11;

    screenX = (int)a3;
    screenY = (int)a4;

    return true;
}

std::string EQ_GetSpellNameById(int spellId)
{
    if (spellId > 5009) //21000)
    {
        return "Unknown Spell";
    }

    DWORD spellManager = EQ_ReadMemory<DWORD>(EQ_POINTER_SPELL_MANAGER);
    if (spellManager == NULL)
    {
        return "Unknown Spell";
    }

    DWORD spell = EQ_ReadMemory<DWORD>(spellManager + (0x68 + (spellId * 4)));
    if (spell == NULL)
    {
        return "Unknown Spell";
    }

    char spellName[0x40] = {0};
    memcpy(spellName, (LPVOID)(spell + 0x247), sizeof(spellName));

    return spellName;
}

#endif // EQSOD_FUNCTIONS_H

