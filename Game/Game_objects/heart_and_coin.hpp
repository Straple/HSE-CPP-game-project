#ifndef GAME_LOOT_HEART_AND_COIN_HPP
#define GAME_LOOT_HEART_AND_COIN_HPP

#include "loot.hpp"

struct Heart : Loot {
    ADD_BYTE_SERIALIZATION();

    Heart() = default;

    Heart(Dot position, Dot dir);

    bool simulate_collection() override;

    void draw() override;
};

struct Coin : Loot {
    ADD_BYTE_SERIALIZATION();

    Coin() = default;

    Coin(Dot position, Dot dir);

    bool simulate_collection() override;

    void draw() override;
};

#endif  // GAME_LOOT_HEART_AND_COIN_HPP