Global eq_process_handle := 0
Global eq_process_id     := 0

Global EQ_CLIENT := "eqgame.exe"

Global EQ_TITLE := "EverQuest"

Global EQ_BASE_ADDRESS     := 0x00400000
Global EQ_GFX_DLL_ADDRESS  := 0x10000000 ; EQGfx_Dx8.dll
Global EQ_D3D8_DLL_ADDRESS := 0x72DD0000

Global EQ_NAME_SIZE      := 0x40
Global EQ_LAST_NAME_SIZE := 0x20

Global EQ_ZONE_LONG_NAME_SIZE  := 0x80
Global EQ_ZONE_SHORT_NAME_SIZE := 0x20

Global EQ_ZONE_INFO_STRUCTURE := 0x006C91DC ; STRUCT
Global EQ_OFFSET_ZONE_INFO_PLAYER_NAME  := 0x000 ; STRING [0x40]
Global EQ_OFFSET_ZONE_INFO_SHORT_NAME   := 0x040 ; STRING [0x20]
Global EQ_OFFSET_ZONE_INFO_LONG_NAME    := 0x060 ; STRING [0x80]
Global EQ_OFFSET_ZONE_INFO_TYPE         := 0x176 ; BYTE
Global EQ_OFFSET_ZONE_INFO_GRAVITY      := 0x1A4 ; FLOAT
Global EQ_OFFSET_ZONE_INFO_SKY_TYPE     := 0x1DA ; BYTE
Global EQ_OFFSET_ZONE_INFO_EXP_MODIFIER := 0x1E8 ; FLOAT
Global EQ_OFFSET_ZONE_INFO_CLIP_MIN     := 0x200 ; FLOAT
Global EQ_OFFSET_ZONE_INFO_CLIP_MAX     := 0x204 ; FLOAT

Global EQ_ZONE_GRAVITY_DEFAULT := 0.400000006

Global EQ_ZONE_SHORT_NAME_LIVE := 0x00787648 ; STRING [0x20 == 32] ; real zone name, not custom zone name

Global EQ_CHAR_INFO_POINTER := 0x0072B7A8 ; DWORD POINTER
Global EQ_OFFSET_CHAR_INFO_NAME                 := 0x0002 ; STRING [0x40 == 64]
Global EQ_OFFSET_CHAR_INFO_LAST_NAME            := 0x0042 ; STRING [0x20 == 32]
Global EQ_OFFSET_CHAR_INFO_GENDER               := 0x0088 ; BYTE
Global EQ_OFFSET_CHAR_INFO_RACE                 := 0x008C ; BYTE ; EQ_RACE_...
Global EQ_OFFSET_CHAR_INFO_CLASS                := 0x0090 ; BYTE ; EQ_CLASS_...
Global EQ_OFFSET_CHAR_INFO_MANA                 := 0x00A4 ; DWORD
Global EQ_OFFSET_CHAR_INFO_STUNNED_STATE        := 0x00B0 ; BYTE ; EQ_STUNNED_STATE...
Global EQ_OFFSET_CHAR_INFO_BUFFS_STRUCTURE      := 0x0120 ; STRUCT [0x0F == 15] ; 15 buffs total
Global EQ_OFFSET_CHAR_INFO_Y                    := 0x0A54 ; FLOAT
Global EQ_OFFSET_CHAR_INFO_X                    := 0x0A58 ; FLOAT
Global EQ_OFFSET_CHAR_INFO_Z                    := 0x0A5C ; FLOAT
Global EQ_OFFSET_CHAR_INFO_HEADING              := 0x0A60 ; FLOAT ; facing direction, rotation
Global EQ_OFFSET_CHAR_INFO_STANDING_STATE       := 0x0A64 ; BYTE ; EQ_STANDING_STATE_...
Global EQ_OFFSET_CHAR_INFO_PLATINUM             := 0x0A68 ; DWORD
Global EQ_OFFSET_CHAR_INFO_GOLD                 := 0x0A6C ; DWORD
Global EQ_OFFSET_CHAR_INFO_SILVER               := 0x0A70 ; DWORD
Global EQ_OFFSET_CHAR_INFO_COPPER               := 0x0A74 ; DWORD
Global EQ_OFFSET_CHAR_INFO_BANK_PLATINUM        := 0x0A78 ; DWORD
Global EQ_OFFSET_CHAR_INFO_BANK_GOLD            := 0x0A7C ; DWORD
Global EQ_OFFSET_CHAR_INFO_BANK_SILVER          := 0x0A80 ; DWORD
Global EQ_OFFSET_CHAR_INFO_BANK_COPPER          := 0x0A84 ; DWORD
Global EQ_OFFSET_CHAR_INFO_BANK_SHARED_PLATINUM := 0x0A98 ; DWORD
Global EQ_OFFSET_CHAR_INFO_SPAWN_INFO_POINTER   := 0x0D80 ; DWORD POINTER

