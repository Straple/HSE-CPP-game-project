#include "render_utils.hpp"
#include "../global_variables.hpp"

void static_pos_update(Dot &pos, bool is_static) {
    if (!is_static) {
        pos -= global_variables::camera.pos;
        /*efloat x = global_variables::camera.pos.x;
        x *= global_variables::scale_factor;
        x = int(x);
        x /= global_variables::scale_factor;

        efloat y = global_variables::camera.pos.y;
        y *= global_variables::scale_factor;
        y = int(y);
        y /= global_variables::scale_factor;

        pos -= Dot(x, y);*/
    }
}

void static_pos_update(Dot &pos) {
    static_pos_update(pos, global_variables::camera_is_static);
}

// проверяет пересечение экрана и этого прямоугольника
bool arena_query(efloat left, efloat right, efloat top, efloat bottom) {
    return !(
        right < -global_variables::arena_half_size.x || global_variables::arena_half_size.x < left ||  // x
        top < -global_variables::arena_half_size.y || global_variables::arena_half_size.y < bottom     // y
    );
}