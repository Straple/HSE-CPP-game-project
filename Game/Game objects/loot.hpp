#ifndef GAME_LOOT_HPP
#define GAME_LOOT_HPP

#include "render.hpp"
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

    [[nodiscard]] bool collection_trigger(Dot player_pos) const {
        return get_collision().trigger(player_pos);
    }

    virtual bool simulate_collection() = 0;

    void draw() const override = 0;

    void simulate(efloat delta_time, Dot player_pos) {
        if ((player_pos - pos).get_len() <= magnet_radius) {
            Dot ddp = (player_pos - pos) * 100;
            move_to2d(pos, player_pos, dp, ddp, delta_time);
        } else {
            simulate_move2d(pos, dp, Dot(), delta_time);
        }
    }
};



#endif  // GAME_LOOT_HPP