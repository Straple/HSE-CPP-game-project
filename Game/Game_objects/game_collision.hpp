#ifndef GAME_COLLISION_HPP
#define GAME_COLLISION_HPP

#include "../../objects.hpp"

void simulate_game_collisions(
    efloat delta_time,
    const std::vector<CollisionBox> &Walls
);

#endif  // GAME_COLLISION_HPP