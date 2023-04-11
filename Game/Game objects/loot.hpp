#ifndef GAME_LOOT_HPP
#define GAME_LOOT_HPP

#include "../../render.hpp"
#include "abstract_game_object.hpp"
#include "player.hpp"

struct Loot : abstract_game_object {
    efloat magnet_radius = 30;

    Loot() {
    }

    Loot(Dot position, Dot dir) {
        pos = position;
        dp = dir * 200;

        collision_radius = 5;
        size = 1;
        delta_draw_pos = Dot(10, 10) * size;
    }

    // коллизия подбора предмета
    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }

    [[nodiscard]] bool collection_trigger() const {
        return get_collision().trigger(Players[0].pos);
    }

    virtual bool simulate_collection() = 0;

    void draw() const override = 0;

    void simulate(efloat delta_time) {
        if ((Players[0].pos - pos).get_len() <= magnet_radius) {
            Dot ddp = (Players[0].pos - pos) * 100;
            move_to2d(pos, Players[0].pos, dp, ddp, delta_time);
        } else {
            simulate_move2d(pos, dp, Dot(), delta_time);
        }
    }
};

#endif  // GAME_LOOT_HPP