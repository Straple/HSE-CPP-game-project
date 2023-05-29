#ifndef GAME_LOOT_HEART_AND_COIN_HPP
#define GAME_LOOT_HEART_AND_COIN_HPP

#include "../../render.hpp"
#include "loot.hpp"
struct Heart : Loot {
    ADD_BYTE_SERIALIZATION();

    Heart() = default;

    Heart(Dot position, Dot dir) : Loot(position, dir) {
        size = 0.4;
    }

    bool simulate_collection() override {
        for (auto &player : Players) {

            if (collection_trigger(player.pos)) {
                if (player.hp == 0) {
                    player.reborn();
                    player.set_invulnerable();
                }
                player.hp++;
                
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

//extern std::vector<Heart> Loot_hearts;
struct Coin : Loot {
    ADD_BYTE_SERIALIZATION();

    Coin() = default;

    Coin(Dot position, Dot dir) : Loot(position, dir) {
        size = 0.4;
    }

    bool simulate_collection() override {
        for (auto &player : Players) {
            if (player.is_dead) {
                continue;
            }
            if (collection_trigger(player.pos)) {
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

//extern std::vector<Coin> Loot_coins;

#endif  // GAME_LOOT_HEART_HPP