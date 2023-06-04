#ifndef GAME_LOOT_HEART_AND_COIN_HPP
#define GAME_LOOT_HEART_AND_COIN_HPP

#include "loot.hpp"

struct Heart : Loot {
    ADD_BYTE_SERIALIZATION();

    Heart() = default;

    Heart(Dot position, Dot dir);

    bool simulate_collection() override;

    void draw() const override;
};

struct Coin : Loot {
    ADD_BYTE_SERIALIZATION();

    Coin() = default;

    Coin(Dot position, Dot dir);

    bool simulate_collection() override;

    void draw() const override;
};

static std::vector<Coin> Loot_coins;
static std::vector<Heart> Loot_hearts;

#endif  // GAME_LOOT_HEART_AND_COIN_HPP