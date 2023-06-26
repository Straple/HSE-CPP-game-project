#include "abstract_physical_object.hpp"

AbstractPhysicalObject::AbstractPhysicalObject(Dot pos) : AbstractObject(pos) {
}

void AbstractPhysicalObject::push_out_of_collision_hard(
    const Collision &collision
) {
    Dot need_delta_pos =
        (collision.bubble(*get_collision())->get_pos() -
         get_collision()->get_pos());
    pos += need_delta_pos;
}

void AbstractPhysicalObject::push_out_of_collision_soft(
    const Collision &collision,
    efloat delta_time
) {
    Dot need_delta_pos =
        (collision.bubble(*get_collision())->get_pos() -
         get_collision()->get_pos());
    dp += delta_time * need_delta_pos * 100;
}
