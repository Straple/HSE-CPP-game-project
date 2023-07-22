#include "basic_render.hpp"
#include "../global_variables.hpp"

#ifdef GAME_ENGINE_USE_SSE
#include <emmintrin.h>
#endif

// заполняет значения dest[0], dest[1], ..., dest[len-1] значением val32
void fill(uint32_t *dest, uint32_t val32, unsigned int len) {
    // можно использовать и это, но моя реализация с val64 быстрее
    // std::fill_n(dest, len, val32);

    // статистика вызовов этой функции
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

    // len <= 13: ~65'000
    // len > 13: ~50'000
    // len > 20: ~22'000
    // len > 30: ~11'000
    // len > 40: ~7'000

#ifdef GAME_ENGINE_USE_SSE

    int to_len = len - len % 4;
    while (len != to_len) {
        len--;
        dest[len] = val32;
    }

    const __m128i val128 = _mm_set1_epi32(val32);
    for (__m128i *ptr = reinterpret_cast<__m128i *>(dest),
                 *end = reinterpret_cast<__m128i *>(dest + len);
         ptr != end; ptr++) {
        _mm_storeu_si128(ptr, val128);
    }

#else
    uint64_t val64 = (static_cast<uint64_t>(val32) << 32) | val32;

    len--;
    for (unsigned int i = 0; i < len; i += 2) {
        *reinterpret_cast<uint64_t *>(dest + i) = val64;
    }
    len++;

    if (len & 1) {
        dest[len - 1] = val32;
    }
#endif
}

void draw_pixels_row_trivial_impl(Color *row, unsigned int len, const Color &color) {
    unsigned int x = 0;
    while (x < len) {
        unsigned int k = x + 1;

        while (k < len && row[k] == row[x]) {
            k++;
        }

        fill(reinterpret_cast<unsigned int *>(row + x), static_cast<unsigned int>(row[x].combine(color)), k - x);
        x = k;
    }
}

void draw_pixels_impl(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, const Color &color) {
    Color *row = global_variables::render_state[y0] + x0;
    const unsigned int screen_width = global_variables::render_state.width();
    unsigned int len = x1 - x0;

    if (color.a == 0xff) {
        for (unsigned int y = y0; y < y1; y++, row += screen_width) {
            fill(reinterpret_cast<unsigned int *>(row), static_cast<unsigned int>(color), len);
        }
    } else {
#ifdef GAME_ENGINE_USE_SSE

        const __m128i zero = _mm_setzero_si128();
        const __m128i alpha = _mm_set1_epi16(color.a);
        const __m128i inverse_alpha = _mm_sub_epi16(_mm_set1_epi16(0x00FF), alpha);

        // заранее посчитанный цвет, чтобы в цикле возиться только с
        // остальными цветами, а этот просто прибавить
        __m128i pre_color = _mm_set1_epi32(static_cast<unsigned int>(color));
        pre_color = _mm_unpacklo_epi8(pre_color, zero);
        pre_color = _mm_mullo_epi16(pre_color, alpha);
        pre_color = _mm_srli_epi16(pre_color, 8);
        pre_color = _mm_packus_epi16(pre_color, pre_color);

        for (unsigned int y = y0; y < y1; y++, row += screen_width) {
            for (__m128i *ptr = reinterpret_cast<__m128i *>(row),
                         *end = reinterpret_cast<__m128i *>(row + len - len % 4);
                 ptr < end; ptr++) {
                const __m128i dest_pixels = _mm_loadu_si128(ptr);

                const __m128i dest_lo16 = _mm_unpacklo_epi8(dest_pixels, zero);
                const __m128i dest_hi16 = _mm_unpackhi_epi8(dest_pixels, zero);

                const __m128i mult_dest_lo16 =
                    _mm_mullo_epi16(dest_lo16, inverse_alpha);
                const __m128i mult_dest_hi16 =
                    _mm_mullo_epi16(dest_hi16, inverse_alpha);

                const __m128i result_lo16 = _mm_srli_epi16(mult_dest_lo16, 8);
                const __m128i result_hi16 = _mm_srli_epi16(mult_dest_hi16, 8);

                const __m128i result_dest = _mm_packus_epi16(result_lo16, result_hi16);

                _mm_storeu_si128(ptr, _mm_add_epi8(result_dest, pre_color));
            }

            draw_pixels_row_trivial_impl(row + len - len % 4, len % 4, color);
        }
#else
        for (unsigned int y = y0; y < y1; y++, row += screen_width) {
            draw_pixels_row_trivial_impl(row, len, color);
        }
#endif
    }
}

#include "multi_threaded_render_unused_need_to_rebuild.cpp"

// рисует в пикселях [x0, x1)*[y0, y1)
// x0, x1 <= width
// y0, y1 <= height
void draw_pixels(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, const Color &color) {
    if (x0 >= x1 || y0 >= y1) {
        return;
    }

    ASSERT(x0 <= global_variables::render_state.width() && x1 <= global_variables::render_state.width() && y0 <= global_variables::render_state.height() && y1 <= global_variables::render_state.height(), "out of render pixels");

#if MULTITHREAD_RENDER == 0
    draw_pixels_impl(x0, y0, x1, y1, color);
#elif MULTITHREAD_RENDER == 1
    Render_tasks.push_back({x0, y0, x1, y1, color});
#endif
}

// рисует прямоугольник в пикселях с обработкой границ
void draw_rect_in_pixels(int64_t x0, int64_t y0, int64_t x1, int64_t y1, const Color &color) {
    x0 = clamp<int64_t>(
        0, x0, static_cast<int64_t>(global_variables::render_state.width())
    );
    x1 = clamp<int64_t>(
        0, x1, static_cast<int64_t>(global_variables::render_state.width())
    );

    y0 = clamp<int64_t>(
        0, y0, static_cast<int64_t>(global_variables::render_state.height())
    );
    y1 = clamp<int64_t>(
        0, y1, static_cast<int64_t>(global_variables::render_state.height())
    );

    draw_pixels(x0, y0, x1, y1, color);
}

// зарисовывает весь экран цветом
void clear_screen(const Color &color) {
    if (color.a == 0xff) {
        Color *pixels = global_variables::render_state.render_memory();
        unsigned int size = global_variables::render_state.height() *
                            global_variables::render_state.width();
        fill(reinterpret_cast<unsigned int *>(pixels), static_cast<unsigned int>(color), size);
    } else {
        draw_pixels(0, 0, global_variables::render_state.width(), global_variables::render_state.height(), color);
    }
}
