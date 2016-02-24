#ifndef EQSOD_FUNCTIONS_H
#define EQSOD_FUNCTIONS_H

#include <string>

#include <cstdint>
#include <cstring>
#include <cmath>

#include "eqsod.h"

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
    T* buffer = (T*)address;
    return (*buffer);
}

template <class T>
void EQ_WriteMemory(DWORD address, T value)
{
    T* buffer = (T*)address;
    *buffer = value;
}

template <class T>
T EQ_ReadMemoryProtected(DWORD address)
{
    DWORD oldProtect;
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, sizeof(T), PAGE_READWRITE, &oldProtect);

    T* buffer = (T*)address;

    VirtualProtectEx(GetCurrentProcess(), (LPVOID)address, sizeof(T), oldProtect, &oldProtect);

    return (*buffer);
}

template <class T>
void EQ_WriteMemoryProtected(DWORD address, T value)
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
class EQ_Guilds;

class CXStr;
class CSidlScreenWnd;
class CXWndManager;
class CXWnd;
class CXPoint;
class CXRect;

class CDisplay;
class CEverQuest;
class CHotButtonWnd;
class CLootWnd;
class CMapViewWnd;
class CTextEntryWnd;
class CTextOverlay;
class EQPlayer;
class EQPlayerManager;
class EQSwitch;
class EQ_Character;
class MapViewMap;

class EQGraphicsDLL
{
public:
    int EQGraphicsDLL::DrawLine(EQLINE* lineBegin, EQLINE* lineEnd, DWORD argbColor);
    int EQGraphicsDLL::DrawQuad(EQRECT* rect, DWORD argbColor);
};

EQGraphicsDLL** EQ_ppEQGraphicsDLL = (EQGraphicsDLL**)EQ_POINTER_EQGraphicsDLL;
#define EQ_EQGraphicsDLL (*EQ_ppEQGraphicsDLL)

class EQ_Guilds
{
public:
    char* EQ_Guilds::GetGuildNameById(DWORD id);
};

EQ_Guilds* EQ_ppEQ_Guilds = (EQ_Guilds*)EQ_GUILDS;
#define EQ_EQ_Guilds (*EQ_ppEQ_Guilds)

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
    void CDisplay::CreatePlayerActor(DWORD spawnInfo, int a2, int a3, int a4, int a5);
    void CDisplay::DeleteActor(DWORD actorInstance);
    void CDisplay::SetViewActor(DWORD actorInstance);
    void CDisplay::SetViewActorByName(const char* spawnName);
    static int __cdecl CDisplay::WriteTextHD2(const char* text, int x, int y, int color);
};

CDisplay** EQ_ppCDisplay = (CDisplay**)EQ_POINTER_CDisplay;
#define EQ_CDisplay (*EQ_ppCDisplay)

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
    char* CEverQuest::GetClassDesc(int);
    char* CEverQuest::GetClassThreeLetterCode(int);
    char* CEverQuest::GetDeityDesc(int);
    char* CEverQuest::GetRaceDesc(int);
    void CEverQuest::InterpretCmd(DWORD spawnInfo, const char* text);
    int __cdecl CEverQuest::LMouseUp(int x, int y);
    int __cdecl CEverQuest::RMouseUp(int x, int y);
    void CEverQuest::MoveToZone(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8);
    void CEverQuest::SetGameState(int state);
    void CEverQuest::StartCasting(int a1);
};

CEverQuest** EQ_ppCEverQuest = (CEverQuest**)EQ_POINTER_CEverQuest;
#define EQ_CEverQuest (*EQ_ppCEverQuest)

class CHotButtonWnd : public CSidlScreenWnd
{
public:
    void CHotButtonWnd::DoHotButton(unsigned int buttonIndex, bool allowAutoRightClick);
};

CHotButtonWnd** EQ_ppCHotButtonWnd = (CHotButtonWnd**)EQ_POINTER_CHotButtonWnd;
#define EQ_CHotButtonWnd (*EQ_ppCHotButtonWnd)

class CLootWnd : public CSidlScreenWnd
{
public:
    void CLootWnd::Deactivate(void);
    void CLootWnd::RequestLootSlot(int slotIndex, bool autoLoot);
};

CLootWnd** EQ_ppCLootWnd = (CLootWnd**)EQ_POINTER_CLootWnd;
#define EQ_CLootWnd (*EQ_ppCLootWnd)

class CMapViewWnd : public CSidlScreenWnd
{
public:
    CMapViewWnd::~CMapViewWnd();
    void CMapViewWnd::DrawMap(int a1, int a2, int a3, int a4);
};

CMapViewWnd** EQ_ppCMapViewWnd = (CMapViewWnd**)EQ_POINTER_CMapViewWnd;
#define EQ_CMapViewWnd (*EQ_ppCMapViewWnd)

class CTextEntryWnd
{
public:
    void CTextEntryWnd::Activate(void* parentWindow, int a2, char* displayText, int a4, int a5, char* defaultText);
};

