#ifndef GAME_ENGINE_MOVE_HPP
#define GAME_ENGINE_MOVE_HPP

#include "../utils.hpp"
#include "Geometry/dot.hpp"

/*
 * p = позиция
 *
 * dp = p' = инерция движения, которую мы накапливаем, чтобы например когда игрок идет вправо,
 * а потом резко поворачивает налево, то он еще некоторое время по инерции уходил вправо.
 * А также, чтобы когда он только начал идти, то сначала медленно, и с каждым разом чуть быстрее до некоторого значения.
 *
 * ddp = p'' = момент движения в кадре. Когда начинается кадр, то ddp = 0. Когда игрок нажимает кнопку идти вправо, то
 * ddp += C, а если влево то ddp -= C. Таким образом, если он нажал и вправо и влево, то он никуда в итоге не пойдет.
 *
 * После симуляции такого движения частичка ddp перейдет в dp, чтобы накопить инерцию движения
 *
 * delta_time = время между текущим кадром и предыдущим
 */

/*
 * симулирует движение по системе, описанной в начале файла
 */
void simulate_move(efloat &p, efloat &dp, efloat ddp, efloat delta_time) {
    ddp -= dp * 15;
    p = p + dp * delta_time + ddp * quare(delta_time) / 2;
    dp = dp + ddp * delta_time;
}

/*
 * аналогично simulate_move, только на плоскости
 */
void simulate_move2d(Dot &p, Dot &dp, Dot ddp, efloat delta_time) {
    simulate_move(p.x, dp.x, ddp.x, delta_time);
    simulate_move(p.y, dp.y, ddp.y, delta_time);
}

/*
 * Симулирует движение до точки p_to по системе из simulate_move
 * Если мы ее перешагнули или что-то сломалось, то мы окажемся в точке p_to.
 * Это нужно, чтобы когда мы были слишком близко к точке p_to, то не постоянно метались между ней, а сразу попали в неё
 */
void simulate_move_to(efloat &p, efloat p_to, efloat &dp, efloat ddp, efloat delta_time) {
    efloat result = p;

    simulate_move(result, dp, ddp, delta_time);

    auto is_spoiled = [](efloat num) -> bool {
        return isnan(num) || isinf(num);
    };

    if (is_spoiled(result) || is_spoiled(dp) || is_spoiled(ddp) ||
        (p <= p_to && p_to <= result) || (result <= p_to && p_to <= p)) {
        result = p_to;
        dp = 0;
    }

    p = result;
}

/*
 * аналогично simulate_move_to, только на плоскости
 */
void simulate_move_to2d(Dot &p, const Dot &p_to, Dot &dp, Dot ddp, efloat delta_time) {
    simulate_move_to(p.x, p_to.x, dp.x, ddp.x, delta_time);
    simulate_move_to(p.y, p_to.y, dp.y, ddp.y, delta_time);
}

enum class direction_t : u8 {
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
    return {0, 0};
}

#endif  // GAME_ENGINE_MOVE_HPP