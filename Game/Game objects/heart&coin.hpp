#ifndef GAME_LOOT_HEART_HPP
#define GAME_LOOT_HEART_HPP

#include "loot.hpp"
#include "render.hpp"

struct Heart : Loot {
    Heart(Dot position, Dot dir)
        : Loot(position, dir) {
        size = 0.4;
    }

    bool simulate_collection() override {
        if (collection_trigger(Players[0].pos)) {
            Players[0].hp++;
            return true;
        } else {
            return false;
        }
    }

    void draw() const override {
        draw_sprite(pos + Dot(10, 10) * 0.25 - Dot(4.25, 0.5), size, SP_HEART);
        draw_collision(*this);
    }
};

std::vector<Heart> Loot_hearts;

struct Coin : Loot {
    Coin(Dot position, Dot dir)
        : Loot(position, dir) {
        size = 0.4;
    }

    bool simulate_collection() override {
        if (collection_trigger(Players[0].pos)) {
            Players[0].coins++;
            return true;
        } else {
            return false;
        }
    }

    void draw() const override {
        draw_sprite(pos + Dot(10, 10) * 0.25 - Dot(4.25, 0.5), size, SP_COIN);
        draw_collision(*this);
    }
};

std::vector<Coin> Loot_coins;

#endif  // GAME_LOOT_HEART_HPP