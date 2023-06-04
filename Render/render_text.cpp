
#include "render_text.hpp"
#include <cstring>
#include "letters.cpp"
#include "render_geometry.hpp"

int symbol_len_in_pixels(char symbol) {
    if (symbol == ' ') {
        return 4;
    } else {
        int right_x = -1;

        auto memory = get_symbol(symbol);
        for (int i = 0; i < 7; i++) {
            right_x = std::max(right_x, static_cast<int>(strlen(memory[i])));
        }
        return right_x + 1;
    }
}

int text_len_in_pixels(const char *text) {
    int max_len = 0;
    int len = 0;
    for (; *text; text++) {
        if (*text == '\n') {
            max_len = std::max(max_len, len);
            len = 0;
        } else {
            len += symbol_len_in_pixels(*text);
        }
    }
    max_len = std::max(max_len, len);
    return max_len;
}

void draw_symbol(char symbol, Dot pos, efloat size, const Color &color) {
    if (symbol == ' ') {
        return;
    }

    if (symbol == 'q' || symbol == 'p' || symbol == 'y' || symbol == 'g' || symbol == ',') {
        pos.y -= 2 * size;
    }

    auto memory = get_symbol(symbol);

    efloat original_x = pos.x;
    Dot half_size = Dot(1, 1) * (size * 0.5);

    for (unsigned int i = 0; i < 7; i++) {
        for (auto row = memory[i]; *row; row++) {
            if (*row == '0') {
                draw_rect(pos, half_size, color);
            }
            pos.x += size;
        }
        pos.x = original_x;
        pos.y -= size;
    }
}

void draw_text(const char *text, Dot pos, efloat size, const Color &color) {
    efloat original_x = pos.x;

    while (*text) {
        if (*text == '\n') {
            pos.y -= size * 9;
            pos.x = original_x;
        } else {
            draw_symbol(*text, pos, size, color);
            pos.x += size * symbol_len_in_pixels(*text);
        }

        text++;
    }
}

void draw_text_align(const char *text, Dot pos, efloat size, const Color &color) {
    pos.x -= text_len_in_pixels(text) * size / 2;
    draw_text(text, pos, size, color);
}
