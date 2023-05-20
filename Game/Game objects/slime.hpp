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
        std::unordered_set<Dot, MyHash<Dot>>web;
        devour_cooldown = devour_accum = 5;
        shot_cooldown = shot_accum = 10;

        // чтобы разнообразить кучу слаймов, которые будут иметь одновременные
        // анимации
        anim.frame_cur_count = rnd() % 24;
    }

    [[nodiscard]] bool is_invulnerable() const {
        return is_devour || paralyzed_accum < paralyzed_cooldown;
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
    void bfs_to_dot(Dot from, Dot to, std::vector<collision_box>&Collision_boxes) {
        bfs_web.clear();

        const efloat coef = 2;
        const static std::vector<Dot>deltas = {{-1,0},{-1, 1},
                                               {0, 1}, {1, 1},
                                               {1, 0}, {1, -1},
                                               {0, -1}, {-1, -1}};
        std::queue<Dot>q;
        std::unordered_map<Dot, efloat, MyHash<Dot>>dist;
        std::unordered_map<Dot, Dot, MyHash<Dot>>pred;
        dist[from] = 0;
        q.push(from);
        while (!q.empty()) {
            Dot cur = q.front();
            q.pop();
            if (len_between(cur, to) <= 10) {
                pred[to] = cur;
                break;
            }
            for (auto& el: deltas) {
                auto neighbour = cur + el * coef;
                bool bad = false;
                for (auto collision_box: Collision_boxes) {
                    collision_box.p0 += Dot(-1, 1) * collision_radius;
                    collision_box.p1 += Dot(1, -1) * collision_radius;
                    if (collision_box.trigger(neighbour)) {
                        bad = true;
                        break;
                    }
                }
                if (!bad) {
                    if (!dist.count(neighbour)) {
                        dist[neighbour] = dist[cur] + coef;
                        pred[neighbour] = cur;
                        bfs_web.insert(neighbour);
                        q.push(neighbour);
                    }
                }
            }
        }
    }

    void go_to_dot(Dot from, Dot to, std::vector<collision_box>& Collision_boxes, efloat delta_time) {
//        web.clear();
        const efloat coef = 2;
        std::priority_queue<std::pair<efloat, Dot>, std::vector<std::pair<efloat, Dot>>, std::greater<>> q;

        const static std::vector<Dot>deltas = {{-1,0},{-1, 1},
                                               {0, 1}, {1, 1},
                                               {1, 0}, {1, -1},
                                               {0, -1}, {-1, -1}};
        std::unordered_map<Dot, efloat, MyHash<Dot>>dist;
        std::unordered_map<Dot, Dot, MyHash<Dot>>pred;
        q.emplace(len_between(from, to) , from);
        dist[from] = 0;
        int it = 0;
        while (!q.empty()) {
            auto[prior, cur] = q.top();
            q.pop();
            if (len_between(cur,to) <= 10) {
                pred[to] = cur;
                break;
            }
            for (auto& el: deltas) {
                auto neighbour = cur + el*coef;
                bool bad = false;
                for (auto collision_box: Collision_boxes) {
                    collision_box.p0 += Dot(-1, 1) * collision_radius;
                    collision_box.p1 += Dot(1, -1) * collision_radius;
                    if (collision_box.trigger(neighbour)) {
                        bad = true;

                        break;
                    }
                }
                if (!bad) {
                    auto cost = dist[cur] + coef;
                    if (!dist.count(neighbour) || dist[neighbour] > cost) {
//                        (this->web).insert(neighbour);
                        dist[neighbour] = cost;
                        pred[neighbour] = cur;
                        q.emplace(len_between(neighbour, to) + dist[neighbour], neighbour);
                    }
                }
            }
        }
        Dot current = to;
        std::vector<Dot>path;
        while (pred[current] != pos) {
            path.push_back(current);
            current = pred[current];
        }
        std::cout << std::endl;
        move_to2d(
                pos, current, dp,
                (current - pos).normalize() * ddp_speed, delta_time
        );
    }

    void simulate(efloat delta_time, std::vector<collision_box> Collision_box) {
        // мы парализованы и отлетаем от удара
        if (paralyzed_accum < paralyzed_cooldown) {
            paralyzed_accum += delta_time;
            shot_accum += delta_time;
            devour_accum += delta_time;
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
            if ((anim.frame_cur_count > 25 || !is_devour) &&
                Players[0].is_paralyzed) {
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
                Bullets.emplace_back(
                    ShooterType::ENEMY, bullet_pos, Players[0].pos, 1, 1000
                );
            }
        } else {
            devour_accum += delta_time;
            shot_accum += delta_time;

            anim.frame_update(delta_time);

            // чтобы слайм был поверх игрока, а не под ним
            Dot to = Players[0].pos - Dot(0, 1e-3);
//            move_to2d(
//                pos, to, dp, (to - pos).normalize() * ddp_speed, delta_time
//            );
            go_to_dot(pos, to, Collision_box, delta_time);
//            bfs_to_dot(pos, to, Collision_box);
            if (
                // игрока никто не ест
                !Players[0].is_paralyzed && shot_accum >= shot_cooldown
                // TODO: и если мы можем попасть по нему
            ) {
                anim = animation_shot;
                is_shooting = true;
            }
            else if (
                // игрока никто не ест
                !Players[0].is_paralyzed &&
                // игрок не прыгает
                !Players[0].is_jumped &&
                // игрок не неуязвим
                !Players[0].is_invulnerable() &&
                // мы близко к игроку
                (Players[0].pos - pos).get_len() <= jump_radius &&
                // перезарядка атаки прошла
                devour_accum >= devour_cooldown) {
                // игрок не может двигаться и у нас анимация атаки
                Players[0].is_paralyzed = is_devour = true;

                pos = Players[0].pos;  // прыгаем на игрока

                anim = animation_devour;
            }
        }
    }

    void draw() const override {
        if (is_devour) {
            if (is_between<u8>(9, anim.frame_count, 25)) {
                draw_sprite(
                    pos + delta_draw_pos, size, SP_SLIME_LARGE_SHADOW,
                    shadow_color_func()
                );
            } else {
                draw_sprite(
                    pos + delta_draw_pos, size, SP_SLIME_MEDIUM_SHADOW,
                    shadow_color_func()
                );
            }
            anim.draw(
                pos + delta_draw_pos, size, alpha_color_func<draw_alpha>()
            );
        } else {
            draw_sprite(
                pos + delta_draw_pos, size, SP_SLIME_MEDIUM_SHADOW,
                shadow_color_func()
            );

            anim.draw(pos + delta_draw_pos, size, [&](Color color) {
                return paralyzed_accum < paralyzed_cooldown
                           ? Color(0xffffff, 128)
                           : Color(color.operator unsigned int(), draw_alpha);
            });
        }

        draw_collision(*this);

        draw_hitbox(*this);

        draw_hp(*this);

//        for (auto& el: web) {
//            draw_rect(el-global_variables::camera.pos, Dot(0.5, 0.5), RED);
//        }
        for (auto& el: bfs_web) {
            draw_rect(el-global_variables::camera.pos, Dot(0.5, 0.5), BLUE);
        }

        if (global_variables::show_locator) {
            draw_circle(
                Circle(pos - global_variables::camera.pos, jump_radius),
                Color(0xff0000, 50)
            );
        }
    }

//    std::unordered_set<Dot, MyHash<Dot>> web;
   std::unordered_set<Dot, MyHash<Dot>> bfs_web;
};

std::vector<Slime> Slimes;

#endif  // GAME_SLIME_HPP
