#include "nun_statue.hpp"

NunStatue::NunStatue(const Dot &position) {
    pos = position;
}

[[nodiscard]] std::unique_ptr<Collision> NunStatue::get_collision() const {
    return std::make_unique<CollisionBox>(pos + Dot(-12, 8), pos + Dot(12, -6));
}

void NunStatue::draw() {
    draw_rect2(
        pos + Dot(-14, 8) - global_variables::camera.pos,
        pos + Dot(14, -10) - global_variables::camera.pos, Color(0, 128)
    );
    draw_sprite(pos + delta_draw_pos, size, SP_NUN_STATUE);

    draw_collision(*this);
}

bool NunStatue::trigger_in_draw_sprite(const Dot &check_pos) {
    return collision_in_draw_sprite(
        pos + delta_draw_pos, size, SP_NUN_STATUE, check_pos
    );
}
