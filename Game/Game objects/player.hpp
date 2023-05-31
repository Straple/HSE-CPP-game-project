﻿#ifndef GAME_PLAYER_HPP
#define GAME_PLAYER_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"
#include "game_utils.hpp"
#include "weapon.hpp"

const static animation player_anims[] = {
    animation(SS_PLAYER, 0, 6, 0.1),   // run right
    animation(SS_PLAYER, 6, 6, 0.1),   // run up
    animation(SS_PLAYER, 12, 6, 0.1),  // run left
    animation(SS_PLAYER, 18, 6, 0.1),  // run down

    animation(SS_PLAYER, 24, 4, 0.12),  // attack right
    animation(SS_PLAYER, 28, 4, 0.12),  // attack up
    animation(SS_PLAYER, 32, 4, 0.12),  // attack left
    animation(SS_PLAYER, 36, 4, 0.12),  // attack down

    animation(SS_PLAYER, 40, 5, 0.1),  // roll right
    animation(SS_PLAYER, 45, 5, 0.1),  // roll up
    animation(SS_PLAYER, 50, 5, 0.1),  // roll left
    animation(SS_PLAYER, 55, 5, 0.1),  // roll down

    animation(SS_PLAYER, 0, 1, 0.1),   // idle right
    animation(SS_PLAYER, 6, 1, 0.1),   // idle up
    animation(SS_PLAYER, 12, 1, 0.1),  // idle left
    animation(SS_PLAYER, 18, 1, 0.1),  // idle down
};

// дерево анимаций игрока
struct Player_anim_tree {
    enum anim_t : u8 {
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

        [[nodiscard]] unsigned int get_num() const {
            return (static_cast<unsigned int>(anim) - 1) * 4 + (static_cast<unsigned int>(dir) - 1);
        }
    };

    // возвращает анимацию
    static anim_and_dir_t get_anim(Dot dir, anim_and_dir_t type) {
        // дефолтная анимация
        if (type.anim == NONE) {
            return {IDLE, direction_t::RIGHT};
        }
        // run
        else if (dir.x < 0) {
            return {RUN, direction_t::LEFT};
        } else if (dir.x > 0) {
            return {RUN, direction_t::RIGHT};
        } else if (dir.y < 0) {
            return {RUN, direction_t::DOWN};
        } else if (dir.y > 0) {
            return {RUN, direction_t::UP};
        }
        // idle
        else {
            return {IDLE, type.dir};
        }
    }
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

    efloat jump_accum = 0;
    efloat invulnerable_accum = 0;

    Weapon weapon;

    animation anim = player_anims[0];

    // для пушки, направление от позиции игрока
    Dot cursor_dir;

    Input input;

    bool is_jumped = false;     // прыгает
    bool is_paralyzed = false;  // парализован слаймом

    Player_anim_tree::anim_and_dir_t anim_type;

    // уникальный id клиента
    int client_id;

    int hp = 10;
    int coins = 100;

    Player(const Dot &position = Dot()) {
        pos = position;

        weapon.cooldown = 0;  // мы боги
    }

    [[nodiscard]] bool is_dead() const {
        return hp <= 0;
    }

    void die() {
        hp = 0;
        set_invulnerable();
        add_death_effect(pos);
        // Сразу сделаем методом, чтобы сюда занести анимации и тд
    }

    void reborn() {
        // Аналогично, можно не только для возрождения но и для появления/телепортации использовать
        set_invulnerable();
        hp = 1;
    }

    [[nodiscard]] bool is_invulnerable() const {
        return invulnerable_accum > 0;
    }

    void set_invulnerable() {
        invulnerable_accum = invulnerable_cooldown;
    }