Global EQ_SPAWN_INFO_BEGIN_POINTER := 0x0072B768 ; DWORD POINTER
Global EQ_SPAWN_INFO_END_POINTER   := 0x0072B78C ; DWORD POINTER

Global EQ_PLAYER_SPAWN_INFO_POINTER   := 0x0072B7A0 ; DWORD POINTER
Global EQ_TARGET_SPAWN_INFO_POINTER   := 0x0072B7AC ; DWORD POINTER
Global EQ_MERCHANT_SPAWN_INFO_POINTER := 0x0072B7B8 ; DWORD POINTER

Global EQ_SPAWN_INFO_NULL := 0x00000000

Global EQ_OFFSET_SPAWN_INFO_NAME                    := 0x001 ; STRING [0x40 == 64]
Global EQ_OFFSET_SPAWN_INFO_LAST_NAME               := 0x041 ; STRING [0x20 == 32] ; title for NPCs
Global EQ_OFFSET_SPAWN_INFO_Y                       := 0x068 ; FLOAT
Global EQ_OFFSET_SPAWN_INFO_X                       := 0x06C ; FLOAT
Global EQ_OFFSET_SPAWN_INFO_Z                       := 0x070 ; FLOAT
Global EQ_OFFSET_SPAWN_INFO_SPEED_MOVEMENT          := 0x080 ; FLOAT
Global EQ_OFFSET_SPAWN_INFO_HEADING                 := 0x084 ; FLOAT ; facing direction, rotation
Global EQ_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER      := 0x0DC ; DWORD POINTER
Global EQ_OFFSET_SPAWN_INFO_IS_LINKDEAD             := 0x0E6 ; BYTE ; BOOL
Global EQ_OFFSET_SPAWN_INFO_IS_LFG                  := 0x0E8 ; BYTE ; BOOL ; is looking for group
Global EQ_OFFSET_SPAWN_INFO_WALK_THROUGH_WALLS      := 0x0EA ; BYTE ; walk through doors and thin walls or climb up and over objects
Global EQ_OFFSET_SPAWN_INFO_NEXT_SPAWN_INFO_POINTER := 0x13C ; DWORD POINTER
Global EQ_OFFSET_SPAWN_INFO_CHAR_INFO_POINTER       := 0x140 ; DWORD POINTER
Global EQ_OFFSET_SPAWN_INFO_PREV_SPAWN_INFO_POINTER := 0x148 ; DWORD POINTER
Global EQ_OFFSET_SPAWN_INFO_AVATAR_HEIGHT           := 0x164 ; FLOAT ; height from ground when standing
Global EQ_OFFSET_SPAWN_INFO_TYPE                    := 0x16C ; BYTE ; EQ_SPAWN_INFO_TYPE_...
Global EQ_OFFSET_SPAWN_INFO_LEVEL                   := 0x174 ; BYTE
Global EQ_OFFSET_SPAWN_INFO_GENDER                  := 0x176 ; BYTE
Global EQ_OFFSET_SPAWN_INFO_PVP_FLAG                := 0x177 ; BYTE
Global EQ_OFFSET_SPAWN_INFO_HIDE_MODE               := 0x178 ; BYTE
Global EQ_OFFSET_SPAWN_INFO_STANDING_STATE          := 0x179 ; BYTE ; EQ_STANDING_STATE_...
Global EQ_OFFSET_SPAWN_INFO_CLASS                   := 0x17A ; BYTE ; EQ_CLASS_...
Global EQ_OFFSET_SPAWN_INFO_IS_GM                   := 0x17E ; BYTE ; is gamemaster
Global EQ_OFFSET_SPAWN_INFO_SPAWN_ID                := 0x180 ; DWORD
Global EQ_OFFSET_SPAWN_INFO_MASTER_ID               := 0x184 ; DWORD ; spawn id of the owner of this pet
Global EQ_OFFSET_SPAWN_INFO_RACE                    := 0x188 ; BYTE ; EQ_RACE_...
Global EQ_OFFSET_SPAWN_INFO_IS_AFK                  := 0x194 ; DWORD
Global EQ_OFFSET_SPAWN_INFO_BODY_TYPE               := 0x198 ; DWORD ; EQ_BODY_TYPE_...
Global EQ_OFFSET_SPAWN_INFO_HP_CURRENT              := 0x19C ; DWORD
Global EQ_OFFSET_SPAWN_INFO_GUILD_STATUS            := 0x1A4 ; DWORD
Global EQ_OFFSET_SPAWN_INFO_DEITY                   := 0x1A8 ; DWORD
Global EQ_OFFSET_SPAWN_INFO_HP_MAX                  := 0x1AC ; DWORD
Global EQ_OFFSET_SPAWN_INFO_GUILD_ID                := 0x1B0 ; DWORD
Global EQ_OFFSET_SPAWN_INFO_LEVITATING_STATE        := 0x1B4 ; BYTE

