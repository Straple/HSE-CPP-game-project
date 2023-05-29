#ifndef GAME_COLLISION_HPP
#define GAME_COLLISION_HPP

#include "bush.hpp"
#include "log.hpp"
#include "table.hpp"
#include "tree.hpp"

// enemies
#include "bat.hpp"
#include "slime.hpp"

// player
#include "player.hpp"

void simulate_game_collisions(efloat delta_time, const std::vector<CollisionBox> &Walls) {
    // через стены никто пройти не может
    for (const auto &wall : Walls) {
        for (auto &player : Players) {
            player.push_out_of_collision_hard(wall);
        }
        for (auto &bat : Bats) {
            bat.push_out_of_collision_hard(wall);
        }
        for (auto &slime : Slimes) {
            slime.push_out_of_collision_hard(wall);
        }
        for (auto &coin : Loot_coins) {
            coin.push_out_of_collision_hard(wall);
        }
        for (auto &heart : Loot_hearts) {
            heart.push_out_of_collision_hard(wall);
        }
    }

    // сами с собой
    {
        // player
        for (auto &player1 : Players) {
            if (!player1.is_paralyzed&&!player1.is_dead) {  // пока мы парализованы или мертвы, мы не выталкиваемы
                for (auto &player2 : Players) {
                    // чтобы не выталкивать самих себя
                    if (&player1 != &player2) {
                        player1.push_out_of_collision_soft(*player2.get_collision(), delta_time);
                    }
                }
            }
        }

        // slime
        for (auto &slime1 : Slimes) {
            if (!slime1.is_devour) {  // пока слайм ест игрока, он не выталкиваем
                for (auto &slime2 : Slimes) {
                    // чтобы не выталкивать самих себя
                    if (&slime1 != &slime2) {
                        slime1.push_out_of_collision_soft(*slime2.get_collision(), delta_time);
                    }
                }
            }
        }

        // bat
        for (auto &bat1 : Bats) {
            for (auto &bat2 : Bats) {
                // чтобы не выталкивать самих себя
                if (&bat1 != &bat2) {
                    bat1.push_out_of_collision_soft(*bat2.get_collision(), delta_time);
                }
            }
        }

        // coin
        for (auto &coin1 : Loot_coins) {
            for (auto &coin2 : Loot_coins) {
                if (&coin1 != &coin2) {
                    coin1.push_out_of_collision_soft(*coin2.get_collision(), delta_time);
                }
            }
        }

        for (auto &heart1 : Loot_hearts) {
            for (auto &heart2 : Loot_hearts) {
                if (&heart1 != &heart2) {
                    heart1.push_out_of_collision_soft(*heart2.get_collision(), delta_time);
                }
            }
        }
    }

    // player <-> slime
    for (auto &player : Players) {
        if (player.is_dead) {
            continue;
        }
        for (auto &slime : Slimes) {
            if (!slime.is_devour) {  // пока слайм ест игрока, он не выталкиваем
                slime.push_out_of_collision_soft(*player.get_collision(), delta_time);
            }
            if (!player.is_paralyzed) {  // пока кто-то ест игрока, игрок не выталкиваем
                player.push_out_of_collision_soft(*slime.get_collision(), delta_time);
            }
        }
    }

    // heart <-> coin
    for (auto &heart : Loot_hearts) {
        for (auto &coin : Loot_coins) {
            heart.push_out_of_collision_soft(*coin.get_collision(), delta_time);
            coin.push_out_of_collision_soft(*heart.get_collision(), delta_time);
        }
    }

    // слайм не может пройти через Bush, Table
    for (auto &slime : Slimes) {
        for (const auto &bush : Bushes) {
            // TODO: maybe soft?
            slime.push_out_of_collision_hard(*bush.get_collision());
        }
        for (const auto &table : Tables) {
            // TODO: maybe soft?
            slime.push_out_of_collision_hard(*table.get_collision());
        }
    }

    // игрок не может пройти через Bush, Table
    for (auto &player : Players) {
        for (const auto &bush : Bushes) {
            player.push_out_of_collision_soft(*bush.get_collision(), delta_time);
        }
        for (const auto &table : Tables) {
            player.push_out_of_collision_soft(*table.get_collision(), delta_time);
        }
    }
}

#endif  // GAME_COLLISION_HPP