#include "bush.hpp"

Bush::Bush(const Dot &position) {
    pos = position;
}

[[nodiscard]] std::unique_ptr<Collision> Bush::get_collision() const {
    return std::make_unique<CollisionCircle>(pos, collision_radius);
}

void Bush::draw() const {
    draw_sprite(pos + delta_draw_pos + Dot(0, -13) * size, size, SP_LARGE_SHADOW);
    draw_sprite(pos + delta_draw_pos, size, SP_BUSH);

    draw_collision(*this);
}

bool Bush::trigger_in_draw_sprite(const Dot &check_pos) {
    return collision_in_draw_sprite(pos + delta_draw_pos + Dot(0, -13) * size, size, SP_LARGE_SHADOW, check_pos) ||
           collision_in_draw_sprite(pos + delta_draw_pos, size, SP_BUSH, check_pos);
}