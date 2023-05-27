#ifndef GAME_LOOT_HEART_HPP
#define GAME_LOOT_HEART_HPP

#include "loot.hpp"
#include "../../render.hpp"

struct Heart : Loot {
    ADD_BYTE_SERIALIZATION();

    Heart() = default;

    Heart(Dot position, Dot dir)
        : Loot(position, dir) {
        size = 0.4;
    }

    bool simulate_collection() override {
        for (auto &player : Players) {
            if (collection_trigger(player.pos)) {
                player.hp++;
                return true;
            }
        }
        return false;
    }

    void draw() const override {
        draw_sprite(pos + Dot(10, 10) * 0.25 - Dot(4.25, 0.5), size, SP_HEART);
        draw_collision(*this);
    }
};

std::vector<Heart> Loot_hearts;

struct Coin : Loot {
    ADD_BYTE_SERIALIZATION();

    Coin() = default;

    Coin(Dot position, Dot dir)
        : Loot(position, dir) {
        size = 0.4;
    }

    bool simulate_collection() override {
        for (auto &player : Players) {
            if (collection_trigger(player.pos)) {
                player.coins++;
                return true;
            }
        }
        return false;
    }

    void draw() const override {
        draw_sprite(pos + Dot(10, 10) * 0.25 - Dot(4.25, 0.5), size, SP_COIN);
        draw_collision(*this);
    }
};

std::vector<Coin> Loot_coins;

#endif  // GAME_LOOT_HEART_HPP