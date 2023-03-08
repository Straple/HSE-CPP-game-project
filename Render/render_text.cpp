
#include "letters.cpp"

// рисует символ
void draw_symbol(char symbol, Dot pos, efloat size, const Color &color) {
    if (symbol == ' ') {
        return;
    }

    if (symbol == 'q' || symbol == 'p' || symbol == 'y' || symbol == 'g' ||
        symbol == ',') {
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

// рисует текст
void draw_text(const char *text, Dot pos, efloat size, const Color &color) {
    efloat original_x = pos.x;

    while (*text) {
        if (*text == '\n') {
            pos.y -= size * 9;
            pos.x = original_x;
        } else {
            draw_symbol(*text, pos, size, color);
            pos.x += size * symbol_len(*text);
        }

        text++;
    }
}

// рисует выравненный текст по центру
void draw_text_align(
    const char *text,
    Dot pos,
    efloat size,
    const Color &color
) {
    pos.x -= text_len(text) * size / 2;
    draw_text(text, pos, size, color);
}

template <typename T>
void draw_object(const T &object, Dot pos, efloat size, const Color &color) {
    draw_text(to_string(object).c_str(), pos, size, color);
}
