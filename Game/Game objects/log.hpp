#ifndef GAME_LOG_HPP
#define GAME_LOG_HPP

#include "abstract_game_object.hpp"
#include "render.hpp"

struct Log : abstract_game_object {
    Log() = default;

    explicit Log(const Dot &new_pos) {
        size = 0.3;
        delta_draw_pos = Dot(-30, 18) * size;
        collision_radius = 4;
        pos = new_pos - delta_draw_pos;
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }

    void draw() const override {
        draw_sprite(pos + delta_draw_pos, size, SP_LOG);

        draw_rect(pos - global_variables::camera.pos, Dot(1, 1) * 0.3, RED);
    }

    void simulate(efloat delta_time) {
        Dot ddp;
        simulate_move2d(pos, dp, ddp, delta_time);
    }
};

std::vector<Log> Logs;

#endif  // GAME_LOG_HPP