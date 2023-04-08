#ifndef GAME_BULLET_HPP
#define GAME_BULLET_HPP

#include "abstract_game_object.hpp"
#include "render.hpp"

struct Bullet : abstract_game_object {
    // Добавим позже поле формы пули, чтобы можно было
    // стрелять как кругами, так и прямоугольниками и ромбами.
    // Возможно, добавим лучевое оружие

    Dot pos;
    Dot dp;

    int speed;
    Dot dir;  // направление полета пули

    int damage;

    // bool ricochet;  // TODO

    Bullet(Dot from, Dot to, int damage, int speed)
        : dir((to - from).normalize()), damage(damage), speed(speed) {
        pos = from;
        collision_radius = 2;
        delta_draw_pos = Dot(-10, 10);
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }

    // не пон че делает
    /*direction_t get_closest_dir() const {
        const std::vector<Dot> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
        std::vector<direction_t> dir_enum = {
            direction_t::RIGHT, direction_t::UP, direction_t::LEFT,
            direction_t::DOWN};
        efloat angle_cos = -2;
        direction_t closest_one;
        for (int i = 0; i < 4; i++) {
            efloat cos = get_angle(directions[i], shoot_to);
            if (abs(cos) > angle_cos) {
                angle_cos = cos;
                closest_one = dir_enum[i];
            }
        }
        return closest_one;
    }*/

    // тоже не пон
    /*[[nodiscard]] collision_box get_attack_collision() const {
        collision_box box;
        switch (get_closest_dir()) {
            case direction_t::RIGHT: {
                box = collision_box(pos + Dot(0, 14), pos + Dot(15, -4));
            } break;
            case direction_t::LEFT: {
                box = collision_box(pos + Dot(-15, 14), pos + Dot(0, -4));
            } break;
            case direction_t::UP: {
                box = collision_box(pos + Dot(-9, 20), pos + Dot(9, 0));
            } break;
            case direction_t::DOWN: {
                box = collision_box(pos + Dot(-9, 3), pos + Dot(9, -14));
            } break;
            case direction_t::NONE:
            case direction_t::COUNT:
                ASSERT(false, "what the collision_box needed?");
        }
        return box;
    }*/

    [[nodiscard]] bool attack_trigger(const Dot &p) const {
        return get_collision().trigger(p);
    }

    /*template <typename enemy_type>
    void simulate_hit(enemy_type enemy, Player &player) {
        enemy.add_hit_effect(pos + Dot(0, 10) * gobj_state.size);

        enemy.hp -= player.damage;

        if (enemy.hp <= 0) {
            restart();
            player.exp += gobj_state.exp_cnt;

            if (player.exp >= 10) {
                player.max_hp += 15;
                player.damage += 2;
                player.exp -= 10;
                player.lvl++;
            }
        } else {
            dp += player.get_dir() * 50;
        }
    }
    */
    // вернет правду, если атака кого-то зацепила
    template <typename container_t>
    bool simulate_attack(Player &player, container_t &Enemies) {
        bool is_attack = false;

        for (int i = 0; i < Enemies.size(); i++) {
            if (attack_trigger(Enemies[i].pos) &&
                reinterpret_cast<char *>(&Enemies[i]) !=
                    reinterpret_cast<char *>(this)) {
                is_attack = true;

                Enemies[i].simulate_hit(player);

                if (Enemies[i].hp <= 0 &&
                    typeid(Enemies[i]) != typeid(Player)) {
                    //                    player.exp +=
                    //                    Enemies[i].gobj_state.exp_cnt;
                    Enemies.erase(Enemies.begin() + i);
                    i--;
                    if (player.exp >= 10) {
                        player.max_hp += 15;
                        player.damage += 2;
                        player.exp -= 10;
                        player.lvl++;
                    }
                }
            }
        }

        return is_attack;
    }

    void simulate(efloat delta_time) {
        Dot ddp = dir * speed;
        simulate_move2d(pos, dp, ddp, delta_time);
        // move_to2d(
        //     pos, pos + shoot_to.normalize() * 10000, dp, Dot(0, -1) * 1000,
        //     delta_time
        //);
    }

    void draw() const {
        //        draw_sprite(pos + BULLET_DELTA_DRAW_DOT*0.25, 0.3, SP_COIN);
        draw_collision_obj(*this);
        //        draw_rect(pos - camera.pos, Dot(1, 1) * 0.3, RED);
        auto circ = get_collision().circle;
        static_pos_update(circ.pos, global_variables::camera_is_static);
        circ.radius = 1.5;
        draw_circle(circ, Color(0xbd9919, 255));
        circ.radius = 1;
        draw_circle(circ, Color(0xfcca12, 255));
    }
};

#endif