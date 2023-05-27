#ifndef GAME_SLIME_HPP
#define GAME_SLIME_HPP

#include "../../render.hpp"
#include "abstract_game_object.hpp"
#include "effect.hpp"
#include "enemy_states.hpp"
#include "game_utils.hpp"
#include "player.hpp"

struct Slime : abstract_game_object, enemy_state_for_trivial_enemy {
    ADD_BYTE_SERIALIZATION();

    inline const static u8 draw_alpha = 210;
    inline const static efloat frame_duration = 1.0 / 7;
    inline const static animation animation_idle = animation(SS_SLIME, 0, 24, frame_duration),
                                  animation_devour = animation(SS_SLIME, 25, 30, frame_duration),
                                  animation_shot = animation(SS_SLIME, 55, 13, frame_duration);

    int hp = 4;
    efloat devour_accum, devour_cooldown;
    efloat shot_accum, shot_cooldown;
    efloat paralyzed_accum;
    animation anim = animation_idle;

    bool is_devour = false;
    bool is_shooting = false;

    Dot move_dir_to_target;
    efloat time_for_update_move_dir = 0;

    int target_client_id = -1;

    Slime(const Dot &position = Dot()) {
        // abstract_game_object
        pos = position;
        size = 0.8;
        collision_radius = 6;
        delta_draw_pos = Dot(-30, 38) * size;

        // enemy_state_for_trivial_enemy
        damage = 2;
        jump_radius = 14;
        ddp_speed = 400;
        paralyzed_accum = paralyzed_cooldown = 0.3;
        // attack_cooldown = 3;
        devour_cooldown = devour_accum = 5;
        shot_cooldown = shot_accum = 10;

        // чтобы разнообразить кучу слаймов, которые будут иметь одновременные
        // анимации
        anim.frame_cur_count = get_random_engine()() % 24;
    }

    [[nodiscard]] bool is_invulnerable() const {
        return is_devour || paralyzed_accum < paralyzed_cooldown;
    }

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override {
        return std::make_unique<CollisionCircle>(pos, collision_radius);
    }

    [[nodiscard]] std::unique_ptr<Collision> get_hitbox() const {
        return get_collision();
    }

    void simulate_move_to_player(Player &player, const std::set<grid_pos_t> &visitable_grid_dots) {
        // чтобы слайм был поверх игрока, а не под ним
        Dot to = player.pos - Dot(0, 0.1);

        if (!get_direction_to_shortest_path(
                pos, to, move_dir_to_target,
                [&](const grid_pos_t &request) { return visitable_grid_dots.count(request); },
                [&](const Dot &request) { return (to - request).get_len() < 10; }
            )) {
            // ASSERT(false, "oh ho, way not found");
        }
    }

    void simulate(const efloat delta_time, const std::set<grid_pos_t> &visitable_grid_dots) {
        paralyzed_accum += delta_time;
        shot_accum += delta_time;
        devour_accum += delta_time;

        // мы парализованы и отлетаем от удара
        if (paralyzed_accum < paralyzed_cooldown) {
            simulate_move2d(pos, dp, Dot(), delta_time);
        } else if (is_devour) {  // мы едим игрока
            // анимация атаки закончилась
            if (anim.frame_update(delta_time)) {
                is_devour = false;
                anim = animation_idle;
                devour_accum = 0;

                // так как мы ели игрока, то почему бы нам не получить доп хп
                hp += 2;
            }

            int index = find_player_index(target_client_id);
            auto &player = Players[index];

            // шарик лопнул
            if ((anim.frame_cur_count > 25 || !is_devour) && player.is_paralyzed) {
                // выплюнуть игрока
                player.dp = Circle(Dot(), 500).get_random_dot();
                player.is_paralyzed = false;
                player.hp -= damage;
                player.set_invulnerable();
                add_hit_effect(player.pos);
            }
        } else if (is_shooting) {
            if (anim.frame_update(delta_time)) {
                shot_accum = 0;
                is_shooting = false;
                anim = animation_idle;
                // чтобы разнообразить кучу слаймов, которые будут иметь одновременные
                // анимации
                anim.frame_cur_count = get_random_engine()() % 24;

                Dot bullet_pos = pos + Dot(0, 15);
                Bullets.emplace_back(
                    ShooterType::ENEMY, bullet_pos, Players[find_player_index(target_client_id)].pos, 1, 1000
                );
            }
        } else {
            anim.frame_update(delta_time);

            target_client_id = find_best_player(pos);
            int index = find_player_index(target_client_id);
            if (index == -1) {
                return;  // нет игроков
            }
            auto &player = Players[index];

            time_for_update_move_dir -= delta_time;
            if (time_for_update_move_dir < 0) {
                if (randomness(30)) {
                    time_for_update_move_dir = 0.3;
                } else if (randomness(50)) {
                    time_for_update_move_dir = 0.2;
                } else {
                    time_for_update_move_dir = 0.1;
                }
                simulate_move_to_player(player, visitable_grid_dots);
            }
            // move_dir уже нормализован в get_direction_to_shortest_path
            simulate_move_to2d(pos, pos + move_dir_to_target, dp, move_dir_to_target * ddp_speed, delta_time);

            if (
                // игрока никто не ест
                !player.is_paralyzed && shot_accum >= shot_cooldown
            ) {
                anim = animation_shot;
                is_shooting = true;
            } else if (
                // игрока никто не ест
                    !player.is_paralyzed &&
                    // игрок не прыгает
                    !player.is_jumped &&
                    // игрок не неуязвим
                    !player.is_invulnerable() &&
                    // мы близко к игроку
                    (player.pos - pos).get_len() <= jump_radius &&
                    // перезарядка атаки прошла
                    devour_accum >= devour_cooldown
                    ) {
                // игрок не может двигаться и у нас анимация атаки
                player.is_paralyzed = is_devour = true;

                pos = player.pos;  // прыгаем на игрока

                anim = animation_devour;
            }
        }
    }

    void draw() const override {
        if (is_devour) {
            if (is_between<u8>(9, anim.frame_count, 25)) {
                draw_sprite(pos + delta_draw_pos, size, SP_SLIME_LARGE_SHADOW, shadow_color_func());
            } else {
                draw_sprite(pos + delta_draw_pos, size, SP_SLIME_MEDIUM_SHADOW, shadow_color_func());
            }
            anim.draw(pos + delta_draw_pos, size, alpha_color_func<draw_alpha>());
        } else {
            draw_sprite(pos + delta_draw_pos, size, SP_SLIME_MEDIUM_SHADOW, shadow_color_func());

            anim.draw(pos + delta_draw_pos, size, [&](Color color) {
                return paralyzed_accum < paralyzed_cooldown ? Color(0xffffff, 128)
                                                            : Color(color.operator unsigned int(), draw_alpha);
            });
        }

        draw_collision(*this);
        draw_hitbox(*this);
        draw_hp(*this);

        if (global_variables::show_locator) {
            draw_circle(Circle(pos - global_variables::camera.pos, jump_radius), Color(0xff0000, 50));
        }
    }
};

std::vector<Slime> Slimes;

#endif  // GAME_SLIME_HPP