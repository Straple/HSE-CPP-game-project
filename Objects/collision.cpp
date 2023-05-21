#include "collision.hpp"

CollisionCircle::CollisionCircle(const Circle &circle)
    : circle(circle) {
}

CollisionCircle::CollisionCircle(Dot pos, efloat radius)
    : circle(pos, radius) {
}

Dot CollisionCircle::get_pos() const {
    return circle.pos;
}

bool CollisionCircle::trigger(const Dot &pos) const {
    return circle.isin(pos);
}

bool CollisionCircle::trigger(const CollisionCircle &other) const {
    return Circle(circle.pos, circle.radius + other.circle.radius).isin(other.circle.pos);
}

bool CollisionCircle::trigger(const CollisionBox &other) const {
    return other.trigger(*this);
}

bool CollisionCircle::trigger(const Collision &other) const {
    if (dynamic_cast<const CollisionCircle *>(&other) != nullptr) {
        return trigger(dynamic_cast<const CollisionCircle &>(other));
    } else if (dynamic_cast<const CollisionBox *>(&other) != nullptr) {
        return trigger(dynamic_cast<const CollisionBox &>(other));
    } else {
        ASSERT(false, "who is other?");
    }
}

Dot CollisionCircle::bubble(Dot pos) const {
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

CollisionCircle CollisionCircle::bubble(const CollisionCircle &other) const {
    return {CollisionCircle(circle.pos, circle.radius + other.circle.radius).bubble(other.circle.pos), other.circle.radius};
}

CollisionBox CollisionCircle::bubble(const CollisionBox &other) const {
    Dot diff = other.bubble(*this).get_pos() - get_pos();
    return {other.p0 - diff, other.p1 - diff};
}

std::unique_ptr<Collision> CollisionCircle::bubble(const Collision &other) const {
    if (dynamic_cast<const CollisionCircle *>(&other) != nullptr) {
        return std::make_unique<CollisionCircle>(bubble(*dynamic_cast<const CollisionCircle *>(&other)));
    } else if (dynamic_cast<const CollisionBox *>(&other) != nullptr) {
        return std::make_unique<CollisionBox>(bubble(*dynamic_cast<const CollisionBox *>(&other)));
    } else {
        ASSERT(false, "who is other?");
    }
}

//================//
//==CollisionBox==//
//================//

CollisionBox::CollisionBox(const Dot &new_p0, const Dot &new_p1)
    : p0(new_p0), p1(new_p1) {
    ASSERT(p0.x <= p1.x && p1.y <= p0.y, "bad points");
}

Dot CollisionBox::get_pos() const {
    return p0;
}

// находится ли точка внутри коллизии
bool CollisionBox::trigger(const Dot &pos) const {
    return is_between(p0.x, pos.x, p1.x) && is_between(p1.y, pos.y, p0.y);
}

bool CollisionBox::trigger(const CollisionCircle &other) const {
    const efloat radius = other.circle.radius;
    const Dot pos = other.circle.pos;
    return CollisionBox(p0 + Dot(-radius, 0), p1 + Dot(radius, 0)).trigger(pos) ||
           CollisionBox(p0 + Dot(0, radius), p1 + Dot(0, -radius)).trigger(pos) ||
           CollisionCircle(p0, radius).trigger(pos) ||
           CollisionCircle(p1, radius).trigger(pos) ||
           CollisionCircle(Dot(p0.x, p1.y), radius).trigger(pos) ||
           CollisionCircle(Dot(p1.x, p0.y), radius).trigger(pos);
}

bool CollisionBox::trigger(const CollisionBox &other) const {
    return !(p1.x <= other.p0.x || p1.y >= other.p0.y || p0.x >= other.p1.x || p0.y <= other.p1.y);
}

[[nodiscard]] bool CollisionBox::trigger(const Collision &other) const {
    if (dynamic_cast<const CollisionCircle *>(&other) != nullptr) {
        return trigger(dynamic_cast<const CollisionCircle &>(other));
    } else if (dynamic_cast<const CollisionBox *>(&other) != nullptr) {
        return trigger(dynamic_cast<const CollisionBox &>(other));
    } else {
        ASSERT(false, "who is other?");
    }
}

Dot CollisionBox::bubble(Dot pos) const {
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

CollisionCircle CollisionBox::bubble(const CollisionCircle &other) const {
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

CollisionBox CollisionBox::bubble(CollisionBox other) const {
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

std::unique_ptr<Collision> CollisionBox::bubble(const Collision &other) const {
    if (dynamic_cast<const CollisionCircle *>(&other) != nullptr) {
        return std::make_unique<CollisionCircle>(bubble(*dynamic_cast<const CollisionCircle *>(&other)));
    } else if (dynamic_cast<const CollisionBox *>(&other) != nullptr) {
        return std::make_unique<CollisionBox>(bubble(*dynamic_cast<const CollisionBox *>(&other)));
    } else {
        ASSERT(false, "who is other?");
    }
}