Global EQ_SPAWN_INFO_TYPE_PLAYER        := 0
Global EQ_SPAWN_INFO_TYPE_NPC           := 1
Global EQ_SPAWN_INFO_TYPE_NPC_CORPSE    := 2
Global EQ_SPAWN_INFO_TYPE_PLAYER_CORPSE := 3

Global EQ_OFFSET_ACTOR_INFO_CAMERA_POINTER              := 0x0000 ; DWORD POINTER
Global EQ_OFFSET_ACTOR_INFO_Z                           := 0x0048 ; FLOAT
Global EQ_OFFSET_ACTOR_INFO_LEVITATION_MODIFIER         := 0x007C ; BYTE ; up/down movement of levitation
Global EQ_OFFSET_ACTOR_INFO_DRUNK_MOVEMENT_MODIFIER     := 0x0080 ; BYTE ; walk sideways and stumble when drunk
Global EQ_OFFSET_ACTOR_INFO_DRUNK_MOVEMENT_MODIFIER_ADD := 0x0088 ; BYTE
Global EQ_OFFSET_ACTOR_INFO_IS_AFFECTED_BY_GRAVITY      := 0x0094 ; BYTE ; BOOL
Global EQ_OFFSET_ACTOR_INFO_IS_SWIMMING_UNDERWATER      := 0x00A4 ; BYTE ; used for breathing/drowning
Global EQ_OFFSET_ACTOR_INFO_IS_SWIMMING                 := 0x00A5 ; BYTE
Global EQ_OFFSET_ACTOR_INFO_IS_SWIMMING_FEET_WET        := 0x00A6 ; BYTE ; feet are in the water
Global EQ_OFFSET_ACTOR_INFO_IS_SWIMMING_LEFT_WATER      := 0x00A7 ; BYTE ; just left the water, close to the shore
Global EQ_OFFSET_ACTOR_INFO_ANIMATION                   := 0x018C ; DWORD
Global EQ_OFFSET_ACTOR_INFO_IS_BLIND                    := 0x01A8 ; BYTE ; BOOL ; causes black screen
; 0x2660
; 0x2688
; 0x268C
Global EQ_OFFSET_ACTOR_INFO_FOLLOWING_SPAWN_INFO_POINTER := 0x2720 ; DWORD POINTER

