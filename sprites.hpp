#ifndef GAME_ENGINE_SPRITES_HPP
#define GAME_ENGINE_SPRITES_HPP

// for print time of reading sprites
#include "Objects/time.hpp"

// for Sprite and Spritesheet
#include "Objects/Sprite/sprite_object.hpp"

enum sprite_t : uint8_t {

    // world

    SP_BUSH,
    SP_TREE,
    SP_LOG,

    SP_FIREPLACE,

    SP_BARREL,

    SP_PILLAR,
    SP_BROKEN_PILLAR,

    SP_CHESTER,
    // SP_OPEN_CHESTER,

    SP_NUN_STATUE,

    SP_TELEPORT,
    SP_TABLE,

    SP_WOODEN_FLOOR,

    // wall

    SP_WALL_LEFT,
    SP_WALL_RIGHT,
    SP_WALL_BOTTOM,
    SP_WALL_TOP,

    SP_WALL_BOTTOM_LEFT,
    SP_WALL_BOTTOM_RIGHT,
    SP_WALL_TOP_LEFT,
    SP_WALL_TOP_RIGHT,

    SP_WALL_BOTTOM_LEFT_INSIDE,
    SP_WALL_BOTTOM_RIGHT_INSIDE,
    SP_WALL_TOP_LEFT_INSIDE,
    SP_WALL_TOP_RIGHT_INSIDE,

    SP_GRASS_BACKGROUND,
    SP_FLOOR,

    // shadows

    SP_LARGE_SHADOW,
    SP_MEDIUM_SHADOW,
    SP_SMALL_SHADOW,

    SP_SLIME_MEDIUM_SHADOW,
    SP_SLIME_LARGE_SHADOW,

    // UI
    SP_CURSOR,
    SP_FOCUS_CURSOR,
    SP_HEART,
    SP_COIN,

    SP_KEK,

    // bullets
    SP_SLIME_BULLET,

    SP_COUNT,
    // НИЖЕ SP_COUNT ничего не должно быть, потому что SP_COUNT является
    // количеством элементов в sprite_t
};

enum spritesheet_t : uint8_t {
    SS_PLAYER_CLOACK,
    SS_PLAYER_BODY,
    SS_PLAYER_T_SHIRT,

    // enemies

    SS_BAT,
    SS_BAT_INVERSE,

    SS_SLIME,

    // effects
    SS_BOOM,
    SS_HIT_EFFECT,
    SS_DEATH_EFFECT,
    SS_FLOWER_EFFECT,
    SS_PORTAL_EFFECT,

    // weapons
    SS_GOLDEN_GUN,

    SS_COUNT,
    // НИЖЕ SS_COUNT ничего не должно быть, потому что SS_COUNT является
    // количеством элементов в spritesheet_t
};

std::string sprite_type_to_string(sprite_t sp);

sprite_t string_to_sprite_type(const std::string &str);

Sprite &get_sprite(sprite_t type);

Spritesheet &get_spritesheet(spritesheet_t type);

void read_sprites();

void read_spritesheets();

#endif  // GAME_ENGINE_SPRITES_HPP