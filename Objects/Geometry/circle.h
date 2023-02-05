#ifndef GAME_ENGINE_GEOMETRY_CIRCLE
#define GAME_ENGINE_GEOMETRY_CIRCLE

#include "dot.h"
#include "utils.h"

// (pos, radius)
struct Circle {
    Dot pos;
    efloat radius = 0;

    Circle() = default;

    Circle(const Dot &new_pos, efloat new_radius)
        : pos(new_pos), radius(new_radius) {
    }

    // возвращает точку на окружности
    // угол против часовой стрелки
    [[nodiscard]] Dot get_dot(efloat angle) const {
        return pos + Dot(cos(angle), sin(angle)) * radius;
    }

    // возвращает рандомную точку на окружности
    [[nodiscard]] Dot get_random_dot() const {
        static std::uniform_int_distribution<int> random_range(
            0, PI * 2 * 1000
        );
        efloat random_angle = static_cast<efloat>(random_range(rnd)) / 1000;
        return get_dot(random_angle);
    }

    // проверка принадлежности точки окружности
    [[nodiscard]] bool ison(const Dot &point) const {
        return (pos - point).get_len() == radius;
    }

    // проверка принадлежности точки кругу
    [[nodiscard]] bool isin(const Dot &point) const {
        return (pos - point).get_len() <= radius;
    }
};

std::ostream &operator<<(std::ostream &output, const Circle &c) {
    return output << "Circle(" << c.pos << ", " << c.radius << ')';
}

#endif  // GAME_ENGINE_GEOMETRY_CIRCLE