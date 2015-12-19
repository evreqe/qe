#ifndef EQSOD_H
#define EQSOD_H

#include <string>
#include <unordered_map>

const char* EQ_STRING_WINDOW_TITLE      = "EverQuest";
const char* EQ_STRING_PROCESS_NAME      = "eqgame.exe";
const char* EQ_STRING_GRAPHICS_DLL_NAME = "EQGraphicsDX9.dll";

const float EQ_PI = 3.14159265358979f;

#define EQ_BASE_ADDRESS 0x00400000

#define EQ_WINDOW_WIDTH  0x00AD6950
#define EQ_WINDOW_HEIGHT 0x00AD6954

#define EQ_BOOL_AUTO_ATTACK        0x00AC1196
#define EQ_BOOL_AUTO_FIRE          0x00AC1197
#define EQ_BOOL_DRAW_NET_STATUS    0x00A603E1

#define EQ_POINTER_GRAPHICS_DLL_BASE_ADDRESS 0x00B112A8 // EQGraphicsDX9.DLL

#define EQ_GRAPHICS_DLL_OFFSET_DrawLine  0x7A850
#define EQ_GRAPHICS_DLL_OFFSET_DrawQuad  0x7A560

#define EQ_GRAPHICS_DLL_ORDINAL_DrawLine 0x84
#define EQ_GRAPHICS_DLL_ORDINAL_DrawQuad 0x9C

#define EQ_POINTER_EQGraphicsDLL              0x00B112BC

#define EQ_POINTER_CDisplay                   0x00A42628
#define EQ_POINTER_CEverQuest                 0x00AD6DB8
#define EQ_POINTER_CHotButtonWnd              0x00B01C0C
#define EQ_POINTER_CHotButtonWnd1             0x00B01C0C
#define EQ_POINTER_CHotButtonWnd2             0x00B01C10
#define EQ_POINTER_CHotButtonWnd3             0x00B01C14
#define EQ_POINTER_CHotButtonWnd4             0x00B01C18
#define EQ_POINTER_CSidlManager               0x00B10A40
#define EQ_POINTER_CTextEntryWnd              0x00990C90
#define EQ_POINTER_CTextOverlay               0x00877500
#define EQ_POINTER_CXWndManager               0x00B10A38
#define EQ_POINTER_EQ_Character               0x00A425E4 // pinstCharData

#define EQ_POINTER_SPELL_MANAGER              0x00AD3A90 // pinstSpellManager

#define EQ_POINTER_CHAR_INFO                  0x00A425E4 // pinstCharData

#define EQ_POINTER_FIRST_SPAWN_INFO           0x00AD3A0C // pinstSpawnManager
#define EQ_POINTER_PLAYER_SPAWN_INFO          0x00A4260C // pinstCharSpawn
#define EQ_POINTER_TARGET_SPAWN_INFO          0x00A42610 // pinstTarget

#define EQ_POINTER_FIRST_GROUND_SPAWN_INFO    0x00A425C8 // pinstEQItemList

#define EQ_POINTER_GROUP_INFO                 0x00A40ABE // pinstGroup

#define EQ_ZONEINFO_SHORT_NAME    0x00A605BC

#define EQ_FUNCTION_CastRay                     0x004C3E40 // __CastRay
#define EQ_FUNCTION_CXWnd_DrawColoredRect       0x006DBB30
#define EQ_FUNCTION_DrawNetStatus               0x00512280
#define EQ_FUNCTION_ExecuteCmd                  0x004A5580
#define EQ_FUNCTION_get_melee_range             0x004AAA20 // __get_melee_range

#define EQ_FUNCTION_CDisplay__WriteTextHD2        0x0046D880

#define EQ_FUNCTION_CHotButtonWnd__DoHotButton    0x005C5E80

#define EQ_FUNCTION_CEverQuest__dsp_chat          0x004DCD60
#define EQ_FUNCTION_CEverQuest__dsp_chat__2       0x004DCF30
#define EQ_FUNCTION_CEverQuest__EnterZone         0x004F2420
#define EQ_FUNCTION_CEverQuest__GetBodyTypeDesc   0x004D4660
#define EQ_FUNCTION_CEverQuest__InterpretCmd      0x004DD7C0
#define EQ_FUNCTION_CEverQuest__LMouseUp          0x004F3200
#define EQ_FUNCTION_CEverQuest__RMouseUp          0x004F2A20

