#include "abstract_physical_object.hpp"


// жестко выталкивает за пределы коллизии
// это нужно, чтобы прям запретить там быть
void AbstractPhysicalObject::push_out_of_collision_hard(const Collision &collision) {
    Dot need_delta_pos = (collision.bubble(*get_collision())->get_pos() - get_collision()->get_pos());
    pos += need_delta_pos;
}

// мягко выталкивает за пределы коллизии
// это нужно, чтобы например слаймы в стае могли прижиматься друг к другу
// выглядит красиво
void AbstractPhysicalObject::push_out_of_collision_soft(const Collision &collision, efloat delta_time) {
    Dot need_delta_pos = (collision.bubble(*get_collision())->get_pos() - get_collision()->get_pos());
    dp += delta_time * need_delta_pos * 100;
}

