#pragma once

#include "utils.h"
#include "Objects/Geometry/dot.h"

void simulate_move(point_t& p, point_t& dp, point_t ddp, point_t dt) {
	ddp -= dp * 15;
	p = p + dp * dt + ddp * quare(dt) * 0.5;
	dp = dp + ddp * dt;
}

void simulate_move2d(dot& p, dot& dp, dot ddp, point_t dt) {
	simulate_move(p.x, dp.x, ddp.x, dt);
	simulate_move(p.y, dp.y, ddp.y, dt);
}

void move_to(point_t& p, point_t p_to, point_t& dp, point_t ddp, point_t dt) {

    point_t to = p;

    simulate_move(to, dp, ddp, dt);

    auto is_spoiled = [](point_t num) -> bool {
        return isnan(num) || isinf(num);
    };

    if (is_spoiled(to) || is_spoiled(dp)) {
        to = p_to;
        dp = 0;
    }
    // p----p_to---to
    // or
    // to---p_to---p
    else if((p <= p_to && p_to <= to) || (to <= p_to && p_to <= p)) {
        to = p_to;
        dp = 0;
    }

    p = to;
}

void move_to2d(dot& p, const dot& p_to, dot& dp, dot ddp, point_t dt) {
    move_to(p.x, p_to.x, dp.x, ddp.x, dt);
    move_to(p.y, p_to.y, dp.y, ddp.y, dt);
}

enum class direction_t : u8 {

    // DON'T SHUFFLE THIS CODE

    NONE,

    RIGHT,
    UP,
    LEFT,
    DOWN,

    COUNT,
};

dot get_direction(direction_t dir) {
    ASSERT(dir != direction_t::COUNT, "wrong direction");

    switch (dir) {
        case direction_t::NONE: {
            return dot();
        }
        case direction_t::RIGHT: {
            return dot(1, 0);
        }
        case direction_t::UP: {
            return dot(0, 1);
        }
        case direction_t::LEFT: {
            return dot(-1, 0);
        }
        case direction_t::DOWN: {
            return dot(0, -1);
        }
    }

    ASSERT(false, "undefind direction type");
}

