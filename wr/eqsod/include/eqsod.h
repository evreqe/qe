#ifndef EQSOD_H
#define EQSOD_H

#include <string>
#include <unordered_map>

const char* EQ_STRING_WINDOW_TITLE      = "EverQuest";
const char* EQ_STRING_PROCESS_NAME      = "eqgame.exe";
const char* EQ_STRING_GRAPHICS_DLL_NAME = "EQGraphicsDX9.dll";

const float EQ_PI = 3.14159265358979f;

const float EQ_FIELD_OF_VIEW_DEFAULT = 45.0f;

const float EQ_DRAW_DISTANCE_DEFAULT = 400.0f;
const float EQ_DRAW_DISTANCE_MINIMUM = 100.0f;

#define EQ_BASE_ADDRESS 0x00400000

#define EQ_WINDOW_HWND   0x00AD6974 // DWORD
#define EQ_WINDOW_WIDTH  0x00AD6950 // DWORD
#define EQ_WINDOW_HEIGHT 0x00AD6954 // DWORD

#define EQ_EXIT_STATUS 0x00A60A03 // BYTE, 0 = not exiting, 1 or 2 = exiting

#define EQ_RESET_DEVICE_VALUE 0x00AD6DB4

#define EQ_BOOL_AUTO_ATTACK    0x00AC1196 // BYTE
#define EQ_BOOL_AUTO_FIRE      0x00AC1197 // BYTE
#define EQ_BOOL_NET_STATUS     0x00A603E1 // BYTE
#define EQ_BOOL_SAFELOCK       0x00A96858 // BYTE

#define EQ_BOOL_KEYBOARD_SHIFT           0x00A609B0 // BYTE
#define EQ_BOOL_KEYBOARD_CONTROL         0x00A609B1 // BYTE
#define EQ_BOOL_KEYBOARD_ALT             0x00A609B2 // BYTE
#define EQ_BOOL_KEYBOARD_LEFT_SHIFT      0x00A609B3 // BYTE
#define EQ_BOOL_KEYBOARD_LEFT_CONTROL    0x00A609B4 // BYTE
#define EQ_BOOL_KEYBOARD_LEFT_ALT        0x00A609B5 // BYTE
#define EQ_BOOL_KEYBOARD_RIGHT_SHIFT     0x00A609B6 // BYTE
#define EQ_BOOL_KEYBOARD_RIGHT_CONTROL   0x00A609B7 // BYTE
#define EQ_BOOL_KEYBOARD_RIGHT_ALT       0x00A609B8 // BYTE

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
#define EQ_POINTER_CLootWnd                   0x00990C50
#define EQ_POINTER_CMapViewWnd                0x00990C0C
#define EQ_POINTER_CSidlManager               0x00B10A40
#define EQ_POINTER_CTextEntryWnd              0x00990C90
#define EQ_POINTER_CTextOverlay               0x00877500
#define EQ_POINTER_CXWndManager               0x00B10A38
#define EQ_POINTER_EQ_Character               0x00A425E4 // pinstCharData

#define EQ_POINTER_SPELL_MANAGER              0x00AD3A90 // pinstSpellManager
#define EQ_POINTER_SWITCH_MANAGER             0x00A40704 // pinstSwitchManager (doors)

#define EQ_POINTER_CHAR_INFO                  0x00A425E4 // pinstCharData

#define EQ_POINTER_FIRST_SPAWN_INFO           0x00AD3A0C // pinstSpawnManager
#define EQ_POINTER_PLAYER_SPAWN_INFO          0x00A4260C // pinstCharSpawn
#define EQ_POINTER_TARGET_SPAWN_INFO          0x00A42610 // pinstTarget

#define EQ_POINTER_FIRST_GROUND_SPAWN_INFO    0x00A425C8 // pinstEQItemList

#define EQ_POINTER_GROUP_INFO                 0x00A40ABE // pinstGroup

#define EQ_GUILDS 0x00A42640

#define EQ_ZONEINFO_CHARACTER_NAME    0x00A6057C // string 64  [0x40]
#define EQ_ZONEINFO_SHORT_NAME        0x00A605BC // string 32  [0x20]
#define EQ_ZONEINFO_LONG_NAME         0x00A6063C // string 128 [0x80]
#define EQ_ZONEINFO_FOG_MIN_CLIP      0x00A60760 // FLOAT
#define EQ_ZONEINFO_FOG_MAX_CLIP      0x00A60770 // FLOAT
#define EQ_ZONEINFO_GRAVITY           0x00A60780 // FLOAT
#define EQ_ZONEINFO_MIN_CLIP          0x00A607E0 // FLOAT
#define EQ_ZONEINFO_MAX_CLIP          0x00A607E4 // FLOAT