CTextEntryWnd** EQ_ppCTextEntryWnd = (CTextEntryWnd**)EQ_POINTER_CTextEntryWnd;
#define EQ_CTextEntryWnd (*EQ_ppCTextEntryWnd)

class CTextOverlay
{
public:
    void CTextOverlay::DisplayText(const char* text, int textColor, int font, int alpha, int x,int y, int duration); // alpha is 0-255, duration is in milliseconds
};

CTextOverlay** EQ_ppCTextOverlay = (CTextOverlay**)EQ_POINTER_CTextOverlay;
#define EQ_CTextOverlay (*EQ_ppCTextOverlay)

class EQPlayer
{
public:
    void EQPlayer::ChangeHeight(float height);
    void EQPlayer::ChangePosition(BYTE standingState);
    void EQPlayer::do_change_form(int changeForm);
    void EQPlayer::FacePlayer(DWORD spawnInfo);
    void EQPlayer::SetRace(int raceId);
    int EQPlayer::UpdateAppearance(int a1, int a2, int a3);
};

class EQPlayerManager
{
public:
    DWORD EQPlayerManager::GetSpawnByID(int spawnId);
    DWORD EQPlayerManager::GetSpawnByName(char* spawnName);
};

EQPlayerManager** EQ_ppEQPlayerManager = (EQPlayerManager**)EQ_POINTER_EQPlayerManager;
#define EQ_EQPlayerManager (*EQ_ppEQPlayerManager)

class EQSwitch
{
public:
    void EQSwitch::ChangeState(BYTE state, int a2, int a3);
};

class EQ_Character
{
public:
    void EQ_Character::eqspa_movement_rate(int unknown);
    int EQ_Character::UseSkill(unsigned char skill, class EQPlayer* targetSpawn);
};

EQ_Character** EQ_ppEQ_Character = (EQ_Character**)EQ_POINTER_EQ_Character;
#define EQ_EQ_Character (*EQ_ppEQ_Character)

class MapViewMap
{
public:
    DWORD MapViewMap::AddLabel(float x, float y, float z, DWORD argbColor, int size, const char* text);
    void MapViewMap::RemoveLabel(DWORD mapLabelInfo);
    void MapViewMap::Save(char* shortZoneName);
    void MapViewMap::SaveEx(char* shortZoneName, int layer);
};

/* EQGraphicsDLL */

typedef int (__thiscall* EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawLine)(void* pThis, EQLINE* lineBegin, EQLINE* lineEnd, DWORD argbColor);
EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawLine EQGraphicsDLL__DrawLine = NULL;

typedef int (__thiscall* EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawQuad)(void* pThis, EQRECT* rect, DWORD argbColor);
EQ_FUNCTION_TYPE_EQGraphicsDLL__DrawQuad EQGraphicsDLL__DrawQuad = NULL;

/* EQ_Guilds */

#ifdef EQ_FUNCTION_EQ_Guilds__GetGuildNameById
EQ_FUNCTION_AT_ADDRESS(char* EQ_Guilds::GetGuildNameById(DWORD id), EQ_FUNCTION_EQ_Guilds__GetGuildNameById);
#endif

/* CDisplay */

#ifdef EQ_FUNCTION_CDisplay__CreatePlayerActor
typedef int (__thiscall* EQ_FUNCTION_TYPE_CDisplay__CreatePlayerActor)(void* pThis, DWORD spawnInfo, int a2, int a3, int a4, int a5);
EQ_FUNCTION_AT_ADDRESS(void CDisplay::CreatePlayerActor(DWORD spawnInfo, int a2, int a3, int a4, int a5), EQ_FUNCTION_CDisplay__CreatePlayerActor);
#endif

#ifdef EQ_FUNCTION_CDisplay__DeleteActor
typedef int (__thiscall* EQ_FUNCTION_TYPE_CDisplay__DeleteActor)(void* pThis, DWORD actorInstance);
EQ_FUNCTION_AT_ADDRESS(void CDisplay::DeleteActor(DWORD actorInstance), EQ_FUNCTION_CDisplay__DeleteActor);
#endif

#ifdef EQ_FUNCTION_CDisplay__SetViewActor
typedef int (__thiscall* EQ_FUNCTION_TYPE_CDisplay__SetViewActor)(void* pThis, DWORD actorInstance);
EQ_FUNCTION_AT_ADDRESS(void CDisplay::SetViewActor(DWORD actorInstance), EQ_FUNCTION_CDisplay__SetViewActor);
#endif

#ifdef EQ_FUNCTION_CDisplay__SetViewActorByName
typedef int (__thiscall* EQ_FUNCTION_TYPE_CDisplay__SetViewActorByName)(void* pThis, const char* spawnName);
EQ_FUNCTION_AT_ADDRESS(void CDisplay::SetViewActorByName(const char* spawnName), EQ_FUNCTION_CDisplay__SetViewActorByName);
#endif

#ifdef EQ_FUNCTION_CDisplay__WriteTextHD2
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

