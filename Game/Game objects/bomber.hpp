#ifndef BULLETPROOF_BOMBER_HPP
#define BULLETPROOF_BOMBER_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"
#include "effect.hpp"
#include "game_utils.hpp"
#include "mob.hpp"
#include "player.hpp"

struct Bomber : Mob {
    ADD_BYTE_SERIALIZATION();

    // visible
    inline const static efloat size = 0.8;
    inline const static Dot delta_draw_pos = Dot(-30, 38) * size;
    inline const static u8 draw_alpha = 210;
    inline const static efloat frame_duration = 1.0 / 7;
    inline const static animation animation_idle = animation(SS_SLIME, 0, 24, frame_duration),
                                  animation_boom = animation(SS_BOOM, 0, 18, frame_duration / 2);
    // physics
    inline const static efloat collision_radius = 6;
    inline const static efloat ddp_speed = 400;
    // others
    inline const static efloat jump_radius = 14;
    inline const static efloat boom_radius = 40;
    inline const static int boom_damage = 2;

    int hp = 2;

    animation anim = animation_idle;

    bool is_booming = false;

    Bomber(const Dot &position = Dot()) {
        pos = position;
        // чтобы разнообразить кучу мобов, которые будут иметь одновременные
        // анимации
        anim.frame_cur_count = get_random_engine()() % 24;
    }

    [[nodiscard]] bool is_invulnerable() const {
        return paralyzed_accum > 0;
    }

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override {
        return std::make_unique<CollisionCircle>(pos, collision_radius);
    }

    [[nodiscard]] std::unique_ptr<Collision> get_hitbox() const {
        return get_collision();
    }

    void simulate(const efloat delta_time, const std::set<grid_pos_t> &visitable_grid_dots) {
        paralyzed_accum -= delta_time;

        // мы парализованы и отлетаем от удара
        if (paralyzed_accum > 0) {
            simulate_move2d(pos, dp, Dot(), delta_time);
        } else if (is_booming) {
            if (anim.frame_update(delta_time)) {
                is_booming = false;
                hp = -1;  // его нужно удалить
            }
        } else {
            anim.frame_update(delta_time);

            update_target(delta_time);

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
                (player.pos - pos).get_len() <= jump_radius
                ) {
                do_boom();
            }
        }
    }

    void do_boom() {
        is_booming = true;
        anim = animation_boom;
        for (auto &player : Players) {
            if ((player.pos - pos).get_len() < boom_radius) {
                player.hp -= boom_damage;
                player.dp += (player.pos - pos).normalize() * 600;
                player.set_invulnerable();
                if (player.hp <= 0) {
                    player.die();
                }
            }
        }
    }

    void draw() const override {
        if (is_booming) {
            anim.draw(pos + delta_draw_pos + Dot(-26, 23), size);
        } else {
            draw_sprite(pos + delta_draw_pos, size, SP_SLIME_MEDIUM_SHADOW, shadow_color_func());

            anim.draw(pos + delta_draw_pos, size, [&](Color color) {
                return paralyzed_accum > 0 ? Color(0xffffff, 128) : Color(static_cast<uint32_t>(color), draw_alpha);
            });
        }
        draw_rect(pos - global_variables::camera.pos + Dot(0, 15), Dot(0.5, 0.5), GREEN);
        draw_collision(*this);
        draw_hitbox(*this);
        draw_hp(*this);
    }
};

std::vector<Bomber> Bombers;

#endif  // BULLETPROOF_BOMBER_HPP