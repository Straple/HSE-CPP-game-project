#include "Objects/collision.hpp"

void draw_collision(const CollisionCircle &collision, const Color color = Color(0x00ff00, 70)) {
    if (global_variables::debug_mode) {
        Circle circle = collision.circle;
        static_pos_update(circle.pos, global_variables::camera_is_static);
        draw_circle(circle, color);
    }
}

void draw_collision(const CollisionBox &collision, const Color color = Color(0x00ff00, 70)) {
    if (global_variables::debug_mode) {
        Dot p0 = collision.p0;
        Dot p1 = collision.p1;
        static_pos_update(p0, global_variables::camera_is_static);
        static_pos_update(p1, global_variables::camera_is_static);
        draw_rect2(p0, p1, color);
    }
}

void draw_collision(const AbstractCollision &collision, const Color color = Color(0x00ff00, 70)) {
    if (dynamic_cast<const CollisionCircle *>(&collision) != nullptr) {
        draw_collision(*dynamic_cast<const CollisionCircle *>(&collision), color);
    } else if (dynamic_cast<const CollisionBox *>(&collision) != nullptr) {
        draw_collision(*dynamic_cast<const CollisionBox *>(&collision), color);
    } else {
        ASSERT(false, "who is this AbstractCollision?");
    }
}

template <typename T>
void draw_collision(const T &object) {
    draw_collision(*object.get_collision());
}

void draw_hitbox(const AbstractCollision &collision) {
    draw_collision(collision, Color(0xff0000, 70));
}

template <typename T>
void draw_hitbox(const T &object) {
    draw_hitbox(*object.get_hitbox());
}

template <typename T>
void draw_hp(const T &object) {
    if (global_variables::debug_mode) {
        Dot pos = object.pos;
        static_pos_update(pos, global_variables::camera_is_static);
        draw_object(object.hp, pos, 0.5, Color(0x0000ff, 0xff));
    }
}