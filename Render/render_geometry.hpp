#ifndef GAME_ENGINE_RENDER_GEOMETRY_HPP
#define GAME_ENGINE_RENDER_GEOMETRY_HPP

#include "../Objects/Geometry/geometry.hpp"
#include "../Objects/Sprite/color.hpp"

void draw_rect(Dot pos, Dot half_size, const Color &color);

void draw_rect2(Dot top_left, Dot bottom_right, const Color &color);

void draw_circle(const Circle &circle, const Color &color);

void draw_line(const Line &line, efloat half_size, const Color &color);

void draw_rhombus(Dot pos, Dot half_size, const Color &color);

#endif  // GAME_ENGINE_RENDER_GEOMETRY_HPP
