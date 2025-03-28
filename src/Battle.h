/*
    Zlib License

    Copyright (c) 2024 SpyterDev

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
        misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once


#include "Animation.h"
#include <stdint.h>
#include <time.h>

enum _Battle_Attacks_Types
{
    NONE, 
    HIT, // Any Enemy
    MICTOSS, PIZZAWHEEL, BIRTHDAY // Freddy
};

typedef struct _Damage_Range
{
    uint32_t min, max;
} _Damage_Range;

typedef struct _Battle_Projectile 
{
    clock_t max_lifetime;
    _Damage_Range damage;
    Vector2 velocity;
    Rectangle hitbox;
    Animation_V2 sprite;
} _Battle_Projectile;

typedef struct _Attack 
{
    _Damage_Range damage;
    enum _Battle_Attacks_Types type;
    clock_t delay;
} _Attack;

enum ENTITY_IDs
{
    UNKNOWN,
    FREDDY,
    BONNIE,
    CHICA,
    FOXY,
    BOUNCEPOT,
    GEARRAT,
    MECHRAB
};
typedef struct _BattleEntity 
{
    uint8_t num_of_attacks; // Mostly for Enemies
    uint32_t remaining_health, full_health;
    char name[20];
    clock_t last_attack;
    Animation_V2 sprite_idle;
    Animation_V2 sprite_attack;
    Rectangle hitbox;
    _Attack attacks[3];
} _BattleEntity;

typedef struct _BattleParty
{
    uint8_t size;
    _BattleEntity member[4];
} _BattleParty;

extern void InitBattle(void);
extern void UninitBattle(void);
extern void PutBattle(void);