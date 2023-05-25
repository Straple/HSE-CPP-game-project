#ifndef GAME_BAT_HPP
#define GAME_BAT_HPP

#include "../../render.hpp"
#include "abstract_game_object.hpp"
#include "effect.hpp"
#include "enemy_states.hpp"
#include "game_utils.hpp"
#include "player.hpp"

struct Bat : abstract_game_object, enemy_state_for_trivial_enemy {
    ADD_BYTE_SERIALIZATION();

    int hp = 3;

    efloat attack_accum;
    efloat paralyzed_accum;

    animation anim = animation(SS_BAT, 0, 5, 1.0 / 7);

    // TODO: вот это
    // наша цель
    // int target_client_id = -1;

    Bat(const Dot &position = Dot()) {
        // abstract_game_object
        pos = position;
        size = 0.8;
        collision_radius = 7;
        delta_draw_pos = Dot(-8, 26) * size;

        // enemy_state_for_trivial_enemy
        damage = 1;
        jump_radius = 8;
        ddp_speed = 600;
        paralyzed_accum = paralyzed_cooldown = 0.3;
        attack_accum = attack_cooldown = 3;

        // чтобы разнообразить кучу летучих мышей, которые будут иметь
        // одновременные анимации
        anim.frame_cur_count = get_random_engine()() % 5;
    }

    [[nodiscard]] bool is_invulnerable() const {
        return paralyzed_accum < paralyzed_cooldown;
    }

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override {
        return std::make_unique<CollisionCircle>(pos, collision_radius);
    }

    [[nodiscard]] std::unique_ptr<Collision> get_hitbox() const {
        return std::make_unique<CollisionCircle>(pos + Dot(0, 13), 5);
    }

    void simulate(efloat delta_time) {
        attack_accum += delta_time;
        paralyzed_accum += delta_time;

        if (paralyzed_accum < paralyzed_cooldown) {
            simulate_move2d(pos, dp, Dot(), delta_time);
        } else {
            anim.frame_update(delta_time);

            // чтобы летучая мышь не дергалась туда-сюда
            if (dp.x < -30) {
                anim.sprite_sheet = SS_BAT_INVERSE;
            } else if (dp.x > 30) {
                anim.sprite_sheet = SS_BAT;
            }

            int target_client_id = find_best_player(pos);
            int index = find_player_index(target_client_id);
            if(index == -1){
                return; // нет игроков
            }

            auto &player = Players[index];

            // чтобы летучая мышь была поверх игрока, а не под ним
            Dot to = player.pos - Dot(0, 0.1);
            simulate_move_to2d(pos, to, dp, (to - pos).normalize() * ddp_speed, delta_time);

            if (!player.is_invulnerable() && !player.is_jumped &&
                (player.pos - pos).get_len() <= jump_radius &&
                attack_accum >= attack_cooldown) {
                // hit
                attack_accum = 0;
                pos = player.pos;  // прыгаем на игрока
                player.hp -= damage;
                player.set_invulnerable();
                add_hit_effect(player.pos);
            }
        }
    }

    void draw() const override {
        draw_sprite(pos + Dot(-5, 0) * size, size, SP_SMALL_SHADOW);

        anim.draw(pos + delta_draw_pos, size, [&](const Color &color) {
            return paralyzed_accum < paralyzed_cooldown ? Color(0xffffff, 128) : color;
        });

        draw_collision(*this);
        draw_hitbox(*this);
        draw_hp(*this);
    }
};

std::vector<Bat> Bats;

#endif  // GAME_BAT_HPP