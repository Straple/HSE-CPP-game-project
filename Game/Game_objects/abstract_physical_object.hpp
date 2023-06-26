#ifndef ABSTRACT_GAME_OBJECT_HPP
#define ABSTRACT_GAME_OBJECT_HPP

#include "../../objects.hpp"
#include "../../utils.hpp"
#include "abstract_object.hpp"

struct AbstractPhysicalObject : AbstractObject {
    Dot dp;

    AbstractPhysicalObject() = default;

    AbstractPhysicalObject(Dot pos);

    [[nodiscard]] virtual std::unique_ptr<Collision> get_collision() const = 0;

    // жестко выталкивает за пределы коллизии
    // это нужно, чтобы прям запретить там быть
    void push_out_of_collision_hard(const Collision &collision);

    // мягко выталкивает за пределы коллизии
    // это нужно, чтобы например слаймы в стае могли прижиматься друг к другу
    // выглядит красиво
    void
    push_out_of_collision_soft(const Collision &collision, efloat delta_time);

    virtual ~AbstractPhysicalObject() = default;
};

#endif  // ABSTRACT_GAME_OBJECT_HPP