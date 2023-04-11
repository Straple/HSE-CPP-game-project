#ifndef GAME_BAT_HPP
#define GAME_BAT_HPP

#include "../../render.hpp"
#include "abstract_game_object.hpp"
#include "effect.hpp"
#include "enemy_states.hpp"
#include "game_utils.hpp"
#include "player.hpp"

struct Bat : abstract_game_object, enemy_state_for_trivial_enemy {
    int hp = 100;

    efloat attack_accum;
    efloat paralyzed_accum;

    animation anim = animation(SS_BAT, 0, 5, 1.0 / 7);

    Bat(const Dot &position = Dot()) {
        // abstract_game_object
        pos = position;
        size = 0.8;
        collision_radius = 7;
        delta_draw_pos = Dot(-8, 26) * size;

        // enemy_state_for_trivial_enemy
        damage = 50;
        jump_radius = 8;
        ddp_speed = 400;
        paralyzed_accum = paralyzed_cooldown = 0.3;
        attack_accum = attack_cooldown = 3;
    }

    [[nodiscard]] bool is_invulnerable() const {
        return paralyzed_accum < paralyzed_cooldown;
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }

    [[nodiscard]] collision_circle get_hitbox() const {
        return collision_circle(Circle(pos + Dot(0, 13), 5));
    }

    void simulate(efloat delta_time) {
        attack_accum += delta_time;
        paralyzed_accum += delta_time;

        if (paralyzed_accum < paralyzed_cooldown) {
            simulate_move2d(pos, dp, Dot(), delta_time);
        } else {

            anim.frame_update(delta_time);

            if (dp.x < 0) {
                anim.sprite_sheet = SS_BAT_INVERSE;
            } else {
                anim.sprite_sheet = SS_BAT;
            }

            move_to2d(
                pos, Players[0].pos, dp,
                (Players[0].pos - pos).normalize() * ddp_speed, delta_time
            );

            if (!Players[0].is_invulnerable() && !Players[0].is_jumped &&
                (Players[0].pos - pos).get_len() <= jump_radius &&
                attack_accum >= attack_cooldown) {
                // hit

                attack_accum = 0;

                pos = Players[0].pos;  // прыгаем на игрока

                Players[0].hp -= damage;

                Players[0].set_invulnerable();

                add_hit_effect(Players[0].pos);
            }
        }
    }

    void draw() const override {
        draw_sprite(pos + Dot(-5, 0) * size, size, SP_SMALL_SHADOW);

        anim.draw(pos + delta_draw_pos, size, [&](const Color &color) {
            return paralyzed_accum < paralyzed_cooldown ? Color(0xffffff, 128)
                                                        : color;
        });

        draw_collision(*this);

        draw_hitbox(*this);

        draw_hp(*this);

        if (global_variables::show_locator) {
            draw_circle(
                Circle(pos - global_variables::camera.pos, jump_radius),
                Color(0xff0000, 16)
            );
        }
    }
};

std::vector<Bat> Bats;

#endif  // GAME_BAT_HPP