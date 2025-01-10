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

#include "Animation.h"
#include "Background.h"
#include "UI.h"
#include "Battle.h"
#include "World.h"
#include <malloc.h>
#include <stdint.h>

#define BATTLE_PIXEL_SCALE (100)

#define BATTLE_POSITION_TO_PIXEL_X(x) (BATTLE_PIXEL_SCALE * (x + 1280. / BATTLE_PIXEL_SCALE / 2))

#define BATTLE_POSITION_TO_PIXEL_Y(y) (BATTLE_PIXEL_SCALE * (y + 720. / BATTLE_PIXEL_SCALE / 2))

#define ENTITY_MACRO_BOUNCEPOT (_BattleEntity) {.name = "Bouncepot", .full_health = 100, .remaining_health = 100, .hitbox = (Rectangle) {0, 0, 1.75, 2.75}, .sprite_idle = CreateAnimation_V2("Assets/Battle/Entity_Sprites/Bouncepot/atlas.png", 30, 10, 250, 250)}

#define ENTITY_MACRO_FREDDY  (_BattleEntity) { .name = "Freddy", .full_health = 100, .remaining_health = 100, .hitbox = (Rectangle) {0, 0, 1.75, 2.75}, .sprite_idle = CreateAnimation_V2("Assets/Battle/Entity_Sprites/Freddy/idle.png", 30, 10, 250, 250)}

#define ENTITY_MACRO_BONNIE  (_BattleEntity) { .name = "Bonnie", .full_health = 100, .remaining_health = 100, .hitbox = (Rectangle) {0, 0, 1.75, 2.75}, .sprite_idle = CreateAnimation_V2("Assets/Battle/Entity_Sprites/Bonnie/idle.png", 30, 10, 250, 250)}

Font Battle_Font = {0};
Music theme = {0};

UITexture BattleBackground = {0};

_BattleParty Party_Enemy = {0};
_BattleParty Party_Player = {0};

enum ENTITY_POSITIONS_FIELD 
{
    L_U,
    L_L,
    L_R,
    L_D,
    R_U,
    R_L,
    R_R,
    R_D

};

void SetBattleEntity_Position(_BattleEntity * dest, enum ENTITY_POSITIONS_FIELD position)
{
    switch (position) {
        case L_U:
            dest -> hitbox.x = -3.5;
            dest -> hitbox.y = -3;
            return;
        case L_L:
            dest -> hitbox.x = -5.5;
            dest -> hitbox.y = -2;
            return;
        case L_R:
            dest -> hitbox.x = -2.5;
            dest -> hitbox.y = -1;
            return;
        case L_D:
            dest -> hitbox.x = -4.5;
            dest -> hitbox.y = 0;
            return;
        case R_U:
            dest -> hitbox.x = 2;
            dest -> hitbox.y = -3;
            return;
        case R_L:
            dest -> hitbox.x = 4;
            dest -> hitbox.y = -2;
            return;
        case R_R:
            dest -> hitbox.x = 1;
            dest -> hitbox.y = -1;
            return;
        case R_D:
            dest -> hitbox.x = 3;
            dest -> hitbox.y = 0;
            return;
    }
}

void LoadEntity(_BattleEntity * dest, _BattleEntity entity, enum ENTITY_POSITIONS_FIELD position)
{
    *dest = entity;

    SetBattleEntity_Position(dest, position);
}

// Turns a Unsigned Long into a Stack Allocated String
static char * __fastcall uintstr(uint64_t number) 
{
    uint64_t temp = number;
    uint64_t length = 1;
    while (temp > 9) temp /= 10, length++; // Checks number of digits and length of string
    char * string = alloca(length);
    temp = 0;
    while (temp < length) string[length-temp-1] = '0' + number - (number / 10) * 10, number/=10, temp++;
    string[temp] = '\0';
    char * stringLocation = string;
    return stringLocation;
}

