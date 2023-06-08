#include "sprites.hpp"

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
        foo(SP_KNIGHT);

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
        // bullets
        foo(SP_SLIME_BULLET);
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

Sprite &get_sprite(sprite_t type) {
    static std::vector<Sprite> Sprites(sprite_t::SP_COUNT);
    return Sprites.at(type);
}

Spritesheet &get_spritesheet(spritesheet_t type) {
    static std::vector<Spritesheet> Spritesheets(spritesheet_t::SS_COUNT);
    return Spritesheets.at(type);
}

void read_sprites() {
    Timer finish_time;

    std::cout << "read_sprites:\n";

#define read(type, path)                                                                                         \
    {                                                                                                            \
        Timer time;                                                                                              \
        get_sprite(type) = read_sprite_from_png(std::string("Sprites\\") + path);                                \
        std::cout << #type << " " << time << " " << get_sprite(type).height() << 'x' << get_sprite(type).width() \
                  << std::endl;                                                                                  \
    }

    read(SP_KEK, "kek.png");

    // world
    {
        read(SP_BUSH, "World\\bush.png");
        read(SP_TREE, "World\\tree.png");
        read(SP_LOG, "World\\log.png");

        read(SP_FIREPLACE, "World\\fireplace.png");

        read(SP_GRASS_BACKGROUND, "World\\grass_background.png");

        read(SP_BARREL, "World\\barrel.png");
        read(SP_PILLAR, "World\\pillar.png");
        read(SP_BROKEN_PILLAR, "World\\broken_pillar.png");
        read(SP_CHESTER, "World\\chester.png");
        // read(SP_OPEN_CHESTER, "World\\open_chester.png");
        read(SP_NUN_STATUE, "World\\nun_statue.png");
        read(SP_KNIGHT, "World\\knight.png");

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

        read(SP_WALL_BOTTOM_LEFT_INSIDE, "World\\Wall\\wall_bottom_left_inside_fill.png");
        read(SP_WALL_BOTTOM_RIGHT_INSIDE, "World\\Wall\\wall_bottom_right_inside_fill.png");
        read(SP_WALL_TOP_LEFT_INSIDE, "World\\Wall\\wall_top_left_inside_fill.png");
        read(SP_WALL_TOP_RIGHT_INSIDE, "World\\Wall\\wall_top_right_inside_fill.png");
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
    // bullets
    { read(SP_SLIME_BULLET, "Enemies\\slime_bullet.png"); }

#undef read

    std::cout << "total_time: " << finish_time << std::endl << std::endl;
}

void read_spritesheets() {
    Timer finish_time;
    std::cout << "read_spritesheets:\n";

#define read(type, path, frame_len_x)                                                                      \
    {                                                                                                      \
        Timer time;                                                                                        \
        auto sprite = read_sprite_from_png(std::string("Sprites\\") + (path));                             \
        get_spritesheet(type) = Spritesheet(sprite, frame_len_x);                                          \
        std::cout << #type << " " << time << " " << sprite.height() << 'x' << sprite.width() << std::endl; \
    }

    // player
    {
        read(SS_PLAYER_CLOACK, "Player\\player_cloack.png", 64);
        read(SS_PLAYER_BODY, "Player\\player_body.png", 64);
        read(SS_PLAYER_T_SHIRT, "Player\\player_t_shirt.png", 64);
        // read(SP_PLAYER_CLOSED_EYES, "Player\\player_closed_eyes.png");
    }

    // enemies
    {
        read(SS_BAT, "Enemies\\bat.png", 16);
        read(SS_BAT_INVERSE, "Enemies\\bat_inverse.png", 16);

        read(SS_SLIME, "Enemies\\slime.png", 64);
        read(SS_BOMBER, "Enemies\\bomber.png", 64);
    }

    // effects
    {
        read(SS_HIT_EFFECT, "Effects\\hit_effect.png", 24);
        read(SS_DEATH_EFFECT, "Effects\\death_effect.png", 32);
        read(SS_BOOM, "Effects\\boom_effect.png", 128);
        read(SS_FLOWER_EFFECT, "Effects\\flower_effect.png", 32);
    }
    // weapons
    {
        read(SS_GOLDEN_GUN, "Weapons\\golden_gun.png", 42);
        read(SS_STAFF, "Weapons\\staff_of_life.png", 32);

    }

#undef read

    std::cout << "total_time: " << finish_time << std::endl << std::endl;
}