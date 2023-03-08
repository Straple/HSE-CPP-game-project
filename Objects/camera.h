#ifndef GAME_ENGINE_CAMERA
#define GAME_ENGINE_CAMERA

#include "..\utils.h"
#include "move.h"

struct Camera {
    Dot pos;
    Dot dp;

    // симулирует движение камеры к игроку
    void simulate(const Dot &player_pos, efloat delta_time) {
        move_to2d(pos, player_pos, dp, (player_pos - pos) * 20, delta_time);
    }
};

#endif  // GAME_ENGINE_CAMERA