void InitBattle(void)
{
    SetWindowTitle("FNaF World: C Edition - Battle");
    
    SetTraceLogLevel(LOG_NONE);

    switch ((enum WORLDZONES) (GetZone() + 1))
    {
        case FAZBEARHILLS:
            BattleBackground = LoadTexture("Assets/Battle/Background/Fazbear_Hills.png");
            break;
        case CHOPPYSWOODS:
            BattleBackground = LoadTexture("Assets/Battle/Background/Choppys_Woods.png");;
            break;
        case DUSTINGFIELDS:
            BattleBackground = LoadTexture("Assets/Battle/Background/Dusting_Fields.png");;
            break;
        case MYSTERIOUSMINES:
            BattleBackground = LoadTexture("Assets/Battle/Background/Mines.png");;
            break;
    }

    SetTextureFilter(BattleBackground, TEXTURE_FILTER_BILINEAR);

    Party_Enemy.size = 4;

    

    LoadEntity(Party_Enemy.member, ENTITY_MACRO_BOUNCEPOT, L_U);
    LoadEntity(Party_Enemy.member + 1, ENTITY_MACRO_BOUNCEPOT, L_L);
    LoadEntity(Party_Enemy.member + 2, ENTITY_MACRO_BOUNCEPOT, L_R);
    LoadEntity(Party_Enemy.member + 3, ENTITY_MACRO_BOUNCEPOT, L_D);

    Party_Player.size = 4;
    
    LoadEntity(Party_Player.member, ENTITY_MACRO_FREDDY, R_U);
    LoadEntity(Party_Player.member + 1, ENTITY_MACRO_BONNIE, R_L);
    LoadEntity(Party_Player.member + 2, ENTITY_MACRO_FREDDY, R_R);
    LoadEntity(Party_Player.member + 3, ENTITY_MACRO_BONNIE, R_D);
    
    theme = LoadMusicStream("Assets/Themes/battle_normal.wav");
    theme.looping = 1;
    
    Battle_Font = LoadFont("Assets/Battle/font.ttf");
    SetTextureFilter(Battle_Font.texture, TEXTURE_FILTER_BILINEAR);
    PlayMusicStream(theme);
    
    SetTraceLogLevel(LOG_ALL);
}

enum ATTACK_MOVE_INDEX
{
    FIRST = 0, 
    SECOND = 1, 
    THIRD = 2, 
    RANDOM = 3
};

typedef struct _AttackQueue 
{
    _Bool imminnent;
    _Attack attack;
    _BattleEntity * target;
    _BattleEntity * source;
} _AttackQueue;

#define MAX_ATTACKS_IN_QUEUE 15

_AttackQueue attack_queue[MAX_ATTACKS_IN_QUEUE] = {0};

static uint8_t GetAvaliable_attack_queue(void)
{
    return 0;
}
#define ATTACK_COOLDOWN (3 * CLOCKS_PER_SEC)

#define ATTACK_PARAMETERS _BattleParty * target_party, _BattleEntity * source, _Attack attack

static void _BattleEntity_Attack_Push(ATTACK_PARAMETERS)
{
    if (!target_party) return;

    uint8_t array_size = target_party -> size;

    uint16_t potental_targets[array_size] = {0xFF};

    uint8_t num_of_potental_targets = 0;

    for (uint8_t attempt_index; attempt_index < array_size; attempt_index++)
    {
        if (!target_party->member[attempt_index].remaining_health) continue;

        potental_targets[num_of_potental_targets] = attempt_index;

        num_of_potental_targets++;
    }

    uint8_t target_id = GetRandomValue(0, num_of_potental_targets);
    target_id = potental_targets[target_id];

    _BattleEntity * target = target_party -> member + target_id;
}

void _BattleEntity_Attack(uint8_t id, enum ATTACK_MOVE_INDEX attack_index)
{
    _BattleParty * target_party = id < Party_Player.size ? &Party_Enemy : &Party_Player;

    _BattleEntity * source = id < Party_Player.size ? Party_Player. member + id : 
                                                      Party_Enemy.member + id - Party_Player.size;

    if (source -> last_attack < ATTACK_COOLDOWN) return;

    if (attack_index == RANDOM) attack_index = GetRandomValue(FIRST, THIRD);
    
    _Attack attack = source -> attacks[attack_index - FIRST];

    _BattleEntity_Attack_Push(target_party, source, attack)
}

void UninitBattle(void)
{
    UnloadTexture(BattleBackground);
    UnloadFont(Battle_Font);
}

void RenderBattleEntity_Hitbox(register _BattleEntity * entity)
{
    DrawRectangle(  BATTLE_POSITION_TO_PIXEL_X(entity -> hitbox.x), 
                    BATTLE_POSITION_TO_PIXEL_Y(entity -> hitbox.y), 
                    BATTLE_PIXEL_SCALE * entity -> hitbox.width, 
                    BATTLE_PIXEL_SCALE * entity -> hitbox.height, RED);
}

