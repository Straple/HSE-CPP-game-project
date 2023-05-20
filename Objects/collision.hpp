#ifndef GAME_ENGINE_COLLISION_HPP
#define GAME_ENGINE_COLLISION_HPP

#include <memory>
#include "Geometry/geometry.hpp"

struct AbstractCollision {
    // некоторая позиция
    [[nodiscard]] virtual Dot get_pos() const = 0;

    // находится ли точка внутри коллизии
    [[nodiscard]] virtual bool trigger(const Dot &pos) const = 0;

    [[nodiscard]] virtual bool trigger(const AbstractCollision &other) const = 0;

    // вернет вытолкнутую точку за пределы коллизии
    [[nodiscard]] virtual Dot bubble(Dot pos) const = 0;

    [[nodiscard]] virtual std::unique_ptr<AbstractCollision> bubble(const AbstractCollision &other) const = 0;

    virtual ~AbstractCollision() = default;
};

struct CollisionBox;

struct CollisionCircle : AbstractCollision {
    Circle circle;

    CollisionCircle() = default;

    explicit CollisionCircle(const Circle &circle)
        : circle(circle) {
    }

    CollisionCircle(Dot pos, efloat radius)
        : circle(pos, radius) {
    }

    [[nodiscard]] Dot get_pos() const override {
        return circle.pos;
    }

    [[nodiscard]] bool trigger(const Dot &pos) const override {
        return circle.isin(pos);
    }

    [[nodiscard]] bool trigger(const CollisionCircle &other) const {
        return Circle(circle.pos, circle.radius + other.circle.radius).isin(other.circle.pos);
    }

    [[nodiscard]] bool trigger(const CollisionBox &other) const;

    [[nodiscard]] bool trigger(const AbstractCollision &other) const override;

    [[nodiscard]] Dot bubble(Dot pos) const override {
        if (circle.isin(pos)) {
            if (pos == circle.pos) {
                pos = circle.get_random_dot();
            } else {
                Dot vector = (pos - circle.pos).normalize() * circle.radius;
                pos = circle.pos + vector;
            }
        }
        return pos;
    }

    [[nodiscard]] std::unique_ptr<AbstractCollision> bubble(const AbstractCollision &other) const override;

    [[nodiscard]] CollisionCircle bubble(const CollisionCircle &other) const {
        return {CollisionCircle(circle.pos, circle.radius + other.circle.radius).bubble(other.circle.pos), other.circle.radius};
    }

    [[nodiscard]] CollisionBox bubble(const CollisionBox &other) const;
};

/*  p0---+
 *  |    |
 *  |    |
 *  |    |
 *  +----p1
 */
struct CollisionBox : AbstractCollision {
    Dot p0, p1;

    CollisionBox() = default;

    CollisionBox(const Dot &new_p0, const Dot &new_p1)
        : p0(new_p0), p1(new_p1) {
        ASSERT(p0.x <= p1.x && p1.y <= p0.y, "bad points");
    }

    // некоторая позиция
    [[nodiscard]] Dot get_pos() const override {
        return p0;
    }

    // находится ли точка внутри коллизии
    [[nodiscard]] bool trigger(const Dot &pos) const override {
        return is_between(p0.x, pos.x, p1.x) && is_between(p1.y, pos.y, p0.y);
    }

    [[nodiscard]] bool trigger(const CollisionCircle &other) const {
        const efloat radius = other.circle.radius;
        const Dot pos = other.circle.pos;
        return CollisionBox(p0 + Dot(-radius, 0), p1 + Dot(radius, 0)).trigger(pos) ||
               CollisionBox(p0 + Dot(0, radius), p1 + Dot(0, -radius)).trigger(pos) ||
               CollisionCircle(p0, radius).trigger(pos) ||
               CollisionCircle(p1, radius).trigger(pos) ||
               CollisionCircle(Dot(p0.x, p1.y), radius).trigger(pos) ||
               CollisionCircle(Dot(p1.x, p0.y), radius).trigger(pos);
    }

    [[nodiscard]] bool trigger(const CollisionBox &other) const {
        return !(p1.x <= other.p0.x || p1.y >= other.p0.y || p0.x >= other.p1.x || p0.y <= other.p1.y);
    }

    [[nodiscard]] bool trigger(const AbstractCollision &other) const override;

    // выталкивает pos за пределы коллизии
    [[nodiscard]] Dot bubble(Dot pos) const override {
        if (!trigger(pos)) {
            return pos;
        }

        efloat min_shift = std::min({
            std::abs(p0.x - pos.x),
            std::abs(p1.x - pos.x),
            std::abs(p0.y - pos.y),
            std::abs(p1.y - pos.y),
        });

        if (min_shift == std::abs(p0.x - pos.x)) {
            pos.x = p0.x;
        } else if (min_shift == std::abs(p1.x - pos.x)) {
            pos.x = p1.x;
        } else if (min_shift == std::abs(p0.y - pos.y)) {
            pos.y = p0.y;
        } else if (min_shift == std::abs(p1.y - pos.y)) {
            pos.y = p1.y;
        } else {
            ASSERT(false, "min_shift should be equal some difference");
        }
        return pos;
    }

