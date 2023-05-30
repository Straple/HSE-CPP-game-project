#ifndef GAME_TREE_HPP
#define GAME_TREE_HPP

#include "../../render.hpp"
#include "abstract_game_object.hpp"
#include "effect.hpp"

struct Tree : AbstractGameObject {
    ADD_BYTE_SERIALIZATION();

    int hp = 200;

    Tree() = default;

    explicit Tree(const Dot &new_pos) {
        size = 1;
        delta_draw_pos = Dot(-16, 36) * size;
        collision_radius = 5;
        pos = new_pos - delta_draw_pos;
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