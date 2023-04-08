#ifndef GAME_TREE_HPP
#define GAME_TREE_HPP

#include "abstract_game_object.hpp"
#include "effect.hpp"
#include "render.hpp"

struct Tree : abstract_game_object {
    int hp = 200;

    Tree() = default;

    explicit Tree(const Dot &new_pos) {
        size = 0.5;
        delta_draw_pos = Dot(-16, 36) * size;
        collision_radius = 5;
        pos = new_pos - delta_draw_pos;
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }

    void draw() const override {
        draw_sprite(pos + Dot(-16, 6) * size, size, SP_LARGE_SHADOW);
        draw_sprite(pos + delta_draw_pos, size, SP_TREE);

        // draw_collision_obj(*this);
        // draw_hp(*this);
    }

    void simulate_hit(int player_damage) {
        add_hit_effect(pos + Dot(-8, 8) * size);

        hp -= player_damage;

        if (hp <= 0) {
            add_death_effect(pos + Dot(-12, 12) * size);
        }
    }
};

#endif  // GAME_TREE_HPP