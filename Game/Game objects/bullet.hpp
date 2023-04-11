#ifndef GAME_BULLET_HPP
#define GAME_BULLET_HPP

#include "../../render.hpp"
#include "abstract_game_object.hpp"
#include "effect.hpp"
#include "game_utils.hpp"
#include "heart.hpp"

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
    bool simulate_attack(std::vector<enemy_t> &Enemies) {
        for (int i = 0; i < Enemies.size(); i++) {
            if (get_collision().trigger(Enemies[i].get_hitbox()) &&
                !Enemies[i].is_invulnerable()) {
                // simulate hit
                {
                    add_hit_effect(pos);

                    Enemies[i].hp -= damage;

                    Enemies[i].dp += dir * speed / 10;
                    Enemies[i].paralyzed_accum = 0;
                }

                if (Enemies[i].hp <= 0) {
                    add_death_effect(Enemies[i].get_hitbox().circle.pos);

                    if (randomness(100)) {
                        Loot_hearts.push_back(
                            Heart(Enemies[i].get_hitbox().circle.pos, dir)
                        );
                    }

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
//        draw_sprite(pos + Dot(-2, 2), 0.2, SP_COIN);
        draw_collision(*this);
        draw_sprite(pos + Dot(-2, 6), 0.2, SP_COIN);

    }
};

std::vector<Bullet> Bullets;

#endif