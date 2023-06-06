#ifndef GAME_TREE_HPP
#define GAME_TREE_HPP

#include "abstract_physical_object.hpp"

struct Tree : AbstractPhysicalObject {
    ADD_BYTE_SERIALIZATION();

    // visible
    inline const static efloat size = 1;
    inline const static Dot delta_draw_pos = Dot(-16, 36) * size;
    // physics
    inline const static efloat collision_radius = 12;

    int hp = 200;

    Tree() = default;

    explicit Tree(const Dot &position);

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

    void draw() const override;

    void simulate_hit(int damage);

    bool trigger_in_draw_sprite(const Dot &check_pos);
};

#endif  // GAME_TREE_HPP