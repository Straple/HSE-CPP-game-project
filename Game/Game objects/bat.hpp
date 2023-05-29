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

    Dot move_dir_to_target;
    efloat time_for_update_move_dir = 0;

    efloat target_change_accum = 5;
    int target_client_id = -1;

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

    void simulate_move_to_player(Player &player, const std::set<grid_pos_t> &visitable_grid_dots) {
        // чтобы летучая мышь была поверх игрока, а не под ним
        Dot to = player.pos - Dot(0, 0.1);

        if (!get_direction_to_shortest_path(
                pos, to, move_dir_to_target,
                [&](const grid_pos_t &request) { return visitable_grid_dots.count(request); },
                [&](const Dot &request) { return (to - request).get_len() < 10; }
            )) {
            // ASSERT(false, "oh ho, way not found");
        }
    }

    void simulate(efloat delta_time, const std::set<grid_pos_t> &visitable_grid_dots) {
        attack_accum += delta_time;
        paralyzed_accum += delta_time;
        target_change_accum += delta_time;

        if (paralyzed_accum < paralyzed_cooldown) {
            simulate_move2d(pos, dp, Dot(), delta_time);
        } else {
            anim.frame_update(delta_time);

            // чтобы летучая мышь не дергалась туда-сюда
            if (dp.x < -15) {
                anim.sprite_sheet = SS_BAT_INVERSE;
            } else if (dp.x > 15) {
                anim.sprite_sheet = SS_BAT;
            }

            // если у нас нет цели, то найдем ее
            if (find_player_index(target_client_id) == -1 || Players[find_player_index(target_client_id)].is_dead||
                // или мы уже долго гонялись за ним. может есть кто лучше?
                target_change_accum > 5) {
                target_client_id = find_best_player(pos);
                target_change_accum = 0;
            }

            int index = find_player_index(target_client_id);
            if (index == -1) {
                return;  // нет игроков
            }

            auto &player = Players[index];

            time_for_update_move_dir -= delta_time;
            if (time_for_update_move_dir < 0) {
                if (randomness(30)) {
                    time_for_update_move_dir = 0.35;
                } else if (randomness(50)) {
                    time_for_update_move_dir = 0.2;
                } else {
                    time_for_update_move_dir = 0.15;
                }
                simulate_move_to_player(player, visitable_grid_dots);
            }
            // move_dir уже нормализован в get_direction_to_shortest_path
            simulate_move_to2d(pos, pos + move_dir_to_target, dp, move_dir_to_target * ddp_speed, delta_time);

            if (!player.is_paralyzed && !player.is_invulnerable() && !player.is_jumped &&
                (player.pos - pos).get_len() <= jump_radius && attack_accum >= attack_cooldown) {
                // hit
                attack_accum = 0;
                pos = player.pos;  // прыгаем на игрока
                player.hp -= damage;
                player.set_invulnerable();
                add_hit_effect(player.pos);
                if (player.hp <= 0) {
                    player.die();
                }
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