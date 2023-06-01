#ifndef GAME_ENGINE_RENDER_TEXT_HPP
#define GAME_ENGINE_RENDER_TEXT_HPP

#include "../Objects/Geometry/dot.hpp"
#include "../Objects/Sprite/color.hpp"

int symbol_len_in_pixels(char symbol);

int text_len_in_pixels(const char *text);

// рисует символ
void draw_symbol(char symbol, Dot pos, efloat size, const Color &color);

// рисует текст
void draw_text(const char *text, Dot pos, efloat size, const Color &color);

// рисует выравненный текст по центру
void draw_text_align(const char *text, Dot pos, efloat size, const Color &color);

template <typename T>
void draw_object(const T &object, Dot pos, efloat size, const Color &color) {
    draw_text(to_string(object).c_str(), pos, size, color);
}

#endif  // GAME_ENGINE_RENDER_TEXT_HPP