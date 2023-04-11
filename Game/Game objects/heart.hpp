#ifndef GAME_LOOT_HEART_HPP
#define GAME_LOOT_HEART_HPP

#include "loot.hpp"
#include "../../render.hpp"

struct Heart : Loot {
    Heart(Dot position, Dot dir) : Loot(position, dir) {
        size = 0.3;
    }

    bool simulate_collection() override {
        if (collection_trigger()) {
            Players[0].hp++;
            return true;
        }
        else {
            return false;
        }
    }

    void draw() const override {
        draw_sprite(
            pos + Dot(10, 10) * 0.25 - Dot(4.25, 0.5), size, SP_HEART
        );
        draw_collision(*this);
    }
};

std::vector<Heart> Loot_hearts;

#endif  // GAME_LOOT_HEART_HPP