#ifndef GAME_PLAYER_HPP
#define GAME_PLAYER_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"
#include "weapon.hpp"

const static animation player_anims[] = {
    animation(SS_PLAYER_BODY, 0, 6, 0.1),   // run right
    animation(SS_PLAYER_BODY, 6, 6, 0.1),   // run up
    animation(SS_PLAYER_BODY, 12, 6, 0.1),  // run left
    animation(SS_PLAYER_BODY, 18, 6, 0.1),  // run down

    animation(SS_PLAYER_BODY, 24, 4, 0.12),  // attack right
    animation(SS_PLAYER_BODY, 28, 4, 0.12),  // attack up
    animation(SS_PLAYER_BODY, 32, 4, 0.12),  // attack left
    animation(SS_PLAYER_BODY, 36, 4, 0.12),  // attack down

    animation(SS_PLAYER_BODY, 40, 5, 0.1),  // roll right
    animation(SS_PLAYER_BODY, 45, 5, 0.1),  // roll up
    animation(SS_PLAYER_BODY, 50, 5, 0.1),  // roll left
    animation(SS_PLAYER_BODY, 55, 5, 0.1),  // roll down

    animation(SS_PLAYER_BODY, 0, 1, 0.1),   // idle right
    animation(SS_PLAYER_BODY, 6, 1, 0.1),   // idle up
    animation(SS_PLAYER_BODY, 12, 1, 0.1),  // idle left
    animation(SS_PLAYER_BODY, 18, 1, 0.1),  // idle down
};

// дерево анимаций игрока
struct Player_anim_tree {
    enum anim_t : uint8_t {
        NONE,
        RUN,
        ATTACK,
        ROLL,
        IDLE,

        COUNT,
    };

    struct anim_and_dir_t {
        anim_t anim = NONE;
        direction_t dir = direction_t::NONE;

        [[nodiscard]] unsigned int get_num() const;
    };

    // возвращает анимацию
    static anim_and_dir_t get_anim(Dot dir, anim_and_dir_t type);
};

struct Player : AbstractPhysicalObject {
    ADD_BYTE_SERIALIZATION();

    // visible
    inline static const efloat size = 1;
    inline static const Dot delta_draw_pos = Dot(-31, 40) * size;
    // physics
    inline static const efloat collision_radius = 6;
    inline static const efloat ddp_speed = 700;
    inline static const efloat jump_speed = 110;
    // cooldowns
    inline static const efloat jump_cooldown = 0;
    inline static const efloat invulnerable_cooldown = 2;

    inline static const std::vector<Color> customization_colors = {
        // white
        Color(0xffffff, 0xff),
        Color(0x9d9d9d, 0xff),

        // gray
        Color(0x7d7d7d, 0xff),
        Color(0x575757, 0xff),

        // yellow
        Color(0xf7e05a, 0xff),
        Color(0xad9d3f, 0xff),

        // brown
        Color(0xcd6b1d, 0xff),
        Color(0x8f4b14, 0xff),

        // pink
        Color(0xff6975, 0xff),
        Color(0xb24952, 0xff),

        // crimson
        Color(0xa80934, 0xff),
        Color(0x671638, 0xff),

        // blue
        Color(0x31a2f2, 0xff),
        Color(0x2271a9, 0xff),

        // purple
        Color(0xaf55dd, 0xff),
        Color(0x7a3b9B, 0xff),

        // green
        Color(0x00854a, 0xff),
        Color(0x006639, 0xff),
    };

    efloat jump_accum = 0;
    efloat invulnerable_accum = 0;
    efloat healing_cooldown = 0;
    int weapon_ind = -1;

    animation anim = player_anims[0];

    // для пушки, направление от позиции игрока
    Dot cursor_dir;

    Input input;

    bool is_jumped = false;     // прыгает
    bool is_paralyzed = false;  // парализован слаймом

    Player_anim_tree::anim_and_dir_t anim_type;

    // уникальный id клиента
    int client_id;

    // id кастомизированного цвета у плаща
    int cloack_color_id = 5;
    // id кастомизированного цвета у футболки
    int t_shirt_color_id = 6;
    bool now_is_customization = false;

    int hp = 10;
    int coins = 100;

    Player(const Dot &position = Dot());

    [[nodiscard]] bool is_dead() const;

    void die();

    void reborn();

    [[nodiscard]] bool is_invulnerable() const;

    void set_invulnerable();

    void simulate(efloat delta_time, Dot ddp, bool pressed_jump);

    void draw() override;

    void drop_weapon();

    void pick_weapon(int wp_ind, efloat delta_time);

    // возвращает направление анимации игрока
    [[nodiscard]] Dot get_dir() const;

    [[nodiscard]] std::unique_ptr<Collision> get_hitbox() const;

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;
};

// найти индекс в Players, у которого такой client_id
// вернет -1, если такого не нашли
int find_player_index(int client_id);

// найти самого близкого игрока к этой точке
// вернет index
int find_nearest_player_index(Dot pos);

// моб стоит в pos
// мы должны найти лучшую для него цель
// вернет client_id, а не index, для более безопасной работы
int find_best_player(Dot pos);

#endif  // GAME_PLAYER_HPP