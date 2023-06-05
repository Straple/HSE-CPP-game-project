#include "tree.hpp"
//
#include "../../render.hpp"
#include "../../sprites.hpp"
#include "effect.hpp"

Tree::Tree(const Dot &position) {
    pos = position;
}

[[nodiscard]] std::unique_ptr<Collision> Tree::get_collision() const {
    return std::make_unique<CollisionCircle>(pos, collision_radius);
}

void Tree::draw() const {
    draw_sprite(pos + Dot(-16, 6) * size, size, SP_LARGE_SHADOW);
    draw_sprite(pos + delta_draw_pos, size, SP_TREE);

    draw_collision(*this);
    draw_hp(*this);
}

void Tree::simulate_hit(int damage) {
    add_hit_effect(pos + Dot(-8, 8) * size);
    hp -= damage;
    if (hp <= 0) {
        add_death_effect(pos + Dot(-12, 12) * size);
    }
}