#ifndef GAME_ENGINE_SPRITES_HPP
#define GAME_ENGINE_SPRITES_HPP

// for print time of reading sprites
#include "Objects/time.hpp"

// for Sprite and Spritesheet
#include "Objects/Sprite/sprite_object.hpp"

enum sprite_t {

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

    SP_COUNT,
    // НИЖЕ SP_COUNT ничего не должно быть, потому что SP_COUNT является
    // количеством элементов в sprite_t
};

enum spritesheet_t : u8{
    SS_PLAYER,

    // enemies

    SS_BAT,
    SS_BAT_INVERSE,

    SS_SLIME,

    // effects

    SS_HIT_EFFECT,
    SS_DEATH_EFFECT,
    SS_PORTAL_EFFECT,

    // weapons
    SS_GOLDEN_GUN,

    SS_COUNT,
    // НИЖЕ SS_COUNT ничего не должно быть, потому что SS_COUNT является
    // количеством элементов в spritesheet_t
};

std::string sprite_type_to_string(sprite_t sp) {
#define foo(x)     \
    case x: {      \
        return #x; \
    }

    switch (sp) {
        foo(SP_BUSH);
        foo(SP_TREE);
        foo(SP_LOG);

        foo(SP_FIREPLACE);

        foo(SP_BARREL);

        foo(SP_PILLAR);
        foo(SP_BROKEN_PILLAR);

        foo(SP_CHESTER);
        // foo(SP_OPEN_CHESTER);

        foo(SP_NUN_STATUE);

        foo(SP_TELEPORT);
        foo(SP_TABLE);

        foo(SP_WOODEN_FLOOR);

        // wall

        foo(SP_WALL_LEFT);
        foo(SP_WALL_RIGHT);
        foo(SP_WALL_BOTTOM);
        foo(SP_WALL_TOP);

        foo(SP_WALL_BOTTOM_LEFT);
        foo(SP_WALL_BOTTOM_RIGHT);
        foo(SP_WALL_TOP_LEFT);
        foo(SP_WALL_TOP_RIGHT);

        foo(SP_WALL_BOTTOM_LEFT_INSIDE);
        foo(SP_WALL_BOTTOM_RIGHT_INSIDE);
        foo(SP_WALL_TOP_LEFT_INSIDE);
        foo(SP_WALL_TOP_RIGHT_INSIDE);

        foo(SP_GRASS_BACKGROUND);
        foo(SP_FLOOR);

        // shadows

        foo(SP_LARGE_SHADOW);
        foo(SP_MEDIUM_SHADOW);
        foo(SP_SMALL_SHADOW);

        foo(SP_SLIME_MEDIUM_SHADOW);
        foo(SP_SLIME_LARGE_SHADOW);

        // UI
        foo(SP_CURSOR);
        foo(SP_FOCUS_CURSOR);
        foo(SP_HEART);
        foo(SP_COIN);
        foo(SP_KEK);
    }
#undef foo

    FAILED_ASSERT("undefined sprite to string");
}

sprite_t string_to_sprite_type(const std::string &str) {
    for (int i = 0; i != SP_COUNT; i++) {
        if (sprite_type_to_string(static_cast<sprite_t>(i)) == str) {
            return static_cast<sprite_t>(i);
        }
    }
    FAILED_ASSERT("undefined string to sprite");
}

std::vector<Sprite> Sprites(sprite_t::SP_COUNT);

std::vector<Spritesheet> Spritesheets(spritesheet_t::SS_COUNT);

