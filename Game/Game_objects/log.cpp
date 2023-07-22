#include "log.hpp"
//
#include "../../render.hpp"

Log::Log(const Dot &position) {
    pos = position;
}

[[nodiscard]] std::unique_ptr<Collision> Log::get_collision() const {
    return std::make_unique<CollisionCircle>(pos, collision_radius);
}

void Log::draw() {
    draw_sprite(pos + delta_draw_pos, size, SP_LOG);
    draw_collision(*this);
}

void Log::simulate(efloat delta_time) {
    simulate_move2d(pos, dp, Dot(), delta_time);
}
