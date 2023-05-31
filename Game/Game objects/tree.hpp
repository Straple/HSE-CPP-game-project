#ifndef GAME_TREE_HPP
#define GAME_TREE_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"
#include "effect.hpp"

struct Tree : AbstractPhysicalObject {
    ADD_BYTE_SERIALIZATION();

    // visible
    inline const static efloat size = 1;
    inline const static Dot delta_draw_pos = Dot(-16, 36) * size;
    // physics
    inline const static efloat collision_radius = 5;

    int hp = 200;

    Tree() = default;

    explicit Tree(const Dot &position) {
        pos = position;
    }

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override {
        return std::make_unique<CollisionCircle>(pos, collision_radius);
    }

    void draw() const override {
        draw_sprite(pos + Dot(-16, 6) * size, size, SP_LARGE_SHADOW);
        draw_sprite(pos + delta_draw_pos, size, SP_TREE);

        draw_collision(*this);
        draw_hp(*this);
    }

    void simulate_hit(int damage) {
        add_hit_effect(pos + Dot(-8, 8) * size);
        hp -= damage;
        if (hp <= 0) {
            add_death_effect(pos + Dot(-12, 12) * size);
        }
    }
};

std::vector<Tree> Trees;

#endif  // GAME_TREE_HPP