#ifndef EQDALAYA_HPP
#define EQDALAYA_HPP

#include <string>
#include <cmath>

#include <windows.h>

#include "memory.hpp"

#define EVERQUEST_IS_AUTO_ATTACK_ENABLED 0x00729962 // BYTE

#define EVERQUEST_PLAYER_SPAWN_INFO_POINTER 0x0072B7A0 // POINTER to STRUCT
#define EVERQUEST_TARGET_SPAWN_INFO_POINTER 0x0072B7AC // POINTER to STRUCT

#define EVERQUEST_SPAWN_INFO_NULL 0x00000000

#define EVERQUEST_OFFSET_SPAWN_INFO_Y 0x068 // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_X 0x06C // FLOAT
#define EVERQUEST_OFFSET_SPAWN_INFO_Z 0x070 // FLOAT

#define EVERQUEST_OFFSET_SPAWN_INFO_STANDING_STATE 0x179 // BYTE ; EVERQUEST_STANDING_STATE_x
#define EVERQUEST_OFFSET_SPAWN_INFO_CLASS          0x17A // BYTE ; EVERQUEST_CLASS_x

#define EVERQUEST_CLASS_WARRIOR 1

#define EVERQUEST_STANDING_STATE_STANDING 0x64
#define EVERQUEST_STANDING_STATE_FROZEN   0x66 // mesmerized / feared ; You lose control of yourself!
#define EVERQUEST_STANDING_STATE_LOOTING  0x69 // looting or bind wound
#define EVERQUEST_STANDING_STATE_SITTING  0x6E
#define EVERQUEST_STANDING_STATE_DUCKING  0x6F // crouching
#define EVERQUEST_STANDING_STATE_FEIGNED  0x73
#define EVERQUEST_STANDING_STATE_DEAD     0x78

#include "eqdalaya_functions.hpp"

float everquest_calculate_distance(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int everquest_get_player_spawn_info(memory &memory)
{
    return memory.read_any<DWORD>(EVERQUEST_PLAYER_SPAWN_INFO_POINTER);
}

int everquest_get_target_spawn_info(memory &memory)
{
    return memory.read_any<DWORD>(EVERQUEST_TARGET_SPAWN_INFO_POINTER);
}

bool everquest_is_auto_attack_enabled(memory &memory)
{
    return memory.read_any<BYTE>(EVERQUEST_IS_AUTO_ATTACK_ENABLED);
}

#endif // EQMAC_HPP
