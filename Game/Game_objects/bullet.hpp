#ifndef GAME_BULLET_HPP
#define GAME_BULLET_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"

enum class BulletHostType {
    // эту пулю выпустил игрок
    PLAYER,
    // эту пулю выпустил моб
    ENEMY,
};

struct Bullet : AbstractPhysicalObject {
    ADD_BYTE_SERIALIZATION();

    // Добавим позже поле формы пули, чтобы можно было
    // стрелять как кругами, так и прямоугольниками и ромбами.
    // Возможно, добавим лучевое оружие

    // physics
    inline const static efloat collision_radius = 2;

    Dot ddp;

    int damage;

    sprite_t sprite;

    BulletHostType host;

    Bullet() = default;

    Bullet(BulletHostType host, Dot from, Dot to, int damage, int speed, sprite_t sp);

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

    // вернет правду, если атака кого-то зацепила
    /*template <typename enemy_t>
    bool simulate_attack_on_mob(std::vector<enemy_t> &Enemies) {
        if (host != BulletHostType::PLAYER) {
            return false;
        }
        for (int i = 0; i < Enemies.size(); i++) {
            if (!Enemies[i].is_invulnerable() && get_collision()->trigger(*Enemies[i].get_hitbox())) {
                // simulate hit
                {
                    add_hit_effect(pos);

                    Enemies[i].hp -= damage;
                    Enemies[i].dp += ddp / 10;
                    Enemies[i].paralyzed_accum = enemy_t::paralyzed_cooldown;
                }

                if (Enemies[i].hp <= 0) {
                    add_death_effect(Enemies[i].get_hitbox()->get_pos());

                    if (randomness(20)) {
                        game_variables::Loot_hearts.push_back(Heart(Enemies[i].get_hitbox()->get_pos(), ddp.normalize())
                        );
                    } else if (randomness(50)) {
                        for (int count = 0; count < 4; count++) {
                            game_variables::Loot_coins.push_back(
                                Coin(Enemies[i].get_hitbox()->get_pos(), ddp.normalize())
                            );
                        }
                    } else {
                        for (int count = 0; count < 8; count++) {
                            game_variables::Loot_coins.push_back(
                                Coin(Enemies[i].get_hitbox()->get_pos(), ddp.normalize())
                            );
                        }
                    }

                    Enemies.erase(Enemies.begin() + i);
                    i--;
                }
                return true;
            }
        }
        return false;
    }*/

    // вернет правду, если атака кого-то зацепила
    /*template <typename Player_type>
    bool simulate_attack_on_player(std::vector<Player_type> &Players) {
        if (host != BulletHostType::ENEMY) {
            return false;
        }
        for (auto &player : Players) {
            if (!player.is_invulnerable() && get_collision()->trigger(*player.get_hitbox())) {
                // simulate hit
                {
                    add_hit_effect(pos);
                    player.hp -= damage;
                    if (player.hp <= 0) {
                        player.die();
                    }
                    player.dp += ddp / 10;
                }
                player.set_invulnerable();
                return true;
            }
        }
        return false;
    }*/

    void simulate(efloat delta_time);

    void draw() override;
};

#endif  // GAME_BULLET_HPP