#include "game_collision.hpp"
//

// world
#include "bush.hpp"
#include "log.hpp"
#include "table.hpp"
#include "tree.hpp"

// enemies
#include "bat.hpp"
#include "bomber.hpp"
#include "slime.hpp"

// player
#include "player.hpp"

// loot
#include "heart_and_coin.hpp"

//
#include "game_variables.hpp"

void simulate_game_collisions(efloat delta_time, const std::vector<CollisionBox> &Walls) {
    // через стены никто пройти не может
    for (const auto &wall : Walls) {
        for (auto &player : game_variables::Players) {
            player.push_out_of_collision_hard(wall);
        }
        for (auto &bat : game_variables::Bats) {
            bat.push_out_of_collision_hard(wall);
        }
        for (auto &slime : game_variables::Slimes) {
            slime.push_out_of_collision_hard(wall);
        }
        for (auto &bomber : game_variables::Bombers) {
            bomber.push_out_of_collision_hard(wall);
        }
        for (auto &coin : game_variables::Loot_coins) {
            coin.push_out_of_collision_hard(wall);
        }
        for (auto &heart : game_variables::Loot_hearts) {
            heart.push_out_of_collision_hard(wall);
        }
        for (auto &knight : game_variables::Knights) {
            knight.push_out_of_collision_hard(wall);
        }
        for (auto &barrel : game_variables::Barrels) {
            barrel.push_out_of_collision_hard(wall);
        }
    }

    // сами с собой
    {
        // player
        for (auto &player1 : game_variables::Players) {
            if (!player1.is_paralyzed && !player1.is_dead()) {  // пока мы парализованы или мертвы, мы не выталкиваемы
                for (auto &player2 : game_variables::Players) {
                    // чтобы не выталкивать самих себя
                    if (&player1 != &player2) {
                        player1.push_out_of_collision_soft(*player2.get_collision(), delta_time);
                    }
                }
            }
        }

        // slime
        for (auto &slime1 : game_variables::Slimes) {
            if (!slime1.is_devour) {  // пока слайм ест игрока, он не выталкиваем
                for (auto &slime2 : game_variables::Slimes) {
                    // чтобы не выталкивать самих себя
                    if (&slime1 != &slime2) {
                        slime1.push_out_of_collision_soft(*slime2.get_collision(), delta_time);
                    }
                }
            }
        }

        // bomber
        for (auto &bomber1 : game_variables::Bombers) {
            for (auto &bomber2 : game_variables::Bombers) {
                // чтобы не выталкивать самих себя
                if (&bomber1 != &bomber2 && !bomber1.is_booming && !bomber2.is_booming) {
                    bomber1.push_out_of_collision_soft(*bomber2.get_collision(), delta_time);
                }
            }
        }

        // bat
        for (auto &bat1 : game_variables::Bats) {
            for (auto &bat2 : game_variables::Bats) {
                // чтобы не выталкивать самих себя
                if (&bat1 != &bat2) {
                    bat1.push_out_of_collision_soft(*bat2.get_collision(), delta_time);
                }
            }
        }

        // coin
        for (auto &coin1 : game_variables::Loot_coins) {
            for (auto &coin2 : game_variables::Loot_coins) {
                if (&coin1 != &coin2) {
                    coin1.push_out_of_collision_soft(*coin2.get_collision(), delta_time);
                }
            }
        }

        for (auto &heart1 : game_variables::Loot_hearts) {
            for (auto &heart2 : game_variables::Loot_hearts) {
                if (&heart1 != &heart2) {
                    heart1.push_out_of_collision_soft(*heart2.get_collision(), delta_time);
                }
            }
        }
    }

    // player <-> slime
    for (auto &player : game_variables::Players) {
        if (player.is_dead()) {
            continue;
        }
        for (auto &slime : game_variables::Slimes) {
            if (!slime.is_devour) {  // пока слайм ест игрока, он не выталкиваем
                slime.push_out_of_collision_soft(*player.get_collision(), delta_time);
            }
            if (!player.is_paralyzed) {  // пока кто-то ест игрока, игрок не выталкиваем
                player.push_out_of_collision_soft(*slime.get_collision(), delta_time);
            }
        }
    }

    // heart <-> coin
    for (auto &heart : game_variables::Loot_hearts) {
        for (auto &coin : game_variables::Loot_coins) {
            heart.push_out_of_collision_soft(*coin.get_collision(), delta_time);
            coin.push_out_of_collision_soft(*heart.get_collision(), delta_time);
        }
    }

    // слайм не может пройти через Bush, Tree, Table
    for (auto &slime : game_variables::Slimes) {
        for (const auto &bush : game_variables::Bushes) {
            // TODO: maybe soft?
            slime.push_out_of_collision_hard(*bush.get_collision());
        }
        for (const auto &tree : game_variables::Trees) {
            // TODO: maybe soft?
            slime.push_out_of_collision_hard(*tree.get_collision());
        }
        for (const auto &table : game_variables::Tables) {
            // TODO: maybe soft?
            slime.push_out_of_collision_hard(*table.get_collision());
        }
    }

    // игрок не может пройти через Bush, Tree, Table, Pillar
    for (auto &player : game_variables::Players) {
        for (const auto &bush : game_variables::Bushes) {
            player.push_out_of_collision_soft(*bush.get_collision(), delta_time);
        }
        for (const auto &tree : game_variables::Trees) {
            player.push_out_of_collision_soft(*tree.get_collision(), delta_time);
        }
        for (const auto &table : game_variables::Tables) {
            player.push_out_of_collision_soft(*table.get_collision(), delta_time);
        }
        for (const auto &table : game_variables::Tables) {
            player.push_out_of_collision_soft(*table.get_collision(), delta_time);
        }
        for (const auto &pillar : game_variables::Pillars) {
            player.push_out_of_collision_hard(*pillar.get_collision());
        }
        for (const auto &nun_statue : game_variables::NunStatues) {
            player.push_out_of_collision_hard(*nun_statue.get_collision());
        }
        for (auto &knight : game_variables::Knights) {
            player.push_out_of_collision_soft(*knight.get_collision(), delta_time);
            knight.push_out_of_collision_soft(*player.get_collision(), delta_time);
        }
        for (auto &barrel : game_variables::Barrels) {
            player.push_out_of_collision_soft(*barrel.get_collision(), delta_time);
            barrel.push_out_of_collision_soft(*player.get_collision(), delta_time);
        }
    }
}