Global EQ_BUFFS_STRUCTURE_BUFF_SIZE := 0x10
Global EQ_OFFSET_BUFFS_STRUCTURE_UNKNOWN_0x00     := 0x00
Global EQ_OFFSET_BUFFS_STRUCTURE_CASTER_LEVEL     := 0x01 ; BYTE ; level of player who casted the buff
Global EQ_OFFSET_BUFFS_STRUCTURE_MODIFIER         := 0x02 ; BYTE ; divide by 10 to get Bard song modifier
Global EQ_OFFSET_BUFFS_STRUCTURE_IS_DAMAGE_SHIELD := 0x03 ; BYTE ; damage shield = -1
Global EQ_OFFSET_BUFFS_STRUCTURE_SPELL_ID         := 0x04 ; DWORD
Global EQ_OFFSET_BUFFS_STRUCTURE_DURATION         := 0x08 ; DWORD ; seconds = duration * 6
Global EQ_OFFSET_BUFFS_STRUCTURE_DAMAGE_ABSORB    := 0x0C ; DWORD

Global EQ_GUILD_NAMES_BEGIN := 0x0072FE8C
Global EQ_GUILD_NAME_SIZE := 0x40 ; STRING [0x40 == 32]

Global EQ_GUILD_NAMES_MAX := 512

Global EQ_GUILD_ID_NULL := 4294967295 ; 0xFFFFFFFF

Global EQ_BUFFS_MAX := 15

Global EQ_BUFF_SPELL_ID_NULL := 0xFFFFFFFF

Global EQ_BUFF_TYPE_DETRIMENTAL           := 0
Global EQ_BUFF_TYPE_BENEFICIAL            := 1
Global EQ_BUFF_TYPE_BENEFICIAL_GROUP_ONLY := 2

Global EQ_WINDOW_IS_FOCUSED := 0x0060C360 ; BOOL

Global EQ_WINDOW_X := 0x0079EF50 ; DWORD
Global EQ_WINDOW_Y := 0x0079EF54 ; DWORD

Global EQ_MOUSE_X := 0x00788C78 ; DWORD
Global EQ_MOUSE_Y := 0x00788C7C ; DWORD

Global EQ_SHOW_LATENCY := 0x006C903C ; BOOL

Global EQ_SHOW_FRAMES_PER_SECOND := 0x1091A12C ; BOOL

Global EQ_CAMERA_POINTER := 0x006152C0 ; DWORD POINTER
Global EQ_OFFSET_CAMERA_REGION_NUMBER     := 0x04 ; DWORD
Global EQ_OFFSET_CAMERA_Y                 := 0x08 ; FLOAT
Global EQ_OFFSET_CAMERA_X                 := 0x0C ; FLOAT
Global EQ_OFFSET_CAMERA_Z                 := 0x10 ; FLOAT
Global EQ_OFFSET_CAMERA_HEADING           := 0x14 ; FLOAT
Global EQ_OFFSET_CAMERA_PITCH             := 0x18 ; FLOAT
Global EQ_OFFSET_CAMERA_FIELD_OF_VIEW     := 0x20 ; FLOAT
Global EQ_OFFSET_CAMERA_ASPECT_RATIO      := 0x24 ; FLOAT
Global EQ_OFFSET_CAMERA_WORLD_SCALE       := 0x28 ; FLOAT
Global EQ_OFFSET_CAMERA_DRAW_DISTANCE     := 0x30 ; FLOAT
Global EQ_OFFSET_CAMERA_RESOLUTION_WIDTH  := 0x48 ; DWORD
Global EQ_OFFSET_CAMERA_RESOLUTION_HEIGHT := 0x4C ; DWORD

Global EQ_DRAW_DISTANCE_MULTIPLIER := 0x006CA0C4 ; DWORD

Global EQ_FOG_DRAW_DISTANCE := 0x1091A28C ; FLOAT

Global EQ_FOG_DRAW_DISTANCE_UNDERWATER := 100

; EQGfx_Dx8.t3dSetFog
Global EQ_ASM_SET_FOG_DRAW_DISTANCE := 0x100507FE ; 89 1D 8CA29110 - mov [EQGfx_Dx8.DLL+91A28C],ebx

