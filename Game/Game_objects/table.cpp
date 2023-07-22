#include "table.hpp"
//
#include "../../render.hpp"
#include "../../sprites.hpp"

Table::Table(const Dot new_pos) {
    pos = new_pos;
}

[[nodiscard]] std::unique_ptr<Collision> Table::get_collision() const {
    return std::make_unique<CollisionBox>(pos + delta_draw_pos + Dot(0, -5), pos + Dot(-delta_draw_pos.x, 0));
}

void Table::draw() {
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

bool Table::trigger_in_draw_sprite(const Dot &check_pos) {
    return collision_in_draw_sprite(pos + delta_draw_pos, size, SP_TABLE, check_pos);
}