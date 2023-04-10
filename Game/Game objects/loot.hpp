#ifndef GAME_LOOT_HPP
#define GAME_LOOT_HPP

#include "../../render.hpp"
#include "abstract_game_object.hpp"
#include "player.hpp"

struct Loot : abstract_game_object {
    efloat magnet_radius = 30;
    Dot initial_pos;
    bool initial_flight = true;

    Loot(Dot position = Dot()) {
        initial_pos = pos = position;

        collision_radius = 4;
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
            initial_flight = false;
            Dot ddp = (Players[0].pos - pos).normalize() * 100;
            move_to2d(pos, Players[0].pos, dp, ddp, delta_time);
        } else if (initial_flight) {
            if (pos == initial_pos + Dot(2, 2)) {
                initial_flight = false;
            } else {
                move_to2d(
                    pos, initial_pos + Dot(4, 4), dp,
                    (initial_pos + Dot(2, 2)) * 2, delta_time
                );
            }
        }
    }
};

std::vector<Loot> Loot_objects;

#endif  // GAME_LOOT_HPP