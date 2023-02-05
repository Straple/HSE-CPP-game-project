enum sprite_t : u8 {

    // world

    SP_BUSH,
    SP_TREE,
    SP_LOG,

    SP_FIREPLACE,

    SP_GRASS_BACKGROUND,
    // SP_WALL,
    SP_FLOOR,

    // player

    SP_PLAYER,
    SP_PLAYER_CLOSED_EYES,

    // enemies

    SP_BAT,
    SP_BAT_INVERSE,

    SP_SLIME,

    // effects

    SP_HIT_EFFECT,
    SP_DEATH_EFFECT,

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
};

sprite Sprites[sprite_t::SP_COUNT];

void read_sprites() {
    Timer finish_time;

    std::cout << "read_sprites:\n";

#define read(type, name)                                         \
    {                                                            \
        Timer time;                                              \
        Sprites[type] = sprite(std::string("Sprites\\") + name); \
        std::cout << #type << " " << time << " "                 \
                  << Sprites[type].picture.height() << 'x'       \
                  << Sprites[type].picture.width() << std::endl; \
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
    }

    // player
    {
        read(SP_PLAYER, "Player\\player.png");
        read(SP_PLAYER_CLOSED_EYES, "Player\\player_closed_eyes.png");
    }

    // enemies
    {
        read(SP_BAT, "Enemies\\bat.png");
        read(SP_BAT_INVERSE, "Enemies\\bat_inverse.png");

        read(SP_SLIME, "Enemies\\slime.png");
    }

    // effects
    {
        read(SP_HIT_EFFECT, "Effects\\hit_effect.png");
        read(SP_DEATH_EFFECT, "Effects\\death_effect.png");
    }

    // shadow
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

    std::cout << "total_time: " << finish_time << std::endl;
}
