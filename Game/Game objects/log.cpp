
#include "abstract_game_object.h"

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

        draw_collision_obj(*this);

        draw_rect(pos - camera.pos, Dot(1, 1) * 0.3, RED);
    }

    void simulate_hit(const Player &player) {
        add_hit_effect(pos + Dot(-10, 8) * size);

        dp += player.get_dir() * 200;
    }

    void simulate(efloat delta_time) {
        Dot ddp;
        simulate_move2d(pos, dp, ddp, delta_time);
    }
};

std::vector<Log> Logs;