    [[nodiscard]] std::unique_ptr<AbstractCollision> bubble(const AbstractCollision &other) const override {
        if (dynamic_cast<const CollisionCircle *>(&other) != nullptr) {
            return std::make_unique<CollisionCircle>(bubble(*dynamic_cast<const CollisionCircle *>(&other)));
        } else if (dynamic_cast<const CollisionBox *>(&other) != nullptr) {
            return std::make_unique<CollisionBox>(bubble(*dynamic_cast<const CollisionBox *>(&other)));
        } else {
            ASSERT(false, "who is other?");
        }
    }

    [[nodiscard]] CollisionCircle bubble(const CollisionCircle &other) const {
        if (!trigger(other)) {
            return other;
        }

        const efloat radius = other.circle.radius;
        Dot pos = other.circle.pos;

        CollisionBox box1(p0 + Dot(-radius, 0), p1 + Dot(radius, 0));
        CollisionBox box2(p0 + Dot(0, radius), p1 + Dot(0, -radius));
        CollisionCircle circle1(p0, radius);
        CollisionCircle circle2(p1, radius);
        CollisionCircle circle3(Dot(p0.x, p1.y), radius);
        CollisionCircle circle4(Dot(p1.x, p0.y), radius);

        // TODO: это не совсем корректно, в некоторых особо почти невозможных случаях

        pos = box1.bubble(pos);
        pos = box2.bubble(pos);
        pos = circle1.bubble(pos);
        pos = circle2.bubble(pos);
        pos = circle3.bubble(pos);
        pos = circle4.bubble(pos);

        return {pos, radius};
    }

    [[nodiscard]] CollisionBox bubble(CollisionBox other) const {
        efloat shift1_right = std::max(0.0, p1.x - other.p0.x);
        efloat shift2_bottom = std::max(0.0, other.p0.y - p1.y);
        efloat shift3_left = std::max(0.0, other.p1.x - p0.x);
        efloat shift4_top = std::max(0.0, p0.y - other.p1.y);

        efloat min_shift = std::min({shift1_right, shift2_bottom, shift3_left, shift4_top});

        if (min_shift == shift1_right) {
            other.p0 += Dot(shift1_right, 0);
            other.p1 += Dot(shift1_right, 0);
        } else if (min_shift == shift2_bottom) {
            other.p0 += Dot(0, -shift2_bottom);
            other.p1 += Dot(0, -shift2_bottom);
        } else if (min_shift == shift3_left) {
            other.p0 += Dot(-shift3_left, 0);
            other.p1 += Dot(-shift3_left, 0);
        } else if (min_shift == shift4_top) {
            other.p0 += Dot(0, shift4_top);
            other.p1 += Dot(0, shift4_top);
        } else {
            ASSERT(false, "min_shift should be equal shiftX");
        }
        return other;
    }
};

[[nodiscard]] bool CollisionCircle::trigger(const CollisionBox &other) const {
    return other.trigger(*this);
}

[[nodiscard]] std::unique_ptr<AbstractCollision> CollisionCircle::bubble(const AbstractCollision &other) const {
    if (dynamic_cast<const CollisionCircle *>(&other) != nullptr) {
        return std::make_unique<CollisionCircle>(bubble(*dynamic_cast<const CollisionCircle *>(&other)));
    } else if (dynamic_cast<const CollisionBox *>(&other) != nullptr) {
        return std::make_unique<CollisionBox>(bubble(*dynamic_cast<const CollisionBox *>(&other)));
    } else {
        ASSERT(false, "who is other?");
    }
}

[[nodiscard]] CollisionBox CollisionCircle::bubble(const CollisionBox &other) const {
    Dot diff = other.bubble(*this).get_pos() - get_pos();
    return {other.p0 - diff, other.p1 - diff};
}

[[nodiscard]] bool CollisionCircle::trigger(const AbstractCollision &other) const {
    if (dynamic_cast<const CollisionCircle *>(&other) != nullptr) {
        return trigger(dynamic_cast<const CollisionCircle &>(other));
    } else if (dynamic_cast<const CollisionBox *>(&other) != nullptr) {
        return trigger(dynamic_cast<const CollisionBox &>(other));
    } else {
        ASSERT(false, "who is other?");
    }
}

[[nodiscard]] bool CollisionBox::trigger(const AbstractCollision &other) const {
    if (dynamic_cast<const CollisionCircle *>(&other) != nullptr) {
        return trigger(dynamic_cast<const CollisionCircle &>(other));
    } else if (dynamic_cast<const CollisionBox *>(&other) != nullptr) {
        return trigger(dynamic_cast<const CollisionBox &>(other));
    } else {
        ASSERT(false, "who is other?");
    }
}

#endif  // GAME_ENGINE_COLLISION_HPP