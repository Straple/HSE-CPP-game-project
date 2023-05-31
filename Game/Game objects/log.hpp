#ifndef GAME_LOG_HPP
#define GAME_LOG_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"

struct Log : AbstractPhysicalObject {
    ADD_BYTE_SERIALIZATION();

    // visible
    inline const static efloat size = 0.3;
    inline const static Dot delta_draw_pos = Dot(-30, 18) * size;
    // physics
    inline const static efloat collision_radius = 4;

    Log() = default;

    explicit Log(const Dot &position) {
        pos = position;
    }

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override {
        return std::make_unique<CollisionCircle>(pos, collision_radius);
    }

    void draw() const override {
        draw_sprite(pos + delta_draw_pos, size, SP_LOG);
        draw_collision(*this);
    }

    void simulate(efloat delta_time) {
        simulate_move2d(pos, dp, Dot(), delta_time);
    }
};

std::vector<Log> Logs;

#endif  // GAME_LOG_HPP