void RenderBattleEntity(register _BattleEntity * entity)
{
    static float entity_scale = 1.5;
    Vector2 position = (Vector2) {  BATTLE_POSITION_TO_PIXEL_X(entity -> hitbox.x),
                                    BATTLE_POSITION_TO_PIXEL_Y(entity -> hitbox.y)};

    if (entity -> remaining_health == 0) return;
    
    #ifdef DEBUG 
    
    RenderBattleEntity_Hitbox(register _BattleEntity * entity);

    #endif

    DrawAnimation_V2(   &entity -> sprite_idle, 
                        position.x + (entity -> hitbox.width * BATTLE_PIXEL_SCALE / 2.) - entity -> sprite_idle.TileSize_x / 2.f * entity_scale, 
                        position.y + (entity -> hitbox.height * BATTLE_PIXEL_SCALE / 2.) - entity -> sprite_idle.TileSize_y / 2.f * entity_scale, entity_scale, 0);
}

void RenderBattle(void)
{
    RenderBackground(BattleBackground);

    static RenderTexture2D virtual_screen = {0};

    if (virtual_screen.texture.height == 0) virtual_screen = LoadRenderTexture(1280, 720);

    BeginTextureMode(virtual_screen);
    ClearBackground(BLANK);
    for (uint8_t i = 0; i < Party_Enemy.size; i++)
    {
        RenderBattleEntity(Party_Enemy.member + i);
    }

    for (uint8_t i = 0; i < Party_Player.size; i++)
    {
        RenderBattleEntity(Party_Player.member + i);
    }

    EndTextureMode();

    SetTextureFilter(BattleBackground, TEXTURE_FILTER_BILINEAR);

    float scale = GetScreenRatio() <= RATIO_16_9 ? GetScreenWidth() / 1280. : GetScreenHeight() / 720.;
    DrawTexturePro( virtual_screen.texture, 
                    (Rectangle) {0, -virtual_screen.texture.height, (float) virtual_screen.texture.width, (float) -virtual_screen.texture.height}, 
                    (Rectangle) {   GetScreenWidth() / 2. - virtual_screen.texture.width / 2. * scale, 
                                        GetScreenHeight() / 2. - virtual_screen.texture.height / 2. * scale, 
                                        virtual_screen.texture.width * scale, virtual_screen.texture.height * scale},
                    (Vector2) {0,0},
                    0,
                    WHITE);
    SetUIScreenScaleMode(HEIGHT);
}

void RenderHealthBar(_BattleEntity * entity, Color colour, uint8_t id, float scale)
{
    static float border_facter = 0.025;
    Rectangle dest = (Rectangle) { GetScreenWidth() - 210 * scale, 60 * id * scale + 10 * scale, 200 * scale, 50 * scale};

    Rectangle health_back_rec = (Rectangle) {dest.x + dest.width * border_facter, dest.y + dest.width * border_facter, dest.width * (1-border_facter*2), dest.height - dest.width * (border_facter * 2)};
    Rectangle health_rec = (Rectangle) {dest.x + dest.width * border_facter, dest.y + dest.width * border_facter, dest.width * (1-border_facter*2), dest.height - dest.width * (border_facter * 2)};
    health_rec.width *= (float) entity->remaining_health / entity->full_health;

    DrawRectangleRounded(dest, 0.1, 10, WHITE);
    DrawRectangleRec(health_back_rec, BLACK);
    DrawRectangleRec(health_rec, colour);

    DrawTextPro( (Font){0}, 
                "100", 
                (Vector2) { health_back_rec.x + health_back_rec.width, health_back_rec.y + health_back_rec.height / 2.}, (Vector2){MeasureText("100", 48), 48/2.}, 0, 48, 1, WHITE);
}

void PutBattle(void)
{
    UpdateMusicStream(theme);
    RenderBattle();
    
    RenderUIText(Party_Enemy.member[0].name, -0.945, -0.795, 0.06, LEFTMOST, Battle_Font, BLACK);
    RenderUIText(Party_Enemy.member[0].name, -0.95, -0.8, 0.06, LEFTMOST, Battle_Font, WHITE);
    //RenderHealthBar(Party_Player.member, RED, 0, GetScreenScale());
}