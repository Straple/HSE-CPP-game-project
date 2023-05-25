#ifndef GAME_SLIME_HPP
#define GAME_SLIME_HPP

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "../../render.hpp"
#include "abstract_game_object.hpp"
#include "effect.hpp"
#include "enemy_states.hpp"
#include "game_utils.hpp"
#include "player.hpp"

template <class T>
inline void hash_combine(std::size_t &s, const T &v) {
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

template <class T>
class MyHash;

template <class S>
struct MyHash {
    std::size_t operator()(const S &s) const {
        std::size_t res = 0;
        hash_combine(res, s.x);
        hash_combine(res, s.y);
        return res;
    }
};

struct Slime : abstract_game_object, enemy_state_for_trivial_enemy {
    inline const static u8 draw_alpha = 210;
    inline const static efloat frame_duration = 1.0 / 7;
    inline const static animation
            animation_idle = animation(SS_SLIME, 0, 24, frame_duration),
            animation_devour = animation(SS_SLIME, 25, 30, frame_duration),
            animation_shot = animation(SS_SLIME, 55, 13, frame_duration);

    int hp = 4;
    efloat devour_accum, devour_cooldown;
    efloat shot_accum, shot_cooldown;
    efloat paralyzed_accum;
    animation anim = animation_idle;

    bool is_devour = false;
    bool is_shooting = false;

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
        std::unordered_set<Dot, MyHash<Dot>> web;
        devour_cooldown = devour_accum = 5;
        shot_cooldown = shot_accum = 10;

        // чтобы разнообразить кучу слаймов, которые будут иметь одновременные
        // анимации
        anim.frame_cur_count = rnd() % 24;
    }

    [[nodiscard]] bool is_invulnerable() const {
        return is_devour || paralyzed_accum < paralyzed_cooldown;
    }

    [[nodiscard]] std::unique_ptr<AbstractCollision> get_collision() const override {
        return std::make_unique<CollisionCircle>(pos, collision_radius);
    }

    [[nodiscard]] std::unique_ptr<AbstractCollision> get_hitbox() const {
        return get_collision();
    }

    void simulate(efloat delta_time, const std::vector<CollisionBox> &Collision_boxes) {
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

            // шарик лопнул
            if ((anim.frame_cur_count > 25 || !is_devour) && Players[0].is_paralyzed) {
                // выплюнуть игрока
                Players[0].dp = Circle(Dot(), 500).get_random_dot();
                Players[0].is_paralyzed = false;
                Players[0].hp -= damage;
                Players[0].set_invulnerable();
                add_hit_effect(Players[0].pos);
            }
        } else if (is_shooting) {
            if (anim.frame_update(delta_time)) {
                shot_accum = 0;
                is_shooting = false;
                anim = animation_idle;

                Dot bullet_pos = pos + Dot(0, 15);
                Bullets.emplace_back(ShooterType::ENEMY, bullet_pos, Players[0].pos, 1, 1000);
            }
        } else {
            anim.frame_update(delta_time);

            // чтобы слайм был поверх игрока, а не под ним
            Dot to = Players[0].pos - Dot(0, 0.1);
            // move_to2d(pos, to, dp, (to - pos).normalize() * ddp_speed, delta_time);
            Dot move_dir;
            if (!get_direction_to_shortest_path_Astar(
                    pos, to, move_dir,
                    [&](const Dot &request) {
                        for (const auto &collision_box : Collision_boxes) {
                            Dot save_pos = pos;  // чтобы мы точно взяли коллизию слайма
                            pos = request;
                            bool trigger = collision_box.trigger(*get_collision());
                            pos = save_pos;

                            if (trigger) {
                                return false;
                            }
                        }
                        return true;
                    },
                    [&](const Dot &request) {
                        return (to - request).get_len() < 10;
                    },
                    shortest_path, grid
            )) {
                // ASSERT(false, "oh ho, way not found");
            }
            // move_dir уже нормализован в get_direction_to_shortest_path
            move_to2d(pos, pos + move_dir, dp, move_dir.normalize() * ddp_speed, delta_time);

            /*if (
                // игрока никто не ест
                !Players[0].is_paralyzed && shot_accum >= shot_cooldown
                // TODO: и если мы можем попасть по нему
            ) {
                anim = animation_shot;
                is_shooting = true;
            } else if (
                // игрока никто не ест
                !Players[0].is_paralyzed &&
                // игрок не прыгает
                !Players[0].is_jumped &&
                // игрок не неуязвим
                !Players[0].is_invulnerable() &&
                // мы близко к игроку
                (Players[0].pos - pos).get_len() <= jump_radius &&
                // перезарядка атаки прошла
                devour_accum >= devour_cooldown
            ) {
                // игрок не может двигаться и у нас анимация атаки
                Players[0].is_paralyzed = is_devour = true;

                pos = Players[0].pos;  // прыгаем на игрока

                anim = animation_devour;
            }*/
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
                return paralyzed_accum < paralyzed_cooldown ? Color(0xffffff, 128) : Color(color.operator unsigned int(), draw_alpha);
            });
        }

        draw_collision(*this);
        draw_hitbox(*this);
        draw_hp(*this);
        {
            for (const auto &pos : grid) {
                draw_rect(pos - global_variables::camera.pos, Dot(0.5, 0.5), BLUE);
            }

            int i = 0;
            for (const auto &pos : shortest_path) {
                draw_rect(pos - global_variables::camera.pos, Dot(0.5, 0.5), GREEN);
                draw_object(i, pos - global_variables::camera.pos, 0.3, RED);
                i++;
            }

            /*for(int i = 0; i < grid.size(); i++){
                for(int j = i + 1; j < grid.size(); j++){
                    if((grid[i] - grid[j]).get_len() < 1.5){
                        //ASSERT(false, to_string(grid[i]) + " " + to_string(grid[j]));
                        ASSERT(false, to_string(grid[i].x - grid[j].x) + " " + to_string(grid[i].y - grid[j].y));
                    }
                }
            }*/

            /*static bool write = false;
            if(!write){
                write = true;
                std::ofstream output("path_log.txt");
                for (const auto &pos : grid) {
                    output << pos << '\n';
                }
            }*/
        }

        if (global_variables::show_locator) {
            draw_circle(
                    Circle(pos - global_variables::camera.pos, jump_radius),
                    Color(0xff0000, 50)
            );
        }
    }

    std::vector<Dot> shortest_path;
    std::vector<Dot> grid;
};

std::vector<Slime> Slimes;

#endif  // GAME_SLIME_HPP
