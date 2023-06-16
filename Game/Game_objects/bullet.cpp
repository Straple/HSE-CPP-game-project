#include "bullet.hpp"
#include "bat.hpp"
#include "effect.hpp"
#include "game_variables.hpp"
#include "slime.hpp"

Bullet::Bullet(BulletHostType host, Dot from, Dot to, int damage, int speed, sprite_t sp)
    : host(host), ddp((to - from).normalize() * speed), damage(damage), sprite(sp) {
    pos = from;
}

[[nodiscard]] std::unique_ptr<Collision> Bullet::get_collision() const {
    return std::make_unique<CollisionCircle>(pos, collision_radius);
}

void Bullet::simulate(efloat delta_time) {
    simulate_move2d(pos, dp, ddp, delta_time);
}

void Bullet::draw() {
    draw_sprite(pos + Dot(-2, 2), 0.4, sprite);
    draw_collision(*this);
}

bool Bullet::simulate_attack_on_mob(std::vector<Slime> &Enemies) {
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
                Enemies[i].paralyzed_accum = Slime::paralyzed_cooldown;
            }

            if (Enemies[i].hp <= 0) {
                add_death_effect(Enemies[i].get_hitbox()->get_pos());

                if (randomness(20)) {
                    game_variables::Loot_hearts.push_back(Heart(Enemies[i].get_hitbox()->get_pos(), ddp.normalize()));
                } else if (randomness(50)) {
                    for (int count = 0; count < 4; count++) {
                        game_variables::Loot_coins.push_back(Coin(Enemies[i].get_hitbox()->get_pos(), ddp.normalize()));
                    }
                } else {
                    for (int count = 0; count < 8; count++) {
                        game_variables::Loot_coins.push_back(Coin(Enemies[i].get_hitbox()->get_pos(), ddp.normalize()));
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

bool Bullet::simulate_attack_on_mob(std::vector<Bat> &Enemies) {
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
                Enemies[i].paralyzed_accum = Bat::paralyzed_cooldown;
            }

            if (Enemies[i].hp <= 0) {
                add_death_effect(Enemies[i].get_hitbox()->get_pos());

                if (randomness(20)) {
                    game_variables::Loot_hearts.push_back(Heart(Enemies[i].get_hitbox()->get_pos(), ddp.normalize()));
                } else if (randomness(50)) {
                    for (int count = 0; count < 4; count++) {
                        game_variables::Loot_coins.push_back(Coin(Enemies[i].get_hitbox()->get_pos(), ddp.normalize()));
                    }
                } else {
                    for (int count = 0; count < 8; count++) {
                        game_variables::Loot_coins.push_back(Coin(Enemies[i].get_hitbox()->get_pos(), ddp.normalize()));
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

bool Bullet::simulate_attack_on_mob(std::vector<Bomber> &Enemies) {
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
                Enemies[i].paralyzed_accum = Bomber::paralyzed_cooldown;
            }

            if (Enemies[i].hp <= 0) {
                add_death_effect(Enemies[i].get_hitbox()->get_pos());

                if (randomness(20)) {
                    game_variables::Loot_hearts.push_back(Heart(Enemies[i].get_hitbox()->get_pos(), ddp.normalize()));
                } else if (randomness(50)) {
                    for (int count = 0; count < 4; count++) {
                        game_variables::Loot_coins.push_back(Coin(Enemies[i].get_hitbox()->get_pos(), ddp.normalize()));
                    }
                } else {
                    for (int count = 0; count < 8; count++) {
                        game_variables::Loot_coins.push_back(Coin(Enemies[i].get_hitbox()->get_pos(), ddp.normalize()));
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

bool Bullet::simulate_attack_on_player(std::vector<Player> &Players) {
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
}