    void simulate(efloat delta_time, Dot ddp, bool pressed_jump) {
        jump_accum -= delta_time;

        if (is_paralyzed) {
            // игрока ест слайм
            anim_type.anim = Player_anim_tree::IDLE;
            anim = player_anims[anim_type.get_num()];
        } else if (is_jumped) {
            Dot dp_tmp = dp;
            simulate_move2d(pos, dp_tmp, Dot(), delta_time);

            if (anim.frame_update(delta_time)) {
                is_jumped = false;
                jump_accum = jump_cooldown;

                anim_type.anim = Player_anim_tree::IDLE;

                anim = player_anims[anim_type.get_num()];
            }
        } else if (pressed_jump && jump_accum <= 0) {
            is_jumped = true;

            anim_type.anim = Player_anim_tree::ROLL;
            anim = player_anims[anim_type.get_num()];

            Dot jump_dir;
            if (ddp.get_len() == 0) {
                jump_dir = get_dir();
            } else {
                jump_dir = ddp.normalize();
            }
            dp = jump_dir * jump_speed;
        } else {
            if (!is_dead()) {
                invulnerable_accum -= delta_time;
                weapon.simulate(delta_time, cursor_dir + pos);
            }

            // simulate move
            {
                ddp *= ddp_speed;
                simulate_move2d(pos, dp, ddp, delta_time);
            }

            // текущая анимация+
            auto current_anim = Player_anim_tree::get_anim(ddp.normalize(), anim_type);

            if (anim_type.get_num() != current_anim.get_num()) {  // у нас сменились анимации

                anim_type = current_anim;

                anim = player_anims[current_anim.get_num()];

                // начинаем с 1 кадра, чтобы мы сразу начинали движение
                anim.frame_cur_count = std::min<unsigned int>(1, anim.frame_count - 1);
            }

            anim.frame_update(delta_time);
        }
    }

    void draw() const override {
        draw_sprite(pos + Dot(-8, 3), size, SP_MEDIUM_SHADOW);

        if (invulnerable_accum > 0 || is_dead()) {
            anim.draw(pos + delta_draw_pos, size, [&](const Color &color) { return Color(0xffffff, 128); });
        } else {
            anim.draw(pos + delta_draw_pos, size);
        }

        if (!is_jumped && !is_dead()) {
            weapon.draw(pos, cursor_dir + pos);
        }

        draw_collision(*this);
        draw_hitbox(*this);
    }

    // возвращает направление анимации игрока
    [[nodiscard]] Dot get_dir() const {
        return get_direction(anim_type.dir);
    }

    [[nodiscard]] std::unique_ptr<Collision> get_hitbox() const {
        return std::make_unique<CollisionBox>(pos + Dot(-7, 20), pos + Dot(7, -5));
    }

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override {
        return std::make_unique<CollisionCircle>(pos, collision_radius);
    }
};

std::vector<Player> Players;

// найти индекс в Players, у которого такой client_id
// вернет -1, если такого не нашли
int find_player_index(int client_id) {
    for (std::size_t index = 0; index < Players.size(); index++) {
        if (Players[index].client_id == client_id) {
            return index;
        }
    }
    return -1;
}

// найти самого близкого игрока к этой точке
// вернет index
int find_nearest_player_index(Dot pos) {
    int best = -1;
    for (std::size_t index = 0; index < Players.size(); index++) {
        if (Players[index].is_dead() || Players[index].is_paralyzed) {
            continue;
        }
        if (best == -1 || (Players[index].pos - pos).get_len() < (Players[best].pos - pos).get_len()) {
            best = index;
        }
    }
    return best;
}

// моб стоит в pos
// мы должны найти лучшую для него цель
// вернет client_id, а не index, для более безопасной работы
int find_best_player(Dot pos) {
    // return find_nearest_player(pos);
    int best = -1;
    for (std::size_t index = 0; index < Players.size(); index++) {
        if (Players[index].is_dead() || Players[index].is_paralyzed) {
            continue;
        }
        if (best == -1 || (Players[index].pos - pos).get_len() < (Players[best].pos - pos).get_len()) {
            best = index;
        }
    }
    if (best == -1) {
        return -1;
    }
    return Players[best].client_id;
}

#endif  // GAME_PLAYER_HPP