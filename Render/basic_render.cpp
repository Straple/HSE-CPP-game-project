
// заполняет значения dest[0], dest[1], ..., dest[len-1] значением val32
void fill(uint32_t *dest, uint32_t val32, unsigned int len) {
    // global_variables::count_of_render_rects += len;

    // global_variables::count_of_render_rects++;

    // можно использовать и это, но реализация ниже оказывается быстрее
    // std::fill_n(dest, len, val32);

    // summary len: ~3'160'000
    // count of call: ~110'000
    // len = 1: ~300
    // len = 2: ~4'000
    // len = 3: ~300
    // len = 4: ~7'000
    // len = 5: ~27'000
    // len = 6: ~300
    // len = 7: ~300
    // len = 8: ~300
    // len = 9: ~10'000
    // len = 10: ~15'000
    // len = 11: ~100
    // len = 12: ~100
    // len = 13: ~100
    //
    // len <= 13: ~65'000
    // len > 13: ~50'000
    // len > 20: ~22'000
    // len > 30: ~11'000
    // len > 40: ~7'000

    // std::fill_n(dest, len, val24);

    // for (int i = 0; i < len; i++) {
    //     dest[i] = val32;
    // }

    /*u64 val64 = (static_cast<u64>(val32) << 32) | val32;

    len--;
    for (unsigned int i = 0; i < len; i += 2) {
        *reinterpret_cast<u64 *>(dest + i) = val64;
    }
    len++;

    if (len & 1) {
        dest[len - 1] = val32;
    }*/
}

void fill2(render_chunk *dest, Color color, unsigned int l, unsigned int r) {
    unsigned int lc = l >> 5;
    unsigned int rc = r >> 5;

    if (lc == rc) {
        dest[lc].flush();
        while (l <= r) {
            dest[lc].data[l & 0b11111] = color;
            l++;
            // global_variables::count_of_render_rects++;
        }
        return;
    }

    if (l & 0b11111) {
        dest[lc].flush();
        while (l & 0b11111) {
            dest[lc].data[l & 0b11111] = color;
            l++;
            //global_variables::count_of_render_rects++;
        }
        lc++;
    }

    if ((r & 0b11111) != 0b11111) {
        dest[rc].flush();
        while (r & 0b11111) {
            dest[rc].data[r & 0b11111] = color;
            r--;
            //global_variables::count_of_render_rects++;
        }
        dest[rc].data[0] = color;
        rc--;
    }

    while (lc <= rc) {
        dest[lc].state = true;
        dest[lc].data[0] = color;
        lc++;
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
    // ~35k
    if (x0 >= x1 || y0 >= y1) {
        return;
    }
    // ~22k

    ASSERT(
        x0 <= global_variables::render_state.width() &&
            x1 <= global_variables::render_state.width() &&
            y0 <= global_variables::render_state.height() &&
            y1 <= global_variables::render_state.height(),
        "out of render pixels"
    );

    const unsigned int screen_width = global_variables::render_state.width();
    const unsigned int len = x1 - x0;

    if (true) {  //(color.a == 0xff) {
        for (unsigned int y = y0; y < y1; y++) {
            fill2(global_variables::render_state.data()[y], color, x0, x1 - 1);

            /*int l = x0 / chunk_len;
            int r = (x1 - 1) / chunk_len;
            if (x0 % chunk_len != 0) {
                row[l].flush();
                while(x0 % chunk_len != 0){
                    row[l].data[x0 % chunk_len] = color;
                    x0++;
                }
                l++;
            }

            while (l <= r) {
                row[l].state = true;
                row[l].data[0] = color;
                l++;
            }*/

            // fill(reinterpret_cast<uint32_t*>(row),
            // static_cast<uint32_t>(color), len);
        }
    } /*else {
        for (unsigned int y = y0; y < y1; y++, row += screen_width) {
            unsigned int x = 0;

            while (x < len) {
                unsigned int k = x + 1;

                while (k < len && row[k] == row[x]) {
                    k++;
                }

                fill(
                    reinterpret_cast<unsigned int *>(row + x),
                    static_cast<unsigned int>(row[x].combine(color)), k - x
                );
                x = k;
            }
        }
    }*/
}

// рисует прямоугольник в пикселях с обработкой границ
void draw_rect_in_pixels(s64 x0, s64 y0, s64 x1, s64 y1, const Color &color) {
    x0 = clamp<s64>(
        0, x0, static_cast<s64>(global_variables::render_state.width())
    );
    x1 = clamp<s64>(
        0, x1, static_cast<s64>(global_variables::render_state.width())
    );

    y0 = clamp<s64>(
        0, y0, static_cast<s64>(global_variables::render_state.height())
    );
    y1 = clamp<s64>(
        0, y1, static_cast<s64>(global_variables::render_state.height())
    );

    draw_pixels(x0, y0, x1, y1, color);
}

// зарисовывает весь экран цветом
void clear_screen(const Color &color) {
    draw_pixels(
        0, 0, global_variables::render_state.width(),
        global_variables::render_state.height(), color
    );
}