#define EQ_FUNCTION_Exit 0x004B06B0 // called by the "/exit" slash command

#define EQ_FUNCTION_ResetDevice  0x00561790
#define EQ_FUNCTION_WindowResize 0x00561840

#define EQ_FUNCTION_CastRay                     0x004C3E40 // __CastRay
#define EQ_FUNCTION_CXWnd_DrawColoredRect       0x006DBB30
#define EQ_FUNCTION_DrawNetStatus               0x00512280
#define EQ_FUNCTION_ExecuteCmd                  0x004A5580
#define EQ_FUNCTION_get_melee_range             0x004AAA20 // __get_melee_range
#define EQ_FUNCTION_get_bearing                 0x004B7BF0

#define EQ_FUNCTION_EQ_Guilds__GetGuildNameById    0x00421930 // GetPlayerGuildName

#define EQ_FUNCTION_CDisplay__CreatePlayerActor    0x0046E0B0
#define EQ_FUNCTION_CDisplay__DeleteActor          0x0046F2F0
#define EQ_FUNCTION_CDisplay__WriteTextHD2         0x0046D880

#define EQ_FUNCTION_CEverQuest__dsp_chat                   0x004DCD60
#define EQ_FUNCTION_CEverQuest__dsp_chat__2                0x004DCF30
#define EQ_FUNCTION_CEverQuest__EnterZone                  0x004F2420
#define EQ_FUNCTION_CEverQuest__GetBodyTypeDesc            0x004D4660
#define EQ_FUNCTION_CEverQuest__GetClassDesc               0x004D3DC0
#define EQ_FUNCTION_CEverQuest__GetClassThreeLetterCode    0x004D43C0
#define EQ_FUNCTION_CEverQuest__GetDeityDesc               0x004D4CD0
#define EQ_FUNCTION_CEverQuest__GetRaceDesc                0x004D4CA0
#define EQ_FUNCTION_CEverQuest__InterpretCmd               0x004DD7C0
#define EQ_FUNCTION_CEverQuest__LMouseUp                   0x004F3200
#define EQ_FUNCTION_CEverQuest__RMouseUp                   0x004F2A20
#define EQ_FUNCTION_CEverQuest__MoveToZone                 0x004FABE0
#define EQ_FUNCTION_CEverQuest__SetGameState               0x004D7890

#define EQ_FUNCTION_CHotButtonWnd__DoHotButton    0x005C5E80

#define EQ_FUNCTION_CLootWnd__Deactivate         0x005EAA50
#define EQ_FUNCTION_CLootWnd__RequestLootSlot    0x005EB430

#define EQ_FUNCTION_CMapViewWnd__DrawMap   0x005F7320

#define EQ_FUNCTION_CTextEntryWnd__Activate    0x00649060

#define EQ_FUNCTION_CTextOverlay__DisplayText     0x0041DB10

#define EQ_FUNCTION_EQ_Character__eqspa_movement_rate    0x0042D970
#define EQ_FUNCTION_EQ_Character__UseSkill               0x00441FC0

#define EQ_FUNCTION_EQPlayer__ChangeHeight      0x00530900
#define EQ_FUNCTION_EQPlayer__ChangePosition    0x0052E120
#define EQ_FUNCTION_EQPlayer__FacePlayer        0x0052C170

#define EQ_FUNCTION_EQSwitch__ChangeState       0x004D0430

#define EQ_CAMERA_VIEW 0x00990CF0 // DWORD

#define EQ_CAMERA_VIEW_FIRST_PERSON          0
#define EQ_CAMERA_VIEW_THIRD_PERSON_OVERHEAD 1
#define EQ_CAMERA_VIEW_THIRD_PERSON2         2
#define EQ_CAMERA_VIEW_THIRD_PERSON3         3
#define EQ_CAMERA_VIEW_THIRD_PERSON4         4
#define EQ_CAMERA_VIEW_THIRD_PERSON_CHASE    5

#define EQ_CAMERA_PITCH_DEFAULT -8.5

#define EQ_DRAW_DISTANCE_MAX 0x00A607E4 // default is 1000.0f

#define EQ_SPAWN_TYPE_PLAYER        0
#define EQ_SPAWN_TYPE_NPC           1
#define EQ_SPAWN_TYPE_NPC_CORPSE    2
#define EQ_SPAWN_TYPE_PLAYER_CORPSE 3

#define EQ_DOOR_STATE_OPEN  2
#define EQ_DOOR_STATE_CLOSE 3

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

