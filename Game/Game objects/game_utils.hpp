#ifndef GAME_UTILS_HPP
#define GAME_UTILS_HPP

#include "../../render.hpp"
#include "../../global_variables.hpp"

void draw_collision(const collision_circle &coll) {
    if (global_variables::debug_mode) {
        Circle circle = coll.circle;
        static_pos_update(circle.pos, global_variables::camera_is_static);

        draw_circle(circle, Color(0xffffff, 128));
    }
}

void draw_collision(const collision_box &coll) {
    if (global_variables::debug_mode) {
        Dot p0 = coll.p0;
        Dot p1 = coll.p1;

        std::swap(p0.y, p1.y);

        static_pos_update(p0, global_variables::camera_is_static);
        static_pos_update(p1, global_variables::camera_is_static);

        draw_rect2(p0, p1, Color(0xffffff, 128));
    }
}

template <typename T>
void draw_collision_obj(const T &object) {
    draw_collision(object.get_collision());
}

template <typename T>
void draw_hp(const T &object) {
    if (global_variables::debug_mode) {
        Dot p = object.pos;
        static_pos_update(p, global_variables::camera_is_static);
        draw_object(object.hp, p, 0.3, Color(0x0000ff, 0xff));
    }
}

#endif  // GAME_UTILS_HPP