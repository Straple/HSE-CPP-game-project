#ifndef ABSTRACT_GAME_OBJECT_HPP
#define ABSTRACT_GAME_OBJECT_HPP

#include "objects.hpp"
#include "utils.hpp"

struct abstract_game_object {
    // physics
    Dot pos;
    Dot dp;
    efloat collision_radius = 0;

    // visibility
    efloat size = 0;
    Dot delta_draw_pos;

    [[nodiscard]] virtual std::unique_ptr<Collision> get_collision() const = 0;

    // выталкивает игрока за пределы коллизии
    void push_out_of_collision(const Collision &collision) {
        pos += collision.bubble(*get_collision())->get_pos() - get_collision()->get_pos();
    }

    virtual void draw() const = 0;

    virtual ~abstract_game_object() = default;
};

#endif  // ABSTRACT_GAME_OBJECT_HPP