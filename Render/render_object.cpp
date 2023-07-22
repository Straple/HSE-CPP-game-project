#include "render_object.hpp"
#include "render_geometry.hpp"

void draw_collision(const CollisionCircle &collision, const Color color) {
    if (global_variables::debug_mode) {
        Circle circle = collision.circle;
        static_pos_update(circle.pos, global_variables::camera_is_static);
        draw_circle(circle, color);
    }
}

void draw_collision(const CollisionBox &collision, const Color color) {
    if (global_variables::debug_mode) {
        Dot p0 = collision.p0;
        Dot p1 = collision.p1;
        static_pos_update(p0, global_variables::camera_is_static);
        static_pos_update(p1, global_variables::camera_is_static);
        draw_rect2(p0, p1, color);
    }
}

void draw_collision(const Collision &collision, const Color color) {
    if (dynamic_cast<const CollisionCircle *>(&collision) != nullptr) {
        draw_collision(*dynamic_cast<const CollisionCircle *>(&collision), color);
    } else if (dynamic_cast<const CollisionBox *>(&collision) != nullptr) {
        draw_collision(*dynamic_cast<const CollisionBox *>(&collision), color);
    } else {
        ASSERT(false, "who is this Collision?");
    }
}

void draw_hitbox(const Collision &collision) {
    draw_collision(collision, Color(0xff0000, 70));
}