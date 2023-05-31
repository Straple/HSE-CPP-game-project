#ifndef GAME_LOOT_HPP
#define GAME_LOOT_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"
#include "player.hpp"

struct Loot : AbstractPhysicalObject {
    // visible
    inline const static efloat size = 0.4;
    // physics
    inline const static efloat magnet_radius = 30;
    inline const static efloat collision_radius = 4;

    Loot() = default;

    Loot(Dot position, Dot dir) {
        pos = position;
        dp = dir * 200;
    }

    // коллизия подбора предмета
    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override {
        return std::make_unique<CollisionCircle>(pos, collision_radius);
    }

    [[nodiscard]] bool collection_trigger(Dot player_pos) const {
        return get_collision()->trigger(player_pos);
    }

    virtual bool simulate_collection() = 0;

    void draw() const override = 0;

    void simulate(efloat delta_time) {
        int index = find_nearest_player(pos);

        if (index == -1) {
            // Нет игроков
            return;
        }
        Dot to = Players[index].pos;

        if ((to - pos).get_len() <= magnet_radius) {
            Dot ddp = (to - pos) * 100;
            simulate_move_to2d(pos, to, dp, ddp, delta_time);
        } else {
            simulate_move2d(pos, dp, Dot(), delta_time);
        }
    }
};

#endif  // GAME_LOOT_HPP