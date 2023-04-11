#ifndef GAME_UTILS_HPP
#define GAME_UTILS_HPP

#include "../../global_variables.hpp"
#include "../../render.hpp"

template <typename T>
void draw_collision(const T &object) {
    if (global_variables::debug_mode) {
        Circle circle = object.get_collision().circle;
        circle.pos -= global_variables::camera.pos;
        draw_circle(circle, Color(0xffffff, 50));
    }
}

template <typename T>
void draw_hitbox(const T &object) {
    if (global_variables::debug_mode) {
        Circle circle = object.get_hitbox().circle;
        circle.pos -= global_variables::camera.pos;
        draw_circle(circle, Color(0xff0000, 50));
    }
}

/*void draw_collision(const collision_box &coll) {
    if (global_variables::debug_mode) {
        Dot p0 = coll.p0;
        Dot p1 = coll.p1;

        std::swap(p0.y, p1.y);

        static_pos_update(p0, global_variables::camera_is_static);
        static_pos_update(p1, global_variables::camera_is_static);

        draw_rect2(p0, p1, Color(0xffffff, 128));
    }
}*/

template <typename T>
void draw_hp(const T &object) {
    if (global_variables::debug_mode) {
        Dot p = object.pos;
        static_pos_update(p, global_variables::camera_is_static);
        draw_object(object.hp, p, 0.3, Color(0x0000ff, 0xff));
    }
}

#endif  // GAME_UTILS_HPP