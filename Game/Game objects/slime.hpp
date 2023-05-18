#ifndef GAME_SLIME_HPP
#define GAME_SLIME_HPP

#include <unordered_map>
#include <queue>
#include "../../render.hpp"
#include "abstract_game_object.hpp"
#include "effect.hpp"
#include "enemy_states.hpp"
#include "game_utils.hpp"
#include "player.hpp"


template <class T>
inline void hash_combine(std::size_t & s, const T & v)
{
    std::hash<T> h;
    s^= h(v) + 0x9e3779b9 + (s<< 6) + (s>> 2);
}

template <class T>
class MyHash;

template<class S>
struct MyHash
{
    std::size_t operator()(S const& s) const
    {
        std::size_t res = 0;
        hash_combine(res,s.x);
        hash_combine(res,s.y);
        return res;
    }
};


struct Slime : abstract_game_object, enemy_state_for_trivial_enemy {
    inline const static u8 draw_alpha = 210;
    inline const static efloat frame_duration = 1.0 / 7;
    inline const static animation
        animation_idle = animation(SS_SLIME, 0, 24, frame_duration),
        animation_attack = animation(SS_SLIME, 25, 30, frame_duration);

    int hp = 4;
    int shooting_radius = 100;
    bool shooter;
    efloat attack_accum;
    efloat paralyzed_accum;
    Weapon weapon;
    animation anim = animation_idle;

    bool is_attack = false;

    Slime(const Dot &position = Dot(), bool shtr = false) {
        // abstract_game_object
        pos = position;
        size = 0.8;
        collision_radius = 6;
        delta_draw_pos = Dot(-30, 38) * size;

        // enemy_state_for_trivial_enemy
        damage = 2;
        jump_radius = 14;
        ddp_speed = 250;
        paralyzed_accum = paralyzed_cooldown = 0.3;
        attack_accum = attack_cooldown = 3;
        shooter = shtr;
    }

    [[nodiscard]] bool is_invulnerable() const {
        return is_attack || paralyzed_accum < paralyzed_cooldown;
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }

    [[nodiscard]] collision_circle get_hitbox() const {
        return get_collision();
    }

    efloat len_between(Dot from, Dot to) {
        return (from - to).get_len();
    }
    efloat manhattan_between(Dot from, Dot to) {
        return abs(from.x - to.x) + abs(from.y - to.y);
    }

    void go_to_dot(Dot to, std::vector<collision_box>& Collision_boxes, efloat delta_time) {
        const efloat coef = 0.2;
//        std::unordered_map<Dot, std::vector<Dot>, MyHash<Dot>>graph;
        std::priority_queue<std::pair<efloat, Dot>, std::vector<std::pair<efloat, Dot>>, std::greater<>> q;

        const static std::vector<Dot>deltas = {{-1,0},{-1, 1},
                                        {0, 1}, {1, 1},
                                        {1, 0}, {1, -1},
                                        {0, -1}, {-1, -1}};
        std::unordered_map<Dot, efloat, MyHash<Dot>>dist;
        std::unordered_map<Dot, Dot, MyHash<Dot>>pred;
        q.emplace(len_between(pos, to)+ manhattan_between(pos, to) , pos);
        dist[pos] = 0;
        while (!q.empty()) {
            auto[prior, cur] = q.top();
            q.pop();
            if (manhattan_between(cur,to) <= 2) {
                pred[to] = cur;
                break;
            }
            for (auto& el: deltas) {
                auto neighbour = cur + el*coef;
                bool bad = false;
                for (auto& collision_box: Collision_boxes) {
                    if (collision_box.trigger(neighbour)) {
                        bad = true;
                        break;
                    }
                }
                if (!bad) {
                    auto cost = dist[cur] + manhattan_between(cur, neighbour);
                    if (!dist.count(neighbour) || dist[neighbour] >= cost) {
                        dist[neighbour] = cost;
                        pred[neighbour] = cur;
                        q.emplace(len_between(neighbour, to) + manhattan_between(neighbour, to), neighbour);
                    }
                }
            }
        }
        Dot current = to;
        while (pred[current] != pos) {
            current = pred[current];
        }
        move_to2d(
                pos, current, dp,
                (current - pos).normalize() * ddp_speed, delta_time
        );
    }

    void simulate(efloat delta_time, std::vector<collision_box>Collision_box) {
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

                Players[0].hp -= damage;

                Players[0].set_invulnerable();

                add_hit_effect(Players[0].pos);
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

//            move_to2d(
//                pos, Players[0].pos, dp,
//                (Players[0].pos - pos).normalize() * ddp_speed, delta_time
//            );
            go_to_dot(Players[0].pos, Collision_box, delta_time);

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
            if ((Players[0].pos - pos).get_len() <= shooting_radius) {
                weapon.shot(pos, Players[0].pos);
            }
        }
        if (shooter) {
            weapon.simulate(delta_time);
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
        if (shooter) {
            if ((Players[0].pos - pos).get_len() <= shooting_radius) {
                weapon.draw(pos, Players[0].pos);
            }
            else {
                weapon.draw(pos, pos + Dot(1, 0));
            }
        }

        draw_collision(*this);

        draw_hitbox(*this);

        draw_hp(*this);

        if (global_variables::show_locator) {
            draw_circle(
                Circle(pos - global_variables::camera.pos, jump_radius),
                Color(0xff0000, 50)
            );
        }
    }

};

std::vector<Slime> Slimes;

#endif  // GAME_SLIME_HPP