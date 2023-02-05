#pragma once

#include "dot.h"
#include "../../utils.h"

// pos, radius
struct Circle {
    dot pos;
    point_t radius;

    Circle() {
        radius = 0;
    }
    Circle(const dot& pos, point_t radius) {
        this->pos = pos;
        this->radius = radius;
    }

    // returns a point on a circle
    // counterclockwise angle
    dot point(point_t angle) const {
        return pos + dot(cos(angle), sin(angle)) * radius;
    }

    dot get_random_dot() const {
        std::uniform_int_distribution<s32> random_range(0, PI * 2 * 1000);

        point_t random_angle = static_cast<point_t>(random_range(rnd)) / 1000;

        return point(random_angle);
    }

    // проверка принадлежности точки окружности
    bool ison(const dot& point) const {
        return (pos - point).getLen() == radius;
    }

    bool isin(const dot& point) const {
        return (pos - point).getLen() <= radius;
    }
};