#define EQ_FUNCTION_EQ_Character__eqspa_movement_rate    0x0042D970
#define EQ_FUNCTION_EQ_Character__UseSkill               0x00441FC0

#define EQ_FUNCTION_CTextEntryWnd__Activate    0x00649060

#define EQ_FUNCTION_CTextOverlay__DisplayText     0x0041DB10

#define EQ_SPAWN_TYPE_PLAYER        0
#define EQ_SPAWN_TYPE_NPC           1
#define EQ_SPAWN_TYPE_NPC_CORPSE    2
#define EQ_SPAWN_TYPE_PLAYER_CORPSE 3

#define EQ_GAME_STATE_CHARACTER_SELECT 1
#define EQ_GAME_STATE_ZONING           3
#define EQ_GAME_STATE_ZONING2          4
#define EQ_GAME_STATE_IN_GAME          5
#define EQ_GAME_STATE_LOADING_SCREEN   6
#define EQ_GAME_STATE_ZONING3          7

#define EQ_TEXT_COLOR_BLACK       0x00
#define EQ_TEXT_COLOR_GRAY        0x01
#define EQ_TEXT_COLOR_GREEN       0x02
#define EQ_TEXT_COLOR_GRAY2       0x03
#define EQ_TEXT_COLOR_BLUE        0x04
#define EQ_TEXT_COLOR_PINK        0x05
#define EQ_TEXT_COLOR_GRAY3       0x06
#define EQ_TEXT_COLOR_WHITE       0x07
#define EQ_TEXT_COLOR_GRAY4       0x08
#define EQ_TEXT_COLOR_GRAY5       0x09
#define EQ_TEXT_COLOR_WHITE2      0x0A
#define EQ_TEXT_COLOR_GRAY6       0x0B
#define EQ_TEXT_COLOR_LIGHT_GRAY  0x0C
#define EQ_TEXT_COLOR_RED         0x0D
#define EQ_TEXT_COLOR_LIGHT_GREEN 0x0E
#define EQ_TEXT_COLOR_YELLOW      0x0F
#define EQ_TEXT_COLOR_LIGHT_BLUE  0x10
#define EQ_TEXT_COLOR_GRAY7       0x11
#define EQ_TEXT_COLOR_CYAN        0x12
#define EQ_TEXT_COLOR_GRAY8       0x13
#define EQ_TEXT_COLOR_BLACK2      0x14

// EQ_Character::eqspa_movement_rate
#define EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN1        0.08f
#define EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN2        0.14f
#define EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN3        0.21f
#define EQ_MOVEMENT_SPEED_MODIFIER_SPIRIT_OF_WOLF 0.30f

const std::unordered_map<std::string, std::string> EQ_KEYVALUE_ACTOR_DEFINITIONS
{
    {"IT27_ACTORDEF",    "Book"},
    {"IT28_ACTORDEF",    "Book"},
    {"IT63_ACTORDEF",    "Small Bag"},
    {"IT64_ACTORDEF",    "Large Bag"},
    {"IT66_ACTORDEF",    "Forge"},
    {"IT69_ACTORDEF",    "Oven"},
    {"IT70_ACTORDEF",    "Brew Barrel"},
    {"IT73_ACTORDEF",    "Kiln"},
    {"IT74_ACTORDEF",    "Pottery Wheel"},
    {"IT128_ACTORDEF",   "Sewing Kit"},
    {"IT10645_ACTORDEF", "Book"},
};

typedef struct _EQLINE
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z = 0.0f;
    //FLOAT X1;
    //FLOAT Y1;
    //FLOAT Z1 = 0.0f;
    //FLOAT X2;
    //FLOAT Y2;
    //FLOAT Z2 = 0.0f;
} EQLINE, *PEQLINE;

typedef struct _EQRECT
{
    FLOAT X1;
    FLOAT Y1;
    FLOAT Z1 = 1.0f;
    FLOAT X2;
    FLOAT Y2;
    FLOAT Z2 = 1.0f;
    FLOAT X3;
    FLOAT Y3;
    FLOAT Z3 = 1.0f;
    FLOAT X4;
    FLOAT Y4;
    FLOAT Z4 = 1.0f;
} EQRECT, *PEQRECT;

typedef struct _EQCXRECT
{
    int X1;
    int Y1;
    int X2;
    int Y2;
} EQCXRECT, *PEQCXRECT;

#endif // EQSOD_H