#ifdef EQ_FUNCTION_CEverQuest__GetClassDesc
EQ_FUNCTION_AT_ADDRESS(char* CEverQuest::GetClassDesc(int class_), EQ_FUNCTION_CEverQuest__GetClassDesc);
#endif

#ifdef EQ_FUNCTION_CEverQuest__GetClassThreeLetterCode
EQ_FUNCTION_AT_ADDRESS(char* CEverQuest::GetClassThreeLetterCode(int class_), EQ_FUNCTION_CEverQuest__GetClassThreeLetterCode);
#endif

#ifdef EQ_FUNCTION_CEverQuest__GetDeityDesc
EQ_FUNCTION_AT_ADDRESS(char* CEverQuest::GetDeityDesc(int deity), EQ_FUNCTION_CEverQuest__GetDeityDesc);
#endif

#ifdef EQ_FUNCTION_CEverQuest__GetRaceDesc
EQ_FUNCTION_AT_ADDRESS(char* CEverQuest::GetRaceDesc(int race), EQ_FUNCTION_CEverQuest__GetRaceDesc);
#endif

#ifdef EQ_FUNCTION_CEverQuest__InterpretCmd
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__InterpretCmd)(void* pThis, DWORD spawnInfo, const char* text);
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::InterpretCmd(DWORD spawnInfo, const char* text), EQ_FUNCTION_CEverQuest__InterpretCmd);
#endif

#ifdef EQ_FUNCTION_CEverQuest__LMouseUp
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__LMouseUp)(void* pThis, int x, int y);
EQ_FUNCTION_AT_ADDRESS(int __cdecl CEverQuest::LMouseUp(int, int), EQ_FUNCTION_CEverQuest__LMouseUp);
#endif

#ifdef EQ_FUNCTION_CEverQuest__RMouseUp
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__RMouseUp)(void* pThis, int x, int y);
EQ_FUNCTION_AT_ADDRESS(int __cdecl CEverQuest::RMouseUp(int, int), EQ_FUNCTION_CEverQuest__RMouseUp);
#endif

#ifdef EQ_FUNCTION_CEverQuest__MoveToZone
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__MoveToZone)(void* pThis, int, int, int, int, int, int, int, int);
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::MoveToZone(int, int, int, int, int, int, int, int), EQ_FUNCTION_CEverQuest__MoveToZone);
#endif

#ifdef EQ_FUNCTION_CEverQuest__SetGameState
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__SetGameState)(void* pThis, int state);
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::SetGameState(int state), EQ_FUNCTION_CEverQuest__SetGameState);
#endif

#ifdef EQ_FUNCTION_CEverQuest__StartCasting
typedef int (__thiscall* EQ_FUNCTION_TYPE_CEverQuest__StartCasting)(void* pThis, int a1);
EQ_FUNCTION_AT_ADDRESS(void CEverQuest::StartCasting(int a1), EQ_FUNCTION_CEverQuest__StartCasting);
#endif

/* CMapViewWnd */

#ifdef EQ_FUNCTION_CMapViewWnd__dCMapViewWnd
typedef int (__thiscall* EQ_FUNCTION_TYPE_CMapViewWnd__dCMapViewWnd)(void* pThis);
EQ_FUNCTION_AT_ADDRESS(CMapViewWnd::~CMapViewWnd(), EQ_FUNCTION_CMapViewWnd__dCMapViewWnd);
#endif

#ifdef EQ_FUNCTION_CMapViewWnd__DrawMap
typedef int (__thiscall* EQ_FUNCTION_TYPE_CMapViewWnd__DrawMap)(void* pThis, int a1, int a2, int a3, int a4);
EQ_FUNCTION_AT_ADDRESS(void CMapViewWnd::DrawMap(int, int, int, int), EQ_FUNCTION_CMapViewWnd__DrawMap);
#endif

/* CHotButtonWnd */

#ifdef EQ_FUNCTION_CHotButtonWnd__DoHotButton
EQ_FUNCTION_AT_ADDRESS(void CHotButtonWnd::DoHotButton(unsigned int buttonIndex, bool allowAutoRightClick), EQ_FUNCTION_CHotButtonWnd__DoHotButton);
#endif

/* CLootWnd */

#ifdef EQ_FUNCTION_CLootWnd__Deactivate
EQ_FUNCTION_AT_ADDRESS(void CLootWnd::Deactivate(void), EQ_FUNCTION_CLootWnd__Deactivate);
#endif

#ifdef EQ_FUNCTION_CLootWnd__RequestLootSlot
EQ_FUNCTION_AT_ADDRESS(void CLootWnd::RequestLootSlot(int slotIndex, bool autoLoot), EQ_FUNCTION_CLootWnd__RequestLootSlot);
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

/* EQPlayer */

#ifdef EQ_FUNCTION_EQPlayer__ChangeHeight
EQ_FUNCTION_AT_ADDRESS(void EQPlayer::ChangeHeight(float height), EQ_FUNCTION_EQPlayer__ChangeHeight);
#endif

