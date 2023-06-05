#include "heart_and_coin.hpp"
#include "game_variables.hpp"


    Heart::Heart(Dot position, Dot dir) : Loot(position, dir) {
    }

    bool Heart::simulate_collection() {
        for (auto &player : game_variables::Players) {
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

    void Heart::draw() const {
        draw_sprite(pos + Dot(-2.5, 2.5), size, SP_HEART);
        draw_collision(*this);
    }




    Coin::Coin(Dot position, Dot dir) : Loot(position, dir) {
    }

    bool Coin::simulate_collection() {
        for (auto &player : game_variables::Players) {
            if (!player.is_dead() && collection_trigger(player.pos)) {
                player.coins++;
                return true;
            }
        }
        return false;
    }

    void Coin::draw() const {
        draw_sprite(pos + Dot(-3, 3), size, SP_COIN);
        draw_collision(*this);
    }
