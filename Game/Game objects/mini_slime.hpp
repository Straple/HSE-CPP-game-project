#ifndef GAME_SMALL_SLIME_HPP
#define GAME_SMALL_SLIME_HPP

#include "../../render.hpp"
#include "abstract_game_object.hpp"
#include "effect.hpp"
#include "enemy_states.hpp"
#include "game_utils.hpp"
#include "player.hpp"

struct MiniSlime : abstract_game_object, enemy_state_for_trivial_enemy {
    inline const static u8 draw_alpha = 210;
    inline const static efloat frame_duration = 1.0 / 7;
    inline const static animation
        animation_idle = animation(SS_SLIME, 0, 24, frame_duration),
        animation_attack = animation(SS_SLIME, 25, 30, frame_duration);

    int hp = 150;

    efloat attack_accum;
    efloat paralyzed_accum;

    animation anim = animation_idle;

    bool is_attack = false;

    MiniSlime(const Dot &position = Dot()) {
        // abstract_game_object
        pos = position;
        size = 0.8;
        collision_radius = 6;
        delta_draw_pos = Dot(-30, 38) * size;

        // enemy_state_for_trivial_enemy
        damage = 100;
        jump_radius = 14;
        ddp_speed = 250;
        paralyzed_accum = paralyzed_cooldown = 0.3;
        attack_accum = attack_cooldown = 3;
    }

    [[nodiscard]] bool is_invulnerable() const {
        return is_attack;
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }

    // for bullet hit
    [[nodiscard]] collision_circle get_body_collision() const {
        return get_collision();
    }

    void simulate(efloat delta_time) {
        // мы парализованы и отлетаем от удара
        if (paralyzed_accum < paralyzed_cooldown) {
            paralyzed_accum += delta_time;
            simulate_move2d(pos, dp, Dot(), delta_time);
        } else if (is_attack) {  // мы едим игрока

            // шарик лопнул
            if (anim.frame_cur_count > 25 && Players[0].is_paralyzed) {
                // выплюнуть игрока

                Players[0].dp = Circle(Dot(), 500).get_random_dot();

                Players[0].is_paralyzed = false;
                // Players[0].paralyzed_cooldown_acc = 0;

                Players[0].hp -= damage;

                add_hit_effect(Players[0].pos);

                // игрок неуязвим
                Players[0].set_invulnerable();
            }

            // анимация атаки закончилась
            if (anim.frame_update(delta_time)) {
                is_attack = false;
                anim = animation_idle;
                attack_accum = 0;
            }
        } else {
            attack_accum += delta_time;
            anim.frame_update(delta_time);

            move_to2d(
                pos, Players[0].pos, dp,
                (Players[0].pos - pos).normalize() * ddp_speed, delta_time
            );

            if (
                // игрока никто не ест
                !Players[0].is_paralyzed &&
                // игрок не прыгает
                !Players[0].is_jumped &&
                // игрок не неуязвим
                !Players[0].is_invulnerable() &&
                // мы близко к игроку
                (Players[0].pos - pos).get_len() <= jump_radius &&
                // перезарядка атаки прошла
                attack_accum >= attack_cooldown) {
                // игрок не может двигаться и у нас анимация атаки
                Players[0].is_paralyzed = is_attack = true;

                pos = Players[0].pos;  // прыгаем на игрока

                anim = animation_attack;
            }
        }
    }

    void draw() const override {
        if (is_attack) {
            if (is_between<u8>(9, anim.frame_count, 25)) {
                draw_sprite(
                    pos + delta_draw_pos, size, SP_SLIME_LARGE_SHADOW,
                    shadow_pixel_func
                );
            } else {
                draw_sprite(
                    pos + delta_draw_pos, size, SP_SLIME_MEDIUM_SHADOW,
                    shadow_pixel_func
                );
            }
            anim.draw(pos + delta_draw_pos, size, alpha_pixel_func<draw_alpha>);
        } else {
            draw_sprite(
                pos + delta_draw_pos, size, SP_SLIME_MEDIUM_SHADOW,
                shadow_pixel_func
            );

            anim.draw(pos + delta_draw_pos, size, [&](Color color) {
                return paralyzed_accum < paralyzed_cooldown
                           ? Color(0xffffff, 128)
                           : Color(color.operator unsigned int(), draw_alpha);
            });
        }

        draw_collision_obj(*this);

        if (global_variables::debug_mode) {
            Circle circle = get_body_collision().circle;
            circle.pos -= global_variables::camera.pos;
            draw_circle(circle, Color(0xff0000, 50));
        }

        draw_hp(*this);

        if (global_variables::show_locator) {
            draw_circle(
                Circle(pos - global_variables::camera.pos, jump_radius),
                Color(0xff0000, 50)
            );
        }
    }
};

std::vector<MiniSlime> MiniSlimes;

#endif  // GAME_SMALL_SLIME_HPP