#ifdef EQ_FUNCTION_EQPlayer__ChangePosition
typedef int (__thiscall* EQ_FUNCTION_TYPE_EQPlayer__ChangePosition)(void* pThis, BYTE standingState);
EQ_FUNCTION_AT_ADDRESS(void EQPlayer::ChangePosition(BYTE standingState), EQ_FUNCTION_EQPlayer__ChangePosition);
#endif

#ifdef EQ_FUNCTION_EQPlayer__do_change_form
typedef int (__thiscall* EQ_FUNCTION_TYPE_EQPlayer__do_change_form)(void* pThis, int changeForm);
EQ_FUNCTION_AT_ADDRESS(void EQPlayer::do_change_form(int changeForm), EQ_FUNCTION_EQPlayer__do_change_form);
#endif

#ifdef EQ_FUNCTION_EQPlayer__FacePlayer
EQ_FUNCTION_AT_ADDRESS(void EQPlayer::FacePlayer(DWORD spawnInfo), EQ_FUNCTION_EQPlayer__FacePlayer);
#endif

#ifdef EQ_FUNCTION_EQPlayer__SetRace
typedef int (__thiscall* EQ_FUNCTION_TYPE_EQPlayer__SetRace)(void* pThis, int raceId);
EQ_FUNCTION_AT_ADDRESS(void EQPlayer::SetRace(int raceId), EQ_FUNCTION_EQPlayer__SetRace);
#endif

#ifdef EQ_FUNCTION_EQPlayer__UpdateAppearance
typedef int (__thiscall* EQ_FUNCTION_TYPE_EQPlayer__UpdateAppearance)(void* pThis, int a1, int a2, int a3);
EQ_FUNCTION_AT_ADDRESS(int EQPlayer::UpdateAppearance(int a1, int a2, int a3), EQ_FUNCTION_EQPlayer__UpdateAppearance);
#endif

/* EQPlayerManager */

#ifdef EQ_FUNCTION_EQPlayerManager__GetSpawnByID
EQ_FUNCTION_AT_ADDRESS(DWORD EQPlayerManager::GetSpawnByID(int spawnId), EQ_FUNCTION_EQPlayerManager__GetSpawnByID);
#endif

#ifdef EQ_FUNCTION_EQPlayerManager__GetSpawnByName
EQ_FUNCTION_AT_ADDRESS(DWORD EQPlayerManager::GetSpawnByName(char* spawnName), EQ_FUNCTION_EQPlayerManager__GetSpawnByName);
#endif

/* EQSwitch */

#ifdef EQ_FUNCTION_EQSwitch__ChangeState
EQ_FUNCTION_AT_ADDRESS(void EQSwitch::ChangeState(BYTE state, int a2, int a3), EQ_FUNCTION_EQSwitch__ChangeState);
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

/* MapViewMap */

#ifdef EQ_FUNCTION_MapViewMap__AddLabel
typedef int (__thiscall* EQ_FUNCTION_TYPE_MapViewMap__AddLabel)(void* pThis, float x, float y, float z, DWORD argbColor, int size, const char* text);
EQ_FUNCTION_AT_ADDRESS(DWORD MapViewMap::AddLabel(float x, float y, float z, DWORD argbColor, int size, const char* text), EQ_FUNCTION_MapViewMap__AddLabel);
#endif

#ifdef EQ_FUNCTION_MapViewMap__RemoveLabel
typedef int (__thiscall* EQ_FUNCTION_TYPE_MapViewMap__RemoveLabel)(void* pThis, DWORD mapLabelInfo);
EQ_FUNCTION_AT_ADDRESS(void MapViewMap::RemoveLabel(DWORD mapLabelInfo), EQ_FUNCTION_MapViewMap__RemoveLabel);
#endif

#ifdef EQ_FUNCTION_MapViewMap__Save
typedef int (__thiscall* EQ_FUNCTION_TYPE_MapViewMap__Save)(void* pThis, char* shortZoneName);
EQ_FUNCTION_AT_ADDRESS(void MapViewMap::Save(char* shortZoneName), EQ_FUNCTION_MapViewMap__Save);
#endif

#ifdef EQ_FUNCTION_MapViewMap__SaveEx
typedef int (__thiscall* EQ_FUNCTION_TYPE_MapViewMap__SaveEx)(void* pThis, char* shortZoneName, int layer);
EQ_FUNCTION_AT_ADDRESS(void MapViewMap::SaveEx(char* shortZoneName, int layer), EQ_FUNCTION_MapViewMap__SaveEx);
#endif

/* other */

#ifdef EQ_FUNCTION_Exit
typedef int (__cdecl* EQ_FUNCTION_TYPE_Exit)();
EQ_FUNCTION_AT_ADDRESS(void EQ_Exit(), EQ_FUNCTION_Exit);
#endif

#ifdef EQ_FUNCTION_ResetDevice
EQ_FUNCTION_AT_ADDRESS(int __cdecl EQ_ResetDevice(), EQ_FUNCTION_ResetDevice);
#endif

