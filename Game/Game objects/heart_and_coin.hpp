#ifndef GAME_LOOT_HEART_AND_COIN_HPP
#define GAME_LOOT_HEART_AND_COIN_HPP

#include "loot.hpp"

struct Heart : Loot {
    ADD_BYTE_SERIALIZATION();

    Heart() = default;

    Heart(Dot position, Dot dir) : Loot(position, dir) {
    }

    bool simulate_collection() override {
        for (auto &player : Players) {
            if (collection_trigger(player.pos)) {
                if (player.is_dead()) {
                    player.reborn();
                }
                else{
                    player.hp++;
                }
                return true;
            }
        }
        return false;
    }

    void draw() const override {
        draw_sprite(pos + Dot(-2.5, 2.5), size, SP_HEART);
        draw_collision(*this);
    }
};

struct Coin : Loot {
    ADD_BYTE_SERIALIZATION();

    Coin() = default;

    Coin(Dot position, Dot dir) : Loot(position, dir) {
    }

    bool simulate_collection() override {
        for (auto &player : Players) {
            if (!player.is_dead() && collection_trigger(player.pos)) {
                player.coins++;
                return true;
            }
        }
        return false;
    }

    void draw() const override {
        draw_sprite(pos + Dot(-3, 3), size, SP_COIN);
        draw_collision(*this);
    }
};

#endif  // GAME_LOOT_HEART_AND_COIN_HPP