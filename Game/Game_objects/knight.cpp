#include "knight.hpp"

Knight::Knight(const Dot &position) {
    pos = position;
}

[[nodiscard]] std::unique_ptr<Collision> Knight::get_collision() const {
    return std::make_unique<CollisionCircle>(pos, 7);
}

void Knight::draw() {
    draw_sprite(pos + Dot(-8, 4), size, SP_MEDIUM_SHADOW);
    draw_sprite(pos + delta_draw_pos, size, SP_KNIGHT);

    draw_collision(*this);
}

bool Knight::trigger_in_draw_sprite(const Dot &check_pos) {
    return collision_in_draw_sprite(
        pos + delta_draw_pos, size, SP_KNIGHT, check_pos
    );
}