Global EQ_ASM_SET_FOG_DRAW_DISTANCE_BYTES := "89,1D,8C,A2,91,10"

Global EQ_ASM_SET_FOG_DRAW_DISTANCE_SIZE := 6

; EQGfx_Dx8.t3dSetCameraLocation
Global EQ_ASM_SET_CAMERA_Y       := 0x100334AA ; 89 01    - mov [ecx],eax
Global EQ_ASM_SET_CAMERA_X       := 0x100334AF ; 89 41 04 - mov [ecx+04],eax
Global EQ_ASM_SET_CAMERA_Z       := 0x100334B5 ; 89 51 08 - mov [ecx+08],edx
Global EQ_ASM_SET_CAMERA_HEADING := 0x100334BB ; 89 46 14 - mov [esi+14],eax
Global EQ_ASM_SET_CAMERA_PITCH   := 0x100334C1 ; 89 4E 18 - mov [esi+18],ecx

Global EQ_ASM_SET_CAMERA_Y_BYTES       := "89,01"
Global EQ_ASM_SET_CAMERA_X_BYTES       := "89,41,04"
Global EQ_ASM_SET_CAMERA_Z_BYTES       := "89,51,08"
Global EQ_ASM_SET_CAMERA_HEADING_BYTES := "89,46,14"
Global EQ_ASM_SET_CAMERA_PITCH_BYTES   := "89,4E,18"

Global EQ_ASM_SET_CAMERA_Y_SIZE       := 2
Global EQ_ASM_SET_CAMERA_X_SIZE       := 3
Global EQ_ASM_SET_CAMERA_Z_SIZE       := 3
Global EQ_ASM_SET_CAMERA_HEADING_SIZE := 3
Global EQ_ASM_SET_CAMERA_PITCH_SIZE   := 3

; EQGfx_Dx8.t3dSetCameraLens
Global EQ_ASM_SET_CAMERA_FIELD_OF_VIEW := 0x1003343B ; D9 59 20 - fstp dword ptr [ecx+20]
Global EQ_ASM_SET_CAMERA_DRAW_DISTANCE := 0x10033448 ; 89 41 30 - mov [ecx+30],eax

Global EQ_ASM_SET_CAMERA_FIELD_OF_VIEW_BYTES := "D9,59,20"
Global EQ_ASM_SET_CAMERA_DRAW_DISTANCE_BYTES := "89,41,30"

Global EQ_ASM_SET_CAMERA_FIELD_OF_VIEW_SIZE := 3
Global EQ_ASM_SET_CAMERA_DRAW_DISTANCE_SIZE := 3

Global EQ_CAMERA_PITCH_DEFAULT := -8.5 ; 119.5 + 8.5 = 128 everquest degrees = 90 degrees

Global EQ_CAMERA_PITCH_MIN := -119.5
Global EQ_CAMERA_PITCH_MAX := 119.5

Global EQ_CAMERA_FIELD_OF_VIEW_FIRST_PERSON_DEFAULT := 55
Global EQ_CAMERA_FIELD_OF_VIEW_THIRD_PERSON_DEFAULT := 45

Global EQ_FIELD_OF_VIEW_MIN := 1
Global EQ_FIELD_OF_VIEW_MAX := 125

Global EQ_DRAW_DISTANCE_MIN := 1
Global EQ_DRAW_DISTANCE_MAX := 9999

Global EQ_CAMERA_VIEW := 0x00615F3C ; DWORD ; EQ_CAMERA_VIEW_...

Global EQ_CAMERA_VIEW_FIRST_PERSON   := 0
Global EQ_CAMERA_VIEW_THIRD_PERSON_1 := 1
Global EQ_CAMERA_VIEW_THIRD_PERSON_2 := 2
Global EQ_CAMERA_VIEW_THIRD_PERSON_3 := 3
Global EQ_CAMERA_VIEW_THIRD_PERSON_4 := 4
Global EQ_CAMERA_VIEW_THIRD_PERSON_5 := 5
Global EQ_CAMERA_VIEW_THIRD_PERSON_OVERHEAD := EQ_CAMERA_VIEW_THIRD_PERSON_1
Global EQ_CAMERA_VIEW_THIRD_PERSON_CHASE    := EQ_CAMERA_VIEW_THIRD_PERSON_2
Global EQ_CAMERA_VIEW_THIRD_PERSON_TETHER   := EQ_CAMERA_VIEW_THIRD_PERSON_5

