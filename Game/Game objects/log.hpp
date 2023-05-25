#ifndef GAME_LOG_HPP
#define GAME_LOG_HPP

#include "abstract_game_object.hpp"
#include "render.hpp"

struct Log : abstract_game_object {
    ADD_BYTE_SERIALIZATION();

    Log() = default;

    explicit Log(const Dot &new_pos) {
        size = 0.3;
        delta_draw_pos = Dot(-30, 18) * size;
        collision_radius = 4;
        pos = new_pos - delta_draw_pos;
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