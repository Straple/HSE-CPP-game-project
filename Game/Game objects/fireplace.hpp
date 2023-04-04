#ifndef GAME_FIREPLACE_HPP
#define GAME_FIREPLACE_HPP

#include "abstract_game_object.hpp"
#include "log.hpp"
#include "render.hpp"

struct Fireplace : abstract_game_object {
    //Fire_machine fire;

    efloat time = 5 * 60;
    efloat cooldown_add_time_accum = 0;

    const int eating_radius = 20;
    const efloat add_time = 1.5 * 60;

    Fireplace() = default;

    explicit Fireplace(const Dot &new_pos) {
        size = 0.4;
        delta_draw_pos = Dot(-42, 10) * size;
        collision_radius = 10;
        pos = new_pos - delta_draw_pos;

        //fire = Fire_machine(pos, 5, 0.5, 0.5, 4, 0.15);
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(
            Circle(pos + Dot(0, -10) * size, collision_radius)
        );
    }

    void draw() const override {
        draw_sprite(
            pos + delta_draw_pos + Dot(10, 0) * size, size * 2, SP_LARGE_SHADOW
        );
        draw_sprite(pos + delta_draw_pos, size, SP_FIREPLACE);

        //fire.draw();

        Dot p = pos + Dot(-10, -30) * size;
        static_pos_update(p);
        draw_object(int(time), p, 0.5, 0xff000000);

        // draw_collision_obj(*this);

        draw_rect(pos - global_variables::camera.pos, Dot(1, 1) * 0.3, RED);
    }

    void simulate(efloat delta_time, std::vector<Log> &Logs) {
        time -= delta_time;

        cooldown_add_time_accum += delta_time;

        if (cooldown_add_time_accum > 1) {
            //fire.cooldown_add += 0.0005;
            cooldown_add_time_accum = 0;
        }

        //fire.simulate(delta_time);

        for (int i = 0; i < Logs.size(); i++) {
            if ((pos - Logs[i].pos).get_len() <= eating_radius) {
                Logs.erase(Logs.begin() + i);
                i--;

                time += add_time;
                //fire.cooldown_add = std::max(0.3, fire.cooldown_add - 0.3);
            }
        }
    }
};

#endif  // GAME_FIREPLACE_HPP