#define EQ_TOOLTIP_COLOR 0xC8000040 // ARGB color

// EQPlayer::ChangePosition(BYTE standingState)
#define EQ_STANDING_STATE_STANDING 0x64
#define EQ_STANDING_STATE_FROZEN   0x66 // stunned / mesmerized / feared ; You lose control of yourself!
#define EQ_STANDING_STATE_LOOTING  0x69 // looting or binding wounds
#define EQ_STANDING_STATE_SITTING  0x6E
#define EQ_STANDING_STATE_DUCKING  0x6F // crouching
#define EQ_STANDING_STATE_FEIGNED  0x73 // feign death
#define EQ_STANDING_STATE_DEAD     0x78

// EQ_Character::eqspa_movement_rate
#define EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN1        0.08f
#define EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN2        0.14f
#define EQ_MOVEMENT_SPEED_MODIFIER_AA_RUN3        0.21f
#define EQ_MOVEMENT_SPEED_MODIFIER_SPIRIT_OF_WOLF 0.30f

#define EQ_ANIMATION_WALKING          17
#define EQ_ANIMATION_RUNNING          18
#define EQ_ANIMATION_JUMPING          19
#define EQ_ANIMATION_DUCKING          24
#define EQ_ANIMATION_IDLE_ANIMATED    26
#define EQ_ANIMATION_IDLE             32
#define EQ_ANIMATION_SITTING          33
#define EQ_ANIMATION_TURNING          34

#define EQ_ITEM_TYPE_CONTAINER          0x7900
#define EQ_ITEM_TYPE_CONTAINER_PLAIN    0x7953

#define EQ_NUM_INVENTORY_SLOTS      31 // 23 equipment slots, 8 bag slots
#define EQ_NUM_BANK_SLOTS           26 // 24 bag slots, 2 shared bag slots
#define EQ_NUM_CONTAINER_SLOTS      10 // number of items that can fit in a bag, backpack or box
#define EQ_NUM_LOOT_WINDOW_SLOTS    32
#define EQ_NUM_GUILDS               1500

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

const std::vector<std::string> EQ_EQUIPMENT_SLOT_NAMES =
{
    "Charm",
    "Left Ear",
    "Head",
    "Face",
    "Right Ear",
    "Neck",
    "Shoulders",
    "Arms",
    "Back",
    "Left Wrist",
    "Right Wrist",
    "Range",
    "Hands",
    "Primary",
    "Secondary",
    "Left Finger",
    "Right Finger",
    "Chest",
    "Legs",
    "Feet",
    "Waist",
    "Power Source",
    "Ammo",
    "Bag #1",
    "Bag #2",
    "Bag #3",
    "Bag #4",
    "Bag #5",
    "Bag #6",
    "Bag #7",
    "Bag #8",
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

typedef struct _EQXYZ
{
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
} EQXYZ, *PEQXYZ;

typedef struct _EQYXZ
{
    FLOAT Y;
    FLOAT X;
    FLOAT Z;
} EQYXZ, *PEQYXZ;

typedef struct _EQARGBCOLOR
{
    union
    {
        struct
        {
            BYTE B;
            BYTE G;
            BYTE R;
            BYTE A;
        };
        DWORD Color;
    };
} EQARGBCOLOR, *PEQARGBCOLOR;

typedef struct _EQRGBCOLOR
{
    BYTE B;
    BYTE G;
    BYTE R;
} EQRGBCOLOR, *PEQRGBCOLOR;

typedef struct _EQMAPLABEL
{
/*0x00*/    struct _EQMAPLABEL* Next = NULL;
/*0x04*/    struct _EQMAPLABEL* Prev;
/*0x08*/    EQXYZ Location;
/*0x14*/    EQARGBCOLOR Color;
/*0x18*/    DWORD Size = 2; // 1-3
/*0x1C*/    PCHAR Label; // text
/*0x20*/    DWORD Layer = 0; // 0-3
/*0x24*/    DWORD Width = 1;
/*0x28*/    DWORD Height = 12;
/*0x2C*/    DWORD Unknown0x2C; // BYTE X;
/*0x30*/    DWORD Unknown0x30; // BYTE Y;
            DWORD Data = 255; // custom data for identifying labels by value
} EQMAPLABEL, *PEQMAPLABEL;

typedef struct _EQMAPLINE
{
    struct _EQMAPLINE* Next;
    struct _EQMAPLINE* Prev;
    EQXYZ Begin;
    EQXYZ End;
    EQARGBCOLOR Color;
    DWORD Layer; // 0-3
} EQMAPLINE, *PEQMAPLINE;

#endif // EQSOD_H
