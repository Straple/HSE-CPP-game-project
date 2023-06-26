#include "barrel.hpp"

Barrel::Barrel(const Dot &position) {
    pos = position;
}

[[nodiscard]] std::unique_ptr<Collision> Barrel::get_collision() const {
    return std::make_unique<CollisionCircle>(pos, 7);
}

void Barrel::draw() {
    draw_sprite(pos + Dot(-10, 2), 1.2, SP_MEDIUM_SHADOW);
    // draw_circle(Circle(pos - global_variables::camera.pos - Dot(), 8), Color(0, 128));
    draw_sprite(pos + delta_draw_pos, size, SP_BARREL);

    draw_collision(*this);
    // draw_rect(pos - global_variables::camera.pos, Dot(0.5, 0.5), RED);
}

bool Barrel::trigger_in_draw_sprite(const Dot &check_pos) {
    return collision_in_draw_sprite(pos + delta_draw_pos, size, SP_BARREL, check_pos);
}
