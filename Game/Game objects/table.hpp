#ifndef GAME_TABLE_HPP
#define GAME_TABLE_HPP

#include "abstract_game_object.hpp"
#include "../../render.hpp"

struct Table : abstract_game_object {
    ADD_BYTE_SERIALIZATION();

    Table() = default;

    explicit Table(const Dot new_pos) {
        size = 1;
        delta_draw_pos = Dot(-10, 30);
        collision_radius = 10;
        pos = new_pos;
    }

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override {
        return std::make_unique<CollisionBox>(pos + delta_draw_pos + Dot(0, -5), pos + Dot(-delta_draw_pos.x, 0));
    }

    void draw() const override {
        // draw shadow
        {
            Dot shadow_p0 = pos + delta_draw_pos + Dot(-1, 0);
            Dot shadow_p1 = pos + Dot(-delta_draw_pos.x, 0) + Dot(1, -10);
            static_pos_update(shadow_p0, global_variables::camera_is_static);
            static_pos_update(shadow_p1, global_variables::camera_is_static);
            draw_rect2(shadow_p0, shadow_p1, Color(0, 128));
        }
        draw_sprite(pos + delta_draw_pos, size, SP_TABLE);

        draw_collision(*this);
    }
};

//std::vector<Table> Tables = {
//    Table(Dot(49, -80)),
//};
std::vector<Table> Tables;

#endif  // GAME_TABLE_HPP