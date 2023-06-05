#ifndef GAME_COLLISION_HPP
#define GAME_COLLISION_HPP

#include "bush.hpp"
#include "log.hpp"
#include "table.hpp"
#include "tree.hpp"

// enemies
#include "bat.hpp"
#include "slime.hpp"
#include "bomber.hpp"

// player
#include "player.hpp"

//loot
#include "heart_and_coin.hpp"

#include "game_variables.hpp"

void simulate_game_collisions(efloat delta_time, const std::vector<CollisionBox> &Walls);

#endif  // GAME_COLLISION_HPP