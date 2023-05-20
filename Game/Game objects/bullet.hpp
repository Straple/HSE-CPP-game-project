#ifndef GAME_BULLET_HPP
#define GAME_BULLET_HPP

#include "abstract_game_object.hpp"
#include "effect.hpp"
#include "enemy_states.hpp"
#include "game_utils.hpp"
#include "heart&coin.hpp"
#include "player.hpp"
#include "render.hpp"

enum class ShooterType {
    PLAYER,
    ENEMY,
};

struct Bullet : abstract_game_object {
    // Добавим позже поле формы пули, чтобы можно было
    // стрелять как кругами, так и прямоугольниками и ромбами.
    // Возможно, добавим лучевое оружие

    int speed;
    Dot dir;  // направление полета пули
    ShooterType host;
    int damage;

    Bullet(ShooterType h, Dot from, Dot to, int damage, int speed)
        : host(h), dir((to - from).normalize()), damage(damage), speed(speed) {
        pos = from;
        collision_radius = 2;
        delta_draw_pos = Dot(-10, 10);
    }

    [[nodiscard]] std::unique_ptr<AbstractCollision> get_collision() const override {
        return std::make_unique<CollisionCircle>(Circle(pos, collision_radius));
    }

    // вернет правду, если атака кого-то зацепила
    template <typename enemy_t>
    bool simulate_attack(std::vector<enemy_t> &Enemies) {
        if (host != ShooterType::PLAYER) {
            return false;
        }
        for (int i = 0; i < Enemies.size(); i++) {
            if (get_collision()->trigger(*Enemies[i].get_hitbox()) &&
                !Enemies[i].is_invulnerable()) {
                // simulate hit
                {
                    add_hit_effect(pos);

                    Enemies[i].hp -= damage;

                    Enemies[i].dp += dir * speed / 10;
                    Enemies[i].paralyzed_accum = 0;
                }

                if (Enemies[i].hp <= 0) {
                    add_death_effect(Enemies[i].get_hitbox()->get_pos());

                    if (randomness(20)) {
                        Loot_hearts.push_back(
                            Heart(Enemies[i].get_hitbox()->get_pos(), dir)
                        );
                    } else {
                        if (randomness(50)) {
                            for (int count = 0; count < 4; count++) {
                                Loot_coins.push_back(Coin(
                                    Enemies[i].get_hitbox()->get_pos(), dir
                                ));
                            }
                        } else {
                            for (int count = 0; count < 5; count++) {
                                Loot_coins.push_back(Coin(
                                    Enemies[i].get_hitbox()->get_pos(), dir
                                ));
                            }
                        }
                    }

                    Enemies.erase(Enemies.begin() + i);
                    i--;
                }
                return true;
            }
        }
        return false;
    }

    template <typename Player_type>
    bool simulate_attack_on_player(std::vector<Player_type> &Players) {
        if (host != ShooterType::ENEMY) {
            return false;
        }
        for (auto &player : Players) {
            if (get_collision()->trigger(*player.get_hitbox()) &&
                !player.is_invulnerable()) {
                // simulate hit
                {
                    add_hit_effect(pos);

                    player.hp -= damage;

                    player.dp += dir * speed / 10;
                }

                //                if (Enemies[i].hp <= 0) {
                //                    add_death_effect(Enemies[i].get_hitbox().circle.pos);
                //
                //                    if (randomness(20)) {
                //                        Loot_hearts.push_back(
                //                                Heart(Enemies[i].get_hitbox().circle.pos, dir)
                //                        );
                //                    } else {
                //                        if (randomness(50)) {
                //                            for (int count = 0; count < 4; count++) {
                //                                Loot_coins.push_back(Coin(
                //                                        Enemies[i].get_hitbox().circle.pos, dir
                //                                ));
                //                            }
                //                        } else {
                //                            for (int count = 0; count < 5; count++) {
                //                                Loot_coins.push_back(Coin(
                //                                        Enemies[i].get_hitbox().circle.pos, dir
                //                                ));
                //                            }
                //                        }
                //                    }
                //
                //                    Enemies.erase(Enemies.begin() + i);
                //                    i--;
                //                }
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
        draw_sprite(pos + Dot(-2, 2), 0.4, SP_COIN);
        draw_collision(*this);
    }
};

std::vector<Bullet> Bullets;

#endif