#ifdef EQ_FUNCTION_WindowResize
EQ_FUNCTION_AT_ADDRESS(int __cdecl EQ_WindowResize(WPARAM wparam, LPARAM lparam), EQ_FUNCTION_WindowResize);
#endif

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

#ifdef EQ_FUNCTION_get_bearing
EQ_FUNCTION_AT_ADDRESS(float __cdecl EQ_get_bearing(float y1, float x1, float y2, float x2), EQ_FUNCTION_get_bearing);
#endif

#ifdef EQ_FUNCTION_ExecuteCmd
typedef int (__cdecl* EQ_FUNCTION_TYPE_ExecuteCmd)(DWORD command, BOOL hold, PVOID unknown);
EQ_FUNCTION_AT_ADDRESS(void EQ_ExecuteCmd(DWORD command, BOOL hold, PVOID unknown), EQ_FUNCTION_ExecuteCmd);
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

bool EQ_IsNetStatusEnabled()
{
    DWORD b = EQ_ReadMemory<BYTE>(EQ_BOOL_NET_STATUS);

    return (b == 1);
}

bool EQ_IsSafeLockEnabled()
{
    DWORD b = EQ_ReadMemory<BYTE>(EQ_BOOL_SAFELOCK);

    return (b == 1);
}

bool EQ_IsScreenshotBeingTaken()
{
    DWORD b = EQ_ReadMemory<BYTE>(EQ_BOOL_SCREENSHOT);

    return (b == 1);
}

bool EQ_IsAutoAttackEnabled()
{
    DWORD b = EQ_ReadMemory<BYTE>(EQ_BOOL_AUTO_ATTACK);

    return (b == 1);
}

bool EQ_IsAutoFireEnabled()
{
    DWORD b = EQ_ReadMemory<BYTE>(EQ_BOOL_AUTO_FIRE);

    return (b == 1);
}

bool EQ_IsKeyShiftPressed()
{
    DWORD b = EQ_ReadMemory<BYTE>(EQ_BOOL_KEYBOARD_SHIFT);

    return (b == 1);
}

bool EQ_IsKeyAltPressed()
{
    DWORD b = EQ_ReadMemory<BYTE>(EQ_BOOL_KEYBOARD_ALT);

    return (b == 1);
}

bool EQ_IsKeyControlPressed()
{
    DWORD b = EQ_ReadMemory<BYTE>(EQ_BOOL_KEYBOARD_CONTROL);

    return (b == 1);
}

DWORD EQ_GetExitStatus()
{
    return EQ_ReadMemory<BYTE>(EQ_EXIT_STATUS);
}

void EQ_WriteToChat(const char* text)
{
    if (EQ_IsInGame() == false)
    {
        return;
    }

    EQ_CEverQuest->dsp_chat(text);
}

void EQ_WriteToChat(const char* text, int textColor)
{
    if (EQ_IsInGame() == false)
    {
        return;
    }

    EQ_CEverQuest->dsp_chat(text, textColor, false);
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

void EQ_SetSpawnCollisionRadius(DWORD spawnInfo, float radius)
{
    DWORD actorInfo = EQ_ReadMemory<DWORD>(spawnInfo + 0xF84);
    if (actorInfo == NULL)
    {
        return;
    }

    EQ_WriteMemory<FLOAT>(actorInfo + 0x104, radius);
}

DWORD EQ_GetZoneId()
{
    DWORD charInfo = EQ_ReadMemory<DWORD>(EQ_POINTER_CHAR_INFO);
    if (charInfo == NULL)
    {
        return NULL;
    }

    WORD zoneId = EQ_ReadMemory<WORD>(charInfo + 0xF2D4);

    return zoneId;
}

bool EQ_IsZoneCity()
{
    // TODO

    DWORD zoneId = EQ_GetZoneId();

    if
    (
        zoneId == 1   || // south newport
        zoneId == 2   || // north newport
        zoneId == 3   || // surefall
        zoneId == 23  || // erudin palace
        zoneId == 24  || // erudin
        zoneId == 29  || // halas
        zoneId == 55  || // underhill
        zoneId == 61  || // athica a
        zoneId == 62  || // athica b
        zoneId == 74  || // sadri malath
        zoneId == 114    // thurgadin
    )
    {
        return true;
    }

    return false;
}

bool EQ_IsPlayerCastingSpell()
{
    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);
    if (playerSpawn == NULL)
    {
        return false;
    }

    DWORD spellCastingTimer = EQ_ReadMemory<DWORD>(playerSpawn + 0x448);
    if (spellCastingTimer != 0)
    {
        return true;
    }

    return false;
}

