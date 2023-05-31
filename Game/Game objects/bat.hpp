#ifndef GAME_BAT_HPP
#define GAME_BAT_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"
#include "effect.hpp"
#include "game_utils.hpp"
#include "mob.hpp"
#include "player.hpp"

struct Bat : Mob {
    ADD_BYTE_SERIALIZATION();

    // visible
    inline const static efloat size = 0.8;
    inline const static Dot delta_draw_pos = Dot(-8, 26) * size;
    // physics
    inline const static efloat collision_radius = 7;
    inline const static efloat jump_radius = 8;
    inline const static efloat ddp_speed = 600;
    // cooldowns
    inline const static efloat attack_cooldown = 4;
    // others
    inline const static int damage = 1;

    int hp = 3;

    efloat attack_accum = 0;

    animation anim = animation(SS_BAT, 0, 5, 1.0 / 7);

    Bat(const Dot &position = Dot()) {
        pos = position;
        // чтобы разнообразить кучу летучих мышей, которые будут иметь
        // одновременные анимации
        anim.frame_cur_count = get_random_engine()() % 5;
    }

    [[nodiscard]] bool is_invulnerable() const {
        return paralyzed_accum > 0;
    }

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override {
        return std::make_unique<CollisionCircle>(pos, collision_radius);
    }

    [[nodiscard]] std::unique_ptr<Collision> get_hitbox() const {
        return std::make_unique<CollisionCircle>(pos + Dot(0, 13), 5);
    }

    void simulate(efloat delta_time, const std::set<grid_pos_t> &visitable_grid_dots) {
        attack_accum -= delta_time;
        paralyzed_accum -= delta_time;
        target_change_accum -= delta_time;

        if (paralyzed_accum > 0) {
            simulate_move2d(pos, dp, Dot(), delta_time);
        } else {
            anim.frame_update(delta_time);

            // чтобы летучая мышь не дергалась туда-сюда
            if (dp.x < -15) {
                anim.sprite_sheet = SS_BAT_INVERSE;
            } else if (dp.x > 15) {
                anim.sprite_sheet = SS_BAT;
            }

            update_target();

            int index = find_player_index(target_client_id);
            if (index == -1) {
                return;  // нет игроков
            }

            auto &player = Players[index];

            update_move_dir(delta_time, player.pos, visitable_grid_dots);
            // move_dir уже нормализован в get_direction_to_shortest_path
            simulate_move_to2d(pos, pos + move_dir_to_target, dp, move_dir_to_target * ddp_speed, delta_time);

            if (
                // игрока никто не ест
                !player.is_paralyzed &&
                // игрок не прыгает
                !player.is_jumped &&
                // игрок не неуязвим
                !player.is_invulnerable() &&
                // мы близко к игроку
                (player.pos - pos).get_len() <= jump_radius &&
                // перезарядка атаки прошла
                attack_accum <= 0) {
                // hit
                attack_accum = attack_cooldown;
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
            return paralyzed_accum > 0 ? Color(0xffffff, 128) : color;
        });

        draw_collision(*this);
        draw_hitbox(*this);
        draw_hp(*this);
    }
};

std::vector<Bat> Bats;

#endif  // GAME_BAT_HPP