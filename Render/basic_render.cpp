
// заполняет значения dest[0], dest[1], ..., dest[len-1] значением val32
void fill(unsigned int *dest, unsigned int val32, unsigned int len) {
    // можно использовать и это, но реализация ниже оказывается быстрее
    // std::fill_n(dest, len, val32);
    // memset(dest, val32, len);

    u64 val64 = (static_cast<u64>(val32) << 32) | val32;

    len--;
    for (unsigned int i = 0; i < len; i += 2) {
        *reinterpret_cast<u64 *>(dest + i) = val64;
    }
    len++;

    if (len & 1) {
        dest[len - 1] = val32;
    }
}

// рисует в пикселях [x0, x1)*[y0, y1)
// x0, x1 <= width
// y0, y1 <= height
void draw_pixels(
    unsigned int x0,
    unsigned int y0,
    unsigned int x1,
    unsigned int y1,
    const Color &color
) {
    if (x0 >= x1 || y0 >= y1) {
        return;
    }

    ASSERT(
        x0 <= render_state.width() && x1 <= render_state.width() &&
            y0 <= render_state.height() && y1 <= render_state.height(),
        "out of render pixels"
    );

    Color *row = render_state[y0];

    if (color.a == 0xff) {
        for (unsigned int y = y0; y < y1; y++) {
            fill(
                reinterpret_cast<unsigned int *>(row + x0),
                static_cast<unsigned int>(color), x1 - x0
            );

            row += render_state.width();
        }
    } else {
        for (unsigned int y = y0; y < y1; y++) {
            unsigned int x = x0;

            while (x < x1) {
                unsigned int k = x + 1;

                while (k < x1 && row[k] == row[x]) {
                    k++;
                }

                fill(
                    reinterpret_cast<unsigned int *>(row + x),
                    static_cast<unsigned int>(row[x].combine(color)), k - x
                );
                x = k;
            }
            row += render_state.width();
        }
    }
}

// рисует прямоугольник в пикселях с обработкой границ
void draw_rect_in_pixels(s64 x0, s64 y0, s64 x1, s64 y1, const Color &color) {
    x0 = clamp<s64>(0, x0, static_cast<s64>(render_state.width()));
    x1 = clamp<s64>(0, x1, static_cast<s64>(render_state.width()));

    y0 = clamp<s64>(0, y0, static_cast<s64>(render_state.height()));
    y1 = clamp<s64>(0, y1, static_cast<s64>(render_state.height()));

    draw_pixels(x0, y0, x1, y1, color);
}

// зарисовывает весь экран цветом
void clear_screen(const Color &color) {
    draw_pixels(0, 0, render_state.width(), render_state.height(), color);
}
