#ifndef GAME_PLAYER_HPP
#define GAME_PLAYER_HPP

#include "../../render.hpp"
#include "abstract_game_object.hpp"
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
            return (static_cast<unsigned int>(anim) - 1) * 4 +
                   (static_cast<unsigned int>(dir) - 1);
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

struct Player : abstract_game_object {
    inline static const efloat ddp_speed = 500;

    int hp = 400;

    efloat jump_cooldown;
    efloat jump_accum;

    // прыгает
    bool is_jumped = false;
    // парализован слаймом
    bool is_paralyzed = false;

    efloat invulnerable_cooldown;
    efloat invulnerable_accum;

    Weapon weapon;

    [[nodiscard]] bool is_invulnerable() const {
        return invulnerable_accum < invulnerable_cooldown;
    }

    void set_invulnerable() {
        invulnerable_accum = 0;
    }

    // animations
    animation anim;
    Player_anim_tree::anim_and_dir_t anim_type;

    Player(Dot position = Dot()) {
        // abstract_game_object
        pos = position;
        size = 1;
        collision_radius = 6;
        delta_draw_pos = Dot(-30, 40) * size;

        // player
        jump_accum = jump_cooldown = 0.5;
        invulnerable_accum = invulnerable_cooldown = 2;
    }

    void simulate(efloat delta_time, Dot ddp, bool pressed_jump) {
        if (is_paralyzed) {
            // игрока ест слайм
            anim_type.anim = Player_anim_tree::IDLE;
            anim = player_anims[anim_type.get_num()];
        } else if (is_jumped) {
            Dot dp_tmp = dp;
            simulate_move2d(pos, dp_tmp, Dot(), delta_time);

            if (anim.frame_update(delta_time)) {
                is_jumped = false;
                jump_accum = 0;

                anim_type.anim = Player_anim_tree::IDLE;

                anim = player_anims[anim_type.get_num()];
            }
        } else if (pressed_jump && jump_accum >= jump_cooldown) {
            is_jumped = true;

            anim_type.anim = Player_anim_tree::ROLL;
            anim = player_anims[anim_type.get_num()];

            if (ddp.get_len() == 0) {
                dp = get_dir() * 70;
            } else {
                dp = ddp.normalize() * 70;
            }
        } else {
            jump_accum += delta_time;
            invulnerable_accum += delta_time;

            weapon.simulate(delta_time);

            // simulate move
            {
                ddp *= ddp_speed;

                simulate_move2d(pos, dp, ddp, delta_time);
            }

            // текущая анимация+
            auto current_anim =
                Player_anim_tree::get_anim(ddp.normalize(), anim_type);

            if (anim_type.get_num() !=
                current_anim.get_num()) {  // у нас сменились анимации

                anim_type = current_anim;

                anim = player_anims[current_anim.get_num()];

                // начинаем с 1 кадра, чтобы мы сразу начинали движение
                anim.frame_cur_count =
                    std::min<unsigned int>(1, anim.frame_count - 1);
            }

            anim.frame_update(delta_time);
        }
    }

    void draw() const override {
        draw_sprite(pos + Dot(-7, 4), size, SP_MEDIUM_SHADOW);

        if (is_invulnerable()) {
            anim.draw(pos + delta_draw_pos, size, [&](const Color &color) {
                return Color(0xffffff, 128);
            });
        } else {
            anim.draw(pos + delta_draw_pos, size);
        }

        if (!is_jumped) {
            weapon.draw(pos);
        }

        draw_collision_obj(*this);
    }

    // возвращает направление анимации игрока
    [[nodiscard]] Dot get_dir() const {
        return get_direction(anim_type.dir);
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }
};

std::vector<Player> Players(1);

#endif  // GAME_PLAYER_HPP