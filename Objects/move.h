#ifndef GAME_ENGINE_MOVE
#define GAME_ENGINE_MOVE

#include "Geometry\dot.h"
#include "utils.h"

void simulate_move(efloat &p, efloat &dp, efloat ddp, efloat dt) {
    ddp -= dp * 15;
    p = p + dp * dt + ddp * dt * dt * 0.5;
    dp = dp + ddp * dt;
}

void simulate_move2d(Dot &p, Dot &dp, Dot ddp, efloat dt) {
    simulate_move(p.x, dp.x, ddp.x, dt);
    simulate_move(p.y, dp.y, ddp.y, dt);
}

void move_to(efloat &p, efloat p_to, efloat &dp, efloat ddp, efloat dt) {
    efloat to = p;

    simulate_move(to, dp, ddp, dt);

    auto is_spoiled = [](efloat num) -> bool {
        return isnan(num) || isinf(num);
    };

    if (is_spoiled(to) || is_spoiled(dp) || is_spoiled(ddp) ||
        (p <= p_to && p_to <= to) || (to <= p_to && p_to <= p)) {
        to = p_to;
        dp = 0;
    }

    p = to;
}

void move_to2d(Dot &p, const Dot &p_to, Dot &dp, Dot ddp, efloat dt) {
    move_to(p.x, p_to.x, dp.x, ddp.x, dt);
    move_to(p.y, p_to.y, dp.y, ddp.y, dt);
}

enum class direction_t : u8 {
    // DON'T SHUFFLE THIS CODE

    NONE = 0,

    RIGHT = 1,
    UP = 2,
    LEFT = 3,
    DOWN = 4,

    COUNT = 5,
};

Dot get_direction(direction_t dir) {
    switch (dir) {
        case direction_t::NONE: {
            return {};
        }
        case direction_t::RIGHT: {
            return {1, 0};
        }
        case direction_t::UP: {
            return {0, 1};
        }
        case direction_t::LEFT: {
            return {-1, 0};
        }
        case direction_t::DOWN: {
            return {0, -1};
        }
        case direction_t::COUNT:
            break;
    }
    ASSERT(false, "incorrect direction type");
}

#endif  // GAME_ENGINE_MOVE