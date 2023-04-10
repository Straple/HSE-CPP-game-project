#ifndef GAME_BULLET_HPP
#define GAME_BULLET_HPP

#include "abstract_game_object.hpp"
#include "bat.hpp"
#include "player.hpp"
#include "render.hpp"
#include "slime.hpp"

struct Bullet : abstract_game_object {
    // Добавим позже поле формы пули, чтобы можно было
    // стрелять как кругами, так и прямоугольниками и ромбами.
    // Возможно, добавим лучевое оружие

    int speed;
    Dot dir;  // направление полета пули

    int damage;

    Bullet(Dot from, Dot to, int damage, int speed)
        : dir((to - from).normalize()), damage(damage), speed(speed) {
        pos = from;
        collision_radius = 2;
        delta_draw_pos = Dot(-10, 10);
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }

    // вернет правду, если атака кого-то зацепила
    template <typename enemy_t>
    bool simulate_attack(Player &player, std::vector<enemy_t> &Enemies) {
        for (int i = 0; i < Enemies.size(); i++) {
            if (get_collision().trigger(Enemies[i].get_body_collision()) &&
                reinterpret_cast<char *>(&Enemies[i]) !=
                    reinterpret_cast<char *>(this) &&
                !Enemies[i].is_invulnerable()) {
                // simulate hit
                {
                    add_hit_effect(pos);

                    Enemies[i].hp -= damage;

                    Enemies[i].dp += dir * speed / 10;
                    Enemies[i].paralyzed_accum = 0;
                }

                if (Enemies[i].hp <= 0) {
                    add_death_effect(Enemies[i].get_body_collision().circle.pos
                    );

                    Enemies.erase(Enemies.begin() + i);
                    i--;
                }
                return true;
            }
        }
        return false;
    }

    void simulate(efloat delta_time) {
        Dot ddp = dir * speed;
        simulate_move2d(pos, dp, ddp, delta_time);
    }

    void draw() const override {
        draw_collision_obj(*this);
        auto circ = get_collision().circle;
        static_pos_update(circ.pos, global_variables::camera_is_static);
        circ.radius = 1.5;
        draw_circle(circ, Color(0xbd9919, 255));
        circ.radius = 1;
        draw_circle(circ, Color(0xfcca12, 255));
    }
};

#endif