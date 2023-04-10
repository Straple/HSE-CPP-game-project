#ifndef GAME_LOOT_HEART_HPP
#define GAME_LOOT_HEART_HPP

#define HEART_DELTA_DRAW_DOT Dot(10, 10)

#include "loot.hpp"
#include "../../render.hpp"

struct Heart : Loot {
    Heart(Dot position) : Loot(position) {
    }

    bool simulate_collection() override {
        if (collection_trigger()) {
            Players[0].hp += 50;
            return true;
        }
        else {
            return false;
        }
    }

    void draw() const override {
        draw_sprite(
            pos + HEART_DELTA_DRAW_DOT * 0.25 - Dot(4.25, 0.5), size, SP_HEART
        );
        draw_collision_obj(*this);
    }
};

#endif  // GAME_LOOT_HEART_HPP