Global EQ_CAMERA_HEADING_MIN := 0
Global EQ_CAMERA_HEADING_MAX := 512

Global EQ_SPEED_HACK_SPEED_MODIFIER := 0x00615013 ; code cave address

Global EQ_ASM_SET_PLAYER_VISION_IF_NOT_DARK_ELF := 0x0041FB78 ; eqgame.exe+1FB72 - C7 86 7C0C0000 FF000000 - mov [esi+00000C7C],000000FF
Global EQ_ASM_SET_PLAYER_VISION_IF_NOT_DARK_ELF_BYTE_DEFAULT := 0xFF ; mov [esi+00000C7C],000000FF
Global EQ_ASM_SET_PLAYER_VISION_IF_NOT_DARK_ELF_BYTE_REPLACE := 0x00 ; mov [esi+00000C7C],00000000

; eqgame.exe+17689 - 38 98 A8010000 - cmp [eax+000001A8],bl ; check if blind
; eqgame.exe+17692 - FF 15 14D77900 - call dword ptr [eqgame.exe+39D714] -> EQGfx_Dx8.t3dClearRenderToBlack ; do black screen
Global EQ_ASM_NEVER_BLIND := 0x0041768F ; eqgame.exe+1768F - 74 08 - je eqgame.exe+17699
Global EQ_ASM_NEVER_BLIND_BYTE_DEFAULT := 0x74 ; je eqgame.exe+17699
Global EQ_ASM_NEVER_BLIND_BYTE_REPLACE := 0xEB ; jmp eqgame.exe+17699 ; jump over black screen

;eqgame.exe+60B0D - 38 8E EA000000 - cmp [esi+000000EA],cl ; check if can walk through walls
Global EQ_ASM_WALK_THROUGH_WALLS := 0x00460B13 ; eqgame.exe+60B13 - 75 09 - jne eqgame.exe+60B1E
Global EQ_ASM_WALK_THROUGH_WALLS_BYTE_DEFAULT := 0x75 ; jne eqgame.exe+60B1E
Global EQ_ASM_WALK_THROUGH_WALLS_BYTE_REPLACE := 0xEB ; jmp eqgame.exe+60B1E ; jump

Global EQ_FALL_DAMAGE_MODIFIER := 0x005C5954

Global EQ_FALL_DAMAGE_DEFAULT := -2.5 ; greater than 0 can cause immediate fall to death
Global EQ_FALL_DAMAGE_NONE    := -999999

Global EQ_JUMP_MODIFIER := 0x005BE45C ; results may vary

Global EQ_JUMP_DEFAULT := -1

Global EQ_IS_SWIMMING_TRUE := 0x05

Global EQ_IS_STUNNED_UNCONSCIOUS := 3

Global EQ_RACE_UNKNOWN   := 0
Global EQ_RACE_HUMAN     := 1
Global EQ_RACE_BARBARIAN := 2
Global EQ_RACE_ERUDITE   := 3
Global EQ_RACE_WOOD_ELF  := 4
Global EQ_RACE_HIGH_ELF  := 5
Global EQ_RACE_DARK_ELF  := 6
Global EQ_RACE_HALF_ELF  := 7
Global EQ_RACE_DWARF     := 8
Global EQ_RACE_TROLL     := 9
Global EQ_RACE_OGRE      := 10
Global EQ_RACE_HALFLING  := 11
Global EQ_RACE_GNOME     := 12
Global EQ_RACE_IKSAR     := 128
Global EQ_RACE_VAH_SHIR  := 130
Global EQ_RACE_FROGLOK   := 330
Global EQ_RACE_DRAKKIN   := -1 ; TODO

