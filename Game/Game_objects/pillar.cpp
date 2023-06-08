#include "pillar.hpp"

Pillar::Pillar(const Dot &position) {
    pos = position;
}

[[nodiscard]] std::unique_ptr<Collision> Pillar::get_collision() const {
    return std::make_unique<CollisionBox>(pos + Dot(-10, 8), pos + Dot(10, -8));
}

void Pillar::draw() {
    draw_rect2(
        pos + Dot(-14, 8) - global_variables::camera.pos,
        pos + Dot(14, -12) - global_variables::camera.pos,
        Color(0, 128)
    );
    draw_sprite(pos + delta_draw_pos, size, SP_PILLAR);

    draw_collision(*this);
    // draw_rect(pos - global_variables::camera.pos, Dot(0.5, 0.5), RED);
}

bool Pillar::trigger_in_draw_sprite(const Dot &check_pos) {
    return collision_in_draw_sprite(pos + delta_draw_pos, size, SP_PILLAR, check_pos);
}