#ifndef ABSTRACT_GAME_OBJECT
#define ABSTRACT_GAME_OBJECT

#include "..\..\Objects\objects.h"
#include "..\..\utils.h"

struct abstract_game_object {
    // physics
    Dot pos;
    Dot dp;
    efloat collision_radius = 0;

    // visibility
    efloat size = 0;
    Dot delta_draw_pos;

    [[nodiscard]] virtual collision_circle get_collision() const = 0;

    virtual void draw() const = 0;

    virtual ~abstract_game_object() = default;
};

#endif  // ABSTRACT_GAME_OBJECT