#ifndef GAME_ENGINE_BASIC_RENDER_HPP
#define GAME_ENGINE_BASIC_RENDER_HPP

#include <cstdint>
#include "../Objects/Sprite/color.hpp"

#define GAME_ENGINE_USE_SSE

// заполняет значения dest[0], dest[1], ..., dest[len-1] значением val32
void fill(uint32_t *dest, uint32_t val32, unsigned int len);

// рисует в пикселях [x0, x1)*[y0, y1)
// 0 <= x0, x1 <= width
// 0 <= y0, y1 <= height
void draw_pixels(
    uint32_t x0,
    uint32_t y0,
    uint32_t x1,
    uint32_t y1,
    const Color &color
);

// рисует прямоугольник в пикселях с обработкой границ
void draw_rect_in_pixels(
    int64_t x0,
    int64_t y0,
    int64_t x1,
    int64_t y1,
    const Color &color
);

// зарисовывает весь экран цветом
void clear_screen(const Color &color);

#endif