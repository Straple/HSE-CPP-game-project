#ifndef GAME_ENGINE_CAMERA_HPP
#define GAME_ENGINE_CAMERA_HPP

#include "../utils.hpp"
#include "move.hpp"

struct Camera {
    Dot pos;
    Dot dp;

    // симулирует движение камеры к игроку
    void simulate(const Dot &player_pos, efloat delta_time) {
        simulate_move_to2d(pos, player_pos, dp, (player_pos - pos) * 20, delta_time);
    }
};

#endif  // GAME_ENGINE_CAMERA_HPP