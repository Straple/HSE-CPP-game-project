#pragma once

#include "../utils.h"
#include "../move.h"

struct Camera {
    dot pos;
    dot dp;

    void simulate(const dot& player_pos, point_t delta_time) {

        move_to2d(pos, player_pos, dp, (player_pos - pos) * 20 , delta_time);
    }
};