void read_sprites() {
    Timer finish_time;

    std::cout << "read_sprites:\n";

#define read(type, path)                                                       \
    {                                                                          \
        Timer time;                                                            \
        Sprites[type] = read_sprite_from_png(std::string("Sprites\\") + path); \
        std::cout << #type << " " << time << " " << Sprites[type].height()     \
                  << 'x' << Sprites[type].width() << std::endl;                \
    }

    read(SP_KEK, "kek.png");

    // world
    {
        read(SP_BUSH, "World\\bush.png");
        read(SP_TREE, "World\\tree.png");
        read(SP_LOG, "World\\log.png");

        read(SP_FIREPLACE, "World\\fireplace.png");

        read(SP_GRASS_BACKGROUND, "World\\grass_background.png");
        // read(SP_WALL, "World\\wall.png");
        read(SP_FLOOR, "World\\floor.png");

        read(SP_BARREL, "World\\barrel.png");
        read(SP_PILLAR, "World\\pillar.png");
        read(SP_BROKEN_PILLAR, "World\\broken_pillar.png");
        read(SP_CHESTER, "World\\chester.png");
        // read(SP_OPEN_CHESTER, "World\\open_chester.png");
        read(SP_NUN_STATUE, "World\\nun_statue.png");
        read(SP_TELEPORT, "World\\teleport.png");

        read(SP_TABLE, "World\\table.png");

        // read(SP_CARPET_PATTERN, "World\\carpet_pattern.png");
        // read(SP_CARPET_TOP_LEFT, "World\\carpet_top_left.png");

        read(SP_WOODEN_FLOOR, "World\\wooden_floor.png");
    }

    // Walls
    {
        read(SP_WALL_LEFT, "World\\Wall\\wall_left_fill.png");
        read(SP_WALL_RIGHT, "World\\Wall\\wall_right_fill.png");
        read(SP_WALL_BOTTOM, "World\\Wall\\wall_bottom_fill.png");
        read(SP_WALL_TOP, "World\\Wall\\wall_top_fill.png");

        read(SP_WALL_BOTTOM_LEFT, "World\\Wall\\wall_bottom_left_fill.png");
        read(SP_WALL_BOTTOM_RIGHT, "World\\Wall\\wall_bottom_right_fill.png");
        read(SP_WALL_TOP_LEFT, "World\\Wall\\wall_top_left_fill.png");
        read(SP_WALL_TOP_RIGHT, "World\\Wall\\wall_top_right_fill.png");

        read(
            SP_WALL_BOTTOM_LEFT_INSIDE,
            "World\\Wall\\wall_bottom_left_inside_fill.png"
        );
        read(
            SP_WALL_BOTTOM_RIGHT_INSIDE,
            "World\\Wall\\wall_bottom_right_inside_fill.png"
        );
        read(
            SP_WALL_TOP_LEFT_INSIDE,
            "World\\Wall\\wall_top_left_inside_fill.png"
        );
        read(
            SP_WALL_TOP_RIGHT_INSIDE,
            "World\\Wall\\wall_top_right_inside_fill.png"
        );
    }

    // Shadows
    {
        read(SP_LARGE_SHADOW, "Shadows\\large_shadow.png");
        read(SP_MEDIUM_SHADOW, "Shadows\\medium_shadow.png");
        read(SP_SMALL_SHADOW, "Shadows\\small_shadow.png");

        read(SP_SLIME_MEDIUM_SHADOW, "Shadows\\slime_medium_shadow.png");
        read(SP_SLIME_LARGE_SHADOW, "Shadows\\slime_large_shadow.png");
    }

    // UI
    {
        read(SP_CURSOR, "UI\\cursor.png");
        read(SP_FOCUS_CURSOR, "UI\\focus_cursor.png");
        read(SP_HEART, "UI\\heart.png");
        read(SP_COIN, "UI\\coin.png");
    }

#undef read

    std::cout << "total_time: " << finish_time << std::endl << std::endl;
}

void read_spritesheets() {
    Timer finish_time;
    std::cout << "read_spritesheets:\n";

#define read(type, path, frame_len_x)                                          \
    {                                                                          \
        Timer time;                                                            \
        auto sprite = read_sprite_from_png(std::string("Sprites\\") + (path)); \
        Spritesheets[type] = Spritesheet(sprite, frame_len_x);                 \
        std::cout << #type << " " << time << " " << sprite.height() << 'x'     \
                  << sprite.width() << std::endl;                              \
    }

    // player
    {
        read(SS_PLAYER, "Player\\player.png", 64);
        // read(SP_PLAYER_CLOSED_EYES, "Player\\player_closed_eyes.png");
    }

    // enemies
    {
        read(SS_BAT, "Enemies\\bat.png", 16);
        read(SS_BAT_INVERSE, "Enemies\\bat_inverse.png", 16);

        read(SS_SLIME, "Enemies\\slime.png", 64);
    }

    // effects
    {
        read(SS_HIT_EFFECT, "Effects\\hit_effect.png", 24);
        read(SS_DEATH_EFFECT, "Effects\\death_effect.png", 32);
    }
    // weapons
    { read(SS_GOLDEN_GUN, "Weapons\\golden_gun_big.png", 800); }

#undef read

    std::cout << "total_time: " << finish_time << std::endl << std::endl;
}

#endif  // GAME_ENGINE_SPRITES_HPP