Global EQ_CLASS_UNKNOWN       := 0
Global EQ_CLASS_WARRIOR       := 1
Global EQ_CLASS_CLERIC        := 2
Global EQ_CLASS_PALADIN       := 3
Global EQ_CLASS_RANGER        := 4
Global EQ_CLASS_SHADOWKNIGHT  := 5
Global EQ_CLASS_DRUID         := 6
Global EQ_CLASS_MONK          := 7
Global EQ_CLASS_BARD          := 8
Global EQ_CLASS_ROGUE         := 9
Global EQ_CLASS_SHAMAN        := 10
Global EQ_CLASS_NECROMANCER   := 11
Global EQ_CLASS_WIZARD        := 12
Global EQ_CLASS_MAGICIAN      := 13
Global EQ_CLASS_ENCHANTER     := 14
Global EQ_CLASS_BEASTLORD     := 15
Global EQ_CLASS_BERSERKER     := 16

Global EQ_CLASS_LAST          := 16

Global EQ_CLASS_GUILDMASTER_MIN := 20
Global EQ_CLASS_GUILDMASTER_MAX := 35

Global EQ_CLASS_BANKER        := 40
Global EQ_CLASS_SHOPKEEPER    := 41

Global EQ_STUNNED_STATE_FALSE       := 0x00
Global EQ_STUNNED_STATE_TRUE        := 0x01
Global EQ_STUNNED_STATE_UNCONSCIOUS := 0x03 ; lying on the floor unconscious and bleeding to death

Global EQ_STANDING_STATE_STANDING := 0x64
Global EQ_STANDING_STATE_FROZEN   := 0x66 ; mesmerized / feared ; You lose control of yourself!
Global EQ_STANDING_STATE_LOOTING  := 0x69 ; looting or bind wound
Global EQ_STANDING_STATE_SITTING  := 0x6E
Global EQ_STANDING_STATE_DUCKING  := 0x6F ; crouching
Global EQ_STANDING_STATE_FEIGNED  := 0x73
Global EQ_STANDING_STATE_DEAD     := 0x78

EQ_GetZonePlayerName()
{
    result := Memory_ReadString(eq_process_handle, EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_PLAYER_NAME, EQ_NAME_SIZE)

    Return, result
}

EQ_GetZoneShortName()
{
    result := Memory_ReadString(eq_process_handle, EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_SHORT_NAME, EQ_ZONE_SHORT_NAME_SIZE)

    Return, result
}

EQ_GetZoneLongName()
{
    result := Memory_ReadString(eq_process_handle, EQ_ZONE_INFO_STRUCTURE + EQ_OFFSET_ZONE_INFO_LONG_NAME,  EQ_ZONE_LONG_NAME_SIZE)

    Return, result
}

EQ_GetZoneShortNameLive()
{
    result := Memory_ReadString(eq_process_handle, EQ_ZONE_SHORT_NAME_LIVE, EQ_ZONE_SHORT_NAME_SIZE)

    Return, result
}

EQ_GetZoneClipMin()
{
    result := Memory_ReadFloat(eq_process_handle, zone_info + EQ_OFFSET_ZONE_INFO_CLIP_MIN)

    Return, result
}

EQ_GetZoneClipMax()
{
    result := Memory_ReadFloat(eq_process_handle, zone_info + EQ_OFFSET_ZONE_INFO_CLIP_MAX)

    Return, result
}

EQ_GetCharInfo()
{
    result := Memory_Read(eq_process_handle, EQ_CHAR_INFO_POINTER)

    Return, result
}

EQ_GetPlayerSpawnInfo()
{
    result := Memory_Read(eq_process_handle, EQ_PLAYER_SPAWN_INFO_POINTER)

    Return, result
}

EQ_GetPlayerActorInfo()
{
    player_spawn_info := EQ_GetPlayerSpawnInfo()

    result := Memory_Read(eq_process_handle, player_spawn_info + EQ_OFFSET_SPAWN_INFO_ACTOR_INFO_POINTER)

    Return, result
}