bool EQ_IsSpawnInGroup(DWORD spawnInfo)
{
    if (spawnInfo == NULL)
    {
        return false;
    }

    DWORD charInfo = EQ_ReadMemory<DWORD>(EQ_POINTER_CHAR_INFO);
    if (charInfo == NULL)
    {
        return false;
    }

    DWORD group = EQ_ReadMemory<DWORD>(charInfo + 0xF1B8);
    if (group == NULL)
    {
        return false;
    }

    for (size_t i = 1; i < 7; i++)
    {
        DWORD groupMember = EQ_ReadMemory<DWORD>(group + (i * 4));
        if (groupMember == NULL)
        {
            continue;
        }

        DWORD groupMemberSpawn = EQ_ReadMemory<DWORD>(groupMember + 0x28);
        if (groupMemberSpawn == NULL)
        {
            continue;
        }

        if (groupMemberSpawn == spawnInfo)
        {
            return true;
        }
    }

    return false;
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

bool EQ_IsHideCorpseLootedEnabled()
{
    DWORD display = EQ_ReadMemory<DWORD>(EQ_POINTER_CDisplay);
    if (display == NULL)
    {
        return NULL;
    }

    DWORD hideCorpseLooted = EQ_ReadMemory<BYTE>(display + 0x14);

    return (hideCorpseLooted == 1);
}

void EQ_SetHideCorpseLooted(bool b)
{
    DWORD display = EQ_ReadMemory<DWORD>(EQ_POINTER_CDisplay);
    if (display == NULL)
    {
        return;
    }

    BYTE value = 0x00;

    if (b == true)
    {
        value = 0x01;
    }

    EQ_WriteMemory<BYTE>(display + 0x14, value);
}

void EQ_StringReplaceUnderscoresWithSpaces(char* str)
{
    while (*str)
    {
        if (*str == '_')
        {
            *str = ' ';
        }

        str++;
    }
}

void EQ_StringReplaceSpacesWithUnderscores(char* str)
{
    while (*str)
    {
        if (*str == ' ')
        {
            *str = '_';
        }

        str++;
    }
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
    
    EQ_WriteMemoryProtected<DWORD>(0x0046D8FF, color); // write new color

    EQ_WriteMemory<DWORD>(font2 + 0x04, size); // write new font size

    EQ_CDisplay->WriteTextHD2(text, x, y, 0); // 0 is the color index that gets overwritten

    EQ_WriteMemoryProtected<DWORD>(0x0046D8FF, 0xFF000000); // restore old color

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

    EQGraphicsDLL__DrawQuad(EQ_EQGraphicsDLL, &rect, argbColor);
}

void EQ_DrawQuadEx(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int argbColor)
{
    EQRECT rect;

    // top left
    rect.X1 = x1;
    rect.Y1 = y1;
    rect.Z1 = 0.0f;

    // top right
    rect.X2 = x2;
    rect.Y2 = y2;
    rect.Z2 = 0.0f;

    // bottom right
    rect.X3 = x3;
    rect.Y3 = y3;
    rect.Z3 = 0.0f;

    // bottom left
    rect.X4 = x4;
    rect.Y4 = y4;
    rect.Z4 = 0.0f;

    EQGraphicsDLL__DrawQuad(EQ_EQGraphicsDLL, &rect, argbColor);
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

    EQGraphicsDLL__DrawLine(EQ_EQGraphicsDLL, &lineBegin, &lineEnd, argbColor);

    return true;
}

DWORD EQ_GetCameraData()
{
    DWORD display = EQ_ReadMemory<DWORD>(EQ_POINTER_CDisplay);
    if (display == NULL)
    {
        return NULL;
    }

    DWORD cameraData = EQ_ReadMemory<DWORD>(display + 0x118);
    if (cameraData == NULL)
    {
        return NULL;
    }

    return cameraData;
}

bool EQ_WorldSpaceToScreenSpace(float worldX, float worldY, float worldZ, int& screenX, int& screenY)
{
    DWORD cameraData = EQ_GetCameraData();
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

    // point is offscreen
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

    int windowWidth  = EQ_ReadMemory<DWORD>(EQ_WINDOW_WIDTH);
    int windowHeight = EQ_ReadMemory<DWORD>(EQ_WINDOW_HEIGHT);

    if (screenX < 0 || screenX > windowWidth || screenY < 0 || screenY > windowHeight)
    {
        screenX = -1;
        screenY = -1;

        return false;
    }

    return true;
}

void EQ_SetFieldOfView(float fov)
{
    DWORD cameraData = EQ_GetCameraData();
    if (cameraData == NULL)
    {
        return;
    }

    if (fov <= 0.0f)
    {
        fov = EQ_FIELD_OF_VIEW_DEFAULT;
    }

    EQ_WriteMemory<FLOAT>(cameraData + 0x04, fov);
}

void EQ_SetDrawDistance(float distance)
{
    DWORD cameraData = EQ_GetCameraData();
    if (cameraData == NULL)
    {
        return;
    }

    if (distance <= 0.0f)
    {
        distance = EQ_DRAW_DISTANCE_DEFAULT;
    }

    EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_MIN_CLIP, distance);
    EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_MAX_CLIP, distance);

    EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_MIN_CLIP, distance);
    EQ_WriteMemory<FLOAT>(EQ_ZONEINFO_FOG_MAX_CLIP, distance);

    EQ_WriteMemory<FLOAT>(EQ_DRAW_DISTANCE_MAX, distance);

    EQ_WriteMemory<FLOAT>(cameraData + 0x14, distance);
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

