#ifndef GAME_ENGINE_COLLISION
#define GAME_ENGINE_COLLISION

#include "Geometry\geometry.h"

struct collision_circle {
    Circle circle;

    collision_circle() = default;

    explicit collision_circle(const Circle &new_circle) : circle(new_circle) {
    }

    // находится ли точка внутри коллизии
    [[nodiscard]] bool trigger(const Dot &pos) const {
        return circle.isin(pos);
    }

    // пересекаются ли коллизии
    [[nodiscard]] bool trigger(const collision_circle &coll) const {
        return collision_circle(
                   Circle(circle.pos, circle.radius + coll.circle.radius)
        )
            .trigger(coll.circle.pos);
    }

    // выталкивает pos за пределы коллизии
    void bubble(Dot &pos) const {
        if (circle.isin(pos)) {
            if (pos == circle.pos) {
                pos = circle.get_random_dot();
            } else {
                Dot vector = (pos - circle.pos).normalize() * circle.radius;
                pos = circle.pos + vector;
            }
        }
    }

    // выталкивает коллизию coll за пределы this коллизии
    [[nodiscard]] Dot bubble(const collision_circle &coll) const {
        Dot res = coll.circle.pos;
        collision_circle(Circle(circle.pos, circle.radius + coll.circle.radius))
            .bubble(res);
        return res;
    }
};

/*  p0---+
 *  |    |
 *  |    |
 *  |    |
 *  +----p1
 */
struct collision_box {
    Dot p0, p1;

    collision_box() = default;

    collision_box(const Dot &new_p0, const Dot &new_p1)
        : p0(new_p0), p1(new_p1) {
        ASSERT(p0.x <= p1.x && p1.y <= p0.y, "bad points");
    }

    // находится ли точка внутри коллизии
    [[nodiscard]] bool trigger(const Dot &pos) const {
        return is_between(p0.x, pos.x, p1.x) && is_between(p1.y, pos.y, p0.y);
    }

    // выталкивает pos за пределы коллизии
    void bubble(Dot &pos) const {
        if (!trigger(pos)) {
            return;
        }

        efloat best_dist = std::min({
            std::abs(p0.x - pos.x),
            std::abs(p1.x - pos.x),

            std::abs(p0.y - pos.y),
            std::abs(p1.y - pos.y),
        });

        if (best_dist == std::abs(p0.x - pos.x)) {
            pos.x = p0.x;
        } else if (best_dist == std::abs(p1.x - pos.x)) {
            pos.x = p1.x;
        } else if (best_dist == std::abs(p0.y - pos.y)) {
            pos.y = p0.y;
        } else {
            pos.y = p1.y;
        }
    }

    [[nodiscard]] Dot bubble(const collision_circle &coll) const {
        Dot res = coll.circle.pos;
        collision_box(
            p0 + Dot(-1, 1) * coll.circle.radius,
            p1 + Dot(1, -1) * coll.circle.radius
        )
            .bubble(res);
        return res;
    }
};

#endif  // GAME_ENGINE_COLLISION