EQ_GetTargetSpawnInfo()
{
    result := Memory_Read(eq_process_handle, EQ_TARGET_SPAWN_INFO_POINTER)

    Return, result
}

EQ_GetCamera()
{
    result := Memory_Read(eq_process_handle, EQ_CAMERA_POINTER)

    Return, result
}

EQ_GetRaceShortName(race)
{
    result := ""

    If (race = EQ_RACE_UNKNOWN)
    {
        result := "UNK"
    }

    If (race = EQ_RACE_HUMAN)
    {
        result := "HUM"
    }

    If (race = EQ_RACE_BARBARIAN)
    {
        result := "BAR"
    }

    If (race = EQ_RACE_ERUDITE)
    {
        result := "ERU"
    }

    If (race = EQ_RACE_WOOD_ELF)
    {
        result := "ELF"
    }

    If (race = EQ_RACE_HIGH_ELF)
    {
        result := "HIE"
    }

    If (race = EQ_RACE_DARK_ELF)
    {
        result := "DEF"
    }

    If (race = EQ_RACE_HALF_ELF)
    {
        result := "HEF"
    }

    If (race = EQ_RACE_DWARF)
    {
        result := "DWF"
    }

    If (race = EQ_RACE_TROLL)
    {
        result := "TRL"
    }

    If (race = EQ_RACE_OGRE)
    {
        result := "OGR"
    }

    If (race = EQ_RACE_HALFLING)
    {
        result := "HFL"
    }

    If (race = EQ_RACE_GNOME)
    {
        result := "GNM"
    }

    If (race = EQ_RACE_IKSAR)
    {
        result := "IKS"
    }

    If (race = EQ_RACE_VAH_SHIR)
    {
        result := "VAH"
    }

    If (race = EQ_RACE_FROGLOK)
    {
        result := "FRG"
    }

    Return, result
}

EQ_GetClassShortName(class)
{
    result := ""

    If (class = EQ_CLASS_UNKNOWN)
    {
        result := "UNK"
    }

    If (class = EQ_CLASS_WARRIOR)
    {
        result := "WAR"
    }

    If (class = EQ_CLASS_CLERIC)
    {
        result := "CLR"
    }

    If (class = EQ_CLASS_PALADIN)
    {
        result := "PAL"
    }

    If (class = EQ_CLASS_RANGER)
    {
        result := "RNG"
    }

    If (class = EQ_CLASS_SHADOWKNIGHT)
    {
        result := "SHD"
    }

    If (class = EQ_CLASS_DRUID)
    {
        result := "DRU"
    }

    If (class = EQ_CLASS_MONK)
    {
        result := "MNK"
    }

    If (class = EQ_CLASS_BARD)
    {
        result := "BRD"
    }

    If (class = EQ_CLASS_ROGUE)
    {
        result := "ROG"
    }

    If (class = EQ_CLASS_SHAMAN)
    {
        result := "SHM"
    }

    If (class = EQ_CLASS_NECROMANCER)
    {
        result := "NEC"
    }

    If (class = EQ_CLASS_WIZARD)
    {
        result := "WIZ"
    }

    If (class = EQ_CLASS_MAGICIAN)
    {
        result := "MAG"
    }

    If (class = EQ_CLASS_ENCHANTER)
    {
        result := "ENC"
    }

    If (class = EQ_CLASS_BEASTLORD)
    {
        result := "BST"
    }

    If (class = EQ_CLASS_BERSERKER)
    {
        result := "BER"
    }

    Return, result
}

EQ_GetGuildName(guild_id)
{
    guild_name_address := EQ_GUILD_NAMES_BEGIN

    Loop, %EQ_GUILD_NAMES_MAX%
    {
        guild_index = %A_Index%

        If (guild_index = guild_id)
        {
            guild_name := Memory_ReadString(eq_process_handle, guild_name_address, EQ_GUILD_NAME_SIZE)

            Return, guild_name
        }

        guild_name_address := guild_name_address + EQ_GUILD_NAME_SIZE
    }

    guild_name := "Unknown Guild"

    Return, guild_name
}