DWORD EQ_GetNumPlayersInZone()
{
    DWORD numPlayers = 0;

    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        int spawnType = EQ_ReadMemory<BYTE>(spawn + 0x125);

        if (spawnType == EQ_SPAWN_TYPE_PLAYER)
        {
            int spawnLevel = EQ_ReadMemory<BYTE>(spawn + 0x315);

            if (spawnLevel > 0 && spawnLevel < 100)
            {
                char spawnName[0x40] = {0};
                memcpy(spawnName, (LPVOID)(spawn + 0xA4), sizeof(spawnName));

                if (strlen(spawnName) > 2)
                {
                    numPlayers++;
                }
            }
        }

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    return numPlayers;
}

bool EQ_DoesSpawnExist(DWORD spawnInfo)
{
    DWORD numPlayers = 0;

    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        if (spawn == spawnInfo)
        {
            return true;
        }

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    return false;
}

DWORD EQ_GetSpawnByName(const char* name)
{
    DWORD spawn = EQ_ReadMemory<DWORD>(EQ_POINTER_FIRST_SPAWN_INFO);

    while (spawn)
    {
        char spawnName[0x40] = {0};
        memcpy(spawnName, (LPVOID)(spawn + 0xA4), sizeof(spawnName)); // 0xA4 Name, not 0xE4 DisplayName

        if (strcmp(spawnName, name) == 0)
        {
            return spawn;
        }

        spawn = EQ_ReadMemory<DWORD>(spawn + 0x08); // next
    }

    return false;
}

DWORD EQ_GetSpawnAnimation(DWORD spawnInfo)
{
    if (spawnInfo == NULL)
    {
        return NULL;
    }

    return EQ_ReadMemory<DWORD>(spawnInfo + 0x1004);
}

struct _EQMAPLABEL* EQ_MapWindow_AddLabel(EQMAPLABEL* mapLabel)
{
    DWORD mapViewWnd = EQ_ReadMemory<DWORD>(EQ_POINTER_CMapViewWnd);
    if (mapViewWnd == NULL)
    {
        return NULL;
    }

    struct _EQMAPLABEL* node = EQ_ReadMemory<struct _EQMAPLABEL*>(mapViewWnd + 0x2D0);
    if (node == NULL)
    {
        return NULL;
    }

    while (node->Next != NULL)
    {
        node = node->Next;
    }

    node->Next = (struct _EQMAPLABEL*)malloc(sizeof(struct _EQMAPLABEL));

    node->Next->Next = NULL;
    node->Next->Prev = node;

    node->Next->Location = mapLabel->Location;
    node->Next->Color = mapLabel->Color;
    node->Next->Size = mapLabel->Size;
    node->Next->Label = mapLabel->Label;
    node->Next->Layer = mapLabel->Layer;
    node->Next->Width = mapLabel->Width;
    node->Next->Height = mapLabel->Height;

    node->Next->Data = mapLabel->Data;

    return node;
}

void EQ_MapWindow_RemoveLabel(DWORD data)
{
    DWORD mapViewWnd = EQ_ReadMemory<DWORD>(EQ_POINTER_CMapViewWnd);
    if (mapViewWnd == NULL)
    {
        return;
    }

    struct _EQMAPLABEL* node = EQ_ReadMemory<struct _EQMAPLABEL*>(mapViewWnd + 0x2D0);
    if (node == NULL)
    {
        return;
    }

    struct _EQMAPLABEL* first = node;

    //std::cout << "first: " << first->Label << std::endl;

    while (node->Next != NULL)
    {
        node = node->Next;
    }

    struct _EQMAPLABEL* last = node;

    //std::cout << "last: " << last->Label << std::endl;

    node = first;

    while (node != NULL)
    {
        struct _EQMAPLABEL* after  = node->Next;
        struct _EQMAPLABEL* before = node->Prev;

        if (node->Data == data)
        {
            //std::cout << "remove: " << node->Label << std::endl;

            if (node == first && node == last)
            {
                node = NULL;
            }
            else if (node == first)
            {
                first = node->Next;
                first->Prev = NULL;
            }
            else if (node == last)
            {
                last = node->Prev;
                last->Next = NULL;
            }
            else
            {
                before->Next = after;
                after->Prev  = before;
            }

            node = NULL;
            free(node);
        }

        node = after;
    }
}

bool EQ_LootItemByName(const char* name)
{
    DWORD lootWindow = EQ_ReadMemory<DWORD>(EQ_POINTER_CLootWnd);
    if (lootWindow == NULL)
    {
        return false;
    }

    DWORD lootWindowIsVisible = EQ_ReadMemory<BYTE>(lootWindow + 0x124);
    if (lootWindowIsVisible == 0)
    {
        return false;
    }

    bool result = false;

    for (size_t i = 0; i < EQ_NUM_LOOT_WINDOW_SLOTS; i++)
    {
        DWORD itemInfo = EQ_ReadMemory<DWORD>(lootWindow + (0x228 + (i * 4)));
        if (itemInfo == NULL)
        {
            continue;
        }

        PCHAR itemName = EQ_ReadMemory<PCHAR>(itemInfo + 0xB8);
        if (itemName == NULL)
        {
            continue;
        }

        if (strlen(itemName) == 0)
        {
            continue;
        }

        if (strstr(itemName, name) != NULL)
        {
            EQ_CLootWnd->RequestLootSlot(i, true);

            result = true;
        }
    }

    return result;
}

DWORD EQ_GetFontHeight(int fontSize)
{
    // TODO

    if (fontSize == 2)
    {
        return 13;
    }

    return 13;
}

FLOAT EQ_GetAverageFps()
{
    // average frames per second

    DWORD baseAddress = EQ_ReadMemory<DWORD>(EQ_POINTER_GRAPHICS_DLL_BASE_ADDRESS);
    if (baseAddress == NULL)
    {
        return 0.0f;
    }

    return EQ_ReadMemory<FLOAT>(baseAddress + 0x173CB0);
}

bool EQ_HasTimePassed(DWORD& timer, DWORD& delay)
{
    DWORD currentTime = EQ_GetTimer();

    if ((currentTime - timer) > delay)
    {
        timer = currentTime;
        return true;
    }

    return false;
}

void EQ_SetViewActorBySpawn(DWORD spawnInfo)
{
    if (spawnInfo == NULL)
    {
        return;
    }

    DWORD actorInstance = EQ_ReadMemory<DWORD>(spawnInfo + 0xF84);
    if (actorInstance == NULL)
    {
        return;
    }

    EQ_CDisplay->SetViewActor(actorInstance);
}

void EQ_ResetViewActor()
{
    DWORD playerSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_PLAYER_SPAWN_INFO);
    if (playerSpawn == NULL)
    {
        return;
    }

    EQ_SetViewActorBySpawn(playerSpawn);
}

std::string EQ_GetSpawnMapLocationString(DWORD spawnInfo)
{
    char spawnName[0x40] = {0};
    memcpy(spawnName, (LPVOID)(spawnInfo + 0xE4), sizeof(spawnName));

    EQ_StringReplaceSpacesWithUnderscores(spawnName);

    FLOAT spawnY = EQ_ReadMemory<FLOAT>(spawnInfo + 0x64);
    FLOAT spawnX = EQ_ReadMemory<FLOAT>(spawnInfo + 0x68);
    FLOAT spawnZ = EQ_ReadMemory<FLOAT>(spawnInfo + 0x6C);

    // -X, -Y, Z, red, green, blue, size, name
    std::stringstream ss;
    ss.precision(5);
    ss << "P " <<  -spawnX << ", " << -spawnY << ", " << spawnZ << ", 192, 0, 0, 2, " << spawnName;

    return ss.str();
}

void EQ_CopyTargetMapLocationToClipboard()
{
    DWORD targetSpawn = EQ_ReadMemory<DWORD>(EQ_POINTER_TARGET_SPAWN_INFO);
    if (targetSpawn == NULL)
    {
        return;
    }

    std::string mapLocation = EQ_GetSpawnMapLocationString(targetSpawn);

    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, mapLocation.size());
    memcpy(GlobalLock(mem), mapLocation.c_str(), mapLocation.size());
    GlobalUnlock(mem);

    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, mem);
    CloseClipboard();
}

DWORD EQ_GetMapViewMap()
{
    DWORD mapViewWnd = EQ_ReadMemory<DWORD>(EQ_POINTER_CMapViewWnd);
    if (mapViewWnd == NULL)
    {
        return NULL;
    }

    return mapViewWnd + 0x28C;
}

void EQ_UpdateMap()
{
    DWORD mapViewWnd = EQ_ReadMemory<DWORD>(EQ_POINTER_CMapViewWnd);
    if (mapViewWnd == NULL)
    {
        return;
    }

    // force map to update
    EQ_WriteMemory<BYTE>(mapViewWnd + 0x312, 0x01);
}

bool EQ_IsWindowVisible(DWORD windowPointer)
{
    DWORD window = EQ_ReadMemory<DWORD>(windowPointer);
    if (window == NULL)
    {
        return false;
    }

    DWORD isVisible = EQ_ReadMemory<BYTE>(window + 0x124);
    if (isVisible == 0)
    {
        return false;
    }

    return true;
}

std::string EQ_GetExecuteCmdName(unsigned int command)
{
    if (command == 0)
    {
        return "Unknown Command";
    }

    DWORD commandNameAddress = EQ_ReadMemory<DWORD>(EQ_EXECUTECMD_LIST + (command * 4));
    if (commandNameAddress == NULL)
    {
        return "Unknown Command";
    }

    char commandName[0x40] = {0};
    memcpy(commandName, (LPVOID)(commandNameAddress), sizeof(commandName));

    if (commandName == NULL)
    {
        return "Unknown Command";
    }

    return commandName;
}

#endif // EQSOD_FUNCTIONS_H

