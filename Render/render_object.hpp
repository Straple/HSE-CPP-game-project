#ifndef GAME_ENGINE_RENDER_OBJECT_HPP
#define GAME_ENGINE_RENDER_OBJECT_HPP

#include "../Objects/Sprite/color.hpp"
#include "../Objects/collision.hpp"
#include "../global_variables.hpp"
#include "render_utils.hpp"

void draw_collision(const CollisionCircle &collision, const Color color = Color(0x00ff00, 70));

void draw_collision(const CollisionBox &collision, const Color color = Color(0x00ff00, 70));

void draw_collision(const Collision &collision, const Color color = Color(0x00ff00, 70));

template <typename T>
void draw_collision(const T &object) {
    draw_collision(*object.get_collision());
}

void draw_hitbox(const Collision &collision);

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

#endif  // GAME_ENGINE_RENDER_OBJECT_HPP