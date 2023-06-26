#ifndef GAME_ENGINE_RENDER_UTILS_HPP
#define GAME_ENGINE_RENDER_UTILS_HPP

#include "../Objects/Geometry/dot.hpp"
#include "../Objects/Sprite/color.hpp"

void static_pos_update(Dot &pos, bool is_static);

void static_pos_update(Dot &pos);

// проверяет пересечение экрана и этого прямоугольника
bool arena_query(efloat left, efloat right, efloat top, efloat bottom);

struct dummy_color_func {
  inline Color operator()(const Color &color) const { return color; }
};

template <uint8_t alpha> struct alpha_color_func {
  inline Color operator()(const Color &color) const {
    return {color.r, color.g, color.b, alpha};
  }
};

using shadow_color_func = alpha_color_func<100>;

#endif // GAME_ENGINE_RENDER_UTILS_HPP