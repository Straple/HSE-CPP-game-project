#ifndef GAME_LOOT_HPP
#define GAME_LOOT_HPP

#include "abstract_physical_object.hpp"

struct Loot : AbstractPhysicalObject {
    // visible
    inline const static efloat size = 0.4;
    // physics
    inline const static efloat magnet_radius = 30;
    inline const static efloat collision_radius = 4;

    Loot() = default;

    Loot(Dot position, Dot dir);

    // коллизия подбора предмета
    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

    [[nodiscard]] bool collection_trigger(Dot player_pos) const;

    virtual bool simulate_collection() = 0;

    void draw() override = 0;

    void simulate(efloat delta_time);
};

#endif  // GAME_LOOT_HPP