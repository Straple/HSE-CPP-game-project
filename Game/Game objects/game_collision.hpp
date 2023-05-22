﻿#ifndef GAME_COLLISION_HPP
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

void simulate_game_collisions(const std::vector<CollisionBox> &Walls) {
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
    }

    // сами с собой
    {
        // player bubbling player
        for (auto &player1 : Players) {
            if (!player1.is_paralyzed) {  // пока мы парализованы, мы не выталкиваемы
                for (auto &player2 : Players) {
                    // чтобы не выталкивать самих себя
                    if (&player1 != &player2) {
                        player1.push_out_of_collision_soft(*player2.get_collision());
                    }
                }
            }
        }

        // slime bubbling slime
        for (auto &slime1 : Slimes) {
            if (!slime1.is_devour) {  // пока мы едим игрока, мы не выталкиваемы
                for (auto &slime2 : Slimes) {
                    // чтобы не выталкивать самих себя
                    if (&slime1 != &slime2) {
                        slime1.push_out_of_collision_soft(*slime2.get_collision());
                    }
                }
            }
        }

        // bat bubbling bat
        /*static bool who = false;
        who = !who;
        for (int i = 0; i < Bats.size(); i++) {
            for (int j = i + 1; j < Bats.size(); j++) {
                if (who) {
                    // i bubble j
                    Bats[j].push_out_of_collision(*Bats[i].get_collision());
                } else {
                    // j bubble i
                    Bats[i].push_out_of_collision(*Bats[j].get_collision());
                }
            }
        }*/

        for (auto &bat1 : Bats) {
            for (auto &bat2 : Bats) {
                // чтобы не выталкивать самих себя
                if (&bat1 != &bat2) {
                    bat1.push_out_of_collision_soft(*bat2.get_collision());
                }
            }
        }

        /*for (const auto &wall : Walls) {
            for (auto &player : Players) {
                player.push_out_of_collision(wall);
            }
            for (auto &bat : Bats) {
                bat.push_out_of_collision(wall);
            }
            for (auto &slime : Slimes) {
                slime.push_out_of_collision(wall);
            }
        }*/

        // coin bubbling coin
        for (auto &coin1 : Loot_coins) {
            for (auto &coin2 : Loot_coins) {
                if (&coin1 != &coin2) {
                    coin1.push_out_of_collision_soft(*coin2.get_collision());
                }
            }
        }
    }

    // слайм не может пройти через Bush, Table
    for (auto &slime : Slimes) {
        for (const auto &bush : Bushes) {
            slime.push_out_of_collision_hard(*bush.get_collision());
        }
        for (const auto &table : Tables) {
            slime.push_out_of_collision_hard(*table.get_collision());
        }
    }

    // игрок не может пройти через Bush, Table
    for (auto &player : Players) {
        for (const auto &bush : Bushes) {
            player.push_out_of_collision_soft(*bush.get_collision());
        }
        for (const auto &table : Tables) {
            player.push_out_of_collision_soft(*table.get_collision());
        }
    }
}

// old version
/*
template <typename T1, typename T2>
bool verify_others_obj(const T1 &Lhs, const T2 &Rhs) {
    return reinterpret_cast<const void *>(&Lhs) !=
           reinterpret_cast<const void *>(&Rhs);
}

template <typename collision>
void update_collision(Player &player, const collision &coll) {
    if (!player.is_paralyzed) {
        player.pos = coll.bubble(player.get_collision());
    }
}

template <typename collision>
void update_collision(Slime &slime, const collision &coll) {
    if (!slime.is_attack) {
        slime.pos = coll.bubble(slime.get_collision());
    }
}

template <typename collision>
void update_collision(Bat &bat, const collision &coll) {
    bat.pos = coll.bubble(bat.get_collision());
}

template <typename collision>
void update_collision(Log &log, const collision &coll) {
    log.pos = coll.bubble(log.get_collision());
}

template <typename collision>
void update_collision(Bush &bush, const collision &coll) {
    bush.pos = coll.bubble(bush.get_collision());
}

template <typename collision>
void update_collision(Tree &tree, const collision &coll) {
    tree.pos = coll.bubble(tree.get_collision());
}

// массив объектов с коллизией выталкивет другие объекты
template <typename container_t>
void simulate_game_collision(container_t &Objects) {
    if (Objects.empty()) {
        return;
    }

    for (auto &obj : Objects) {
        obj.pos =
            Dot(clamp(-world_half_size.x, obj.pos.x, world_half_size.x),
                clamp(-world_half_size.y, obj.pos.y, world_half_size.y));
    }

    if (typeid(Objects.back()) == typeid(Bat)) {
        // bat2 толкает bat1
        for (auto &bat1 : Bats) {
            for (auto &bat2 : Bats) {
                if (verify_others_obj(bat1, bat2)) {
                    update_collision(bat2, bat1.get_collision());
                }
            }
        }
    } else {  // obj is slime or player

        // players
        if (typeid(Objects.back()) != typeid(Slime) &&
            typeid(Objects.back()) != typeid(Player)) {
            for (auto &obj : Objects) {
                update_collision(Players[0], obj.get_collision());
            }
        }

        // slimes
        for (auto &obj : Objects) {
            for (auto &slime : Slimes) {
                if (verify_others_obj(slime, obj)) {
                    update_collision(slime, obj.get_collision());
                }
            }
        }

        // trees
        for (auto &obj : Objects) {
            for (auto &tree : Trees) {
                if (verify_others_obj(tree, obj)) {
                    update_collision(obj, tree.get_collision());
                }
            }
        }

        // bushes
        for (auto &obj : Objects) {
            for (auto &bush : Bushes) {
                if (verify_others_obj(bush, obj)) {
                    update_collision(obj, bush.get_collision());
                }
            }
        }

        // logs
        for (auto &obj : Objects) {
            for (auto &logs : Logs) {
                if (verify_others_obj(logs, obj)) {
                    update_collision(logs, obj.get_collision());
                }
            }
        }
    }
}*/

#endif  // GAME_COLLISION_HPP