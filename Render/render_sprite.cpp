
void static_pos_update(Dot &pos, bool is_static) {
    if (!is_static) {
        pos -= camera.pos;
    }
}

void static_pos_update(Dot &pos) {
    static_pos_update(pos, camera_is_static);
}

// проверяет пересечение экрана и этого прямоугольника
bool arena_query(efloat left, efloat right, efloat top, efloat bottom) {
    return !(
        right < -arena_half_size.x || arena_half_size.x < left ||  // x
        top < -arena_half_size.y || arena_half_size.y < bottom     // y
    );
}

Color standart_pixel_func(const Color &color) {
    return color;
}

template <u8 alpha>
Color alpha_pixel_func(const Color &color) {
    return {color.r, color.g, color.b, alpha};
}

#define shadow_pixel_func alpha_pixel_func<100>

// вызывается из других функций draw_sprite
template <typename func_t = Color(const Color &color)>
void draw_sprite_basic(
    Dot pos,
    efloat size,
    Matrix<Color> &pixels,
    int sprite_x0,
    int sprite_y0,
    int sprite_x1,
    int sprite_y1,
    func_t &&func = standart_pixel_func
) {
    efloat rect_sz = size * scale_factor;

    efloat y = (pos.y + arena_half_size.y - size / 2) * scale_factor;

    for (int i = sprite_y0; i < sprite_y1; i++) {
        efloat x = (pos.x + arena_half_size.x - size / 2) * scale_factor;

        s64 y0 = static_cast<s64>(y);
        s64 y1 = static_cast<s64>(y + rect_sz);

        for (int j = sprite_x0; j < sprite_x1;) {
            s64 x0 = static_cast<s64>(x);
            int k = j;
            while (k < sprite_x1 && pixels[i][k] == pixels[i][j]) {
                k++;
            }

            x += (k - j) * rect_sz;
            s64 x1 = static_cast<s64>(x);

            if (is_draw(pixels[i][j])) {
                draw_rect_in_pixels(x0, y0, x1, y1, func(pixels[i][j]));
            } else if (debug_mode) {
                draw_rect_in_pixels(x0, y0, x1, y1, Color(0xffffff, 60));
            }

            j = k;
        }
        y -= rect_sz;
    }

    // не переливает пиксели, но трясет сам спрайт
    /*efloat y = (arena_half_size.y - size / 2) * scale_factor;

    for (int i = sprite_y0; i < sprite_y1; i++) {
        efloat x = (arena_half_size.x - size / 2) * scale_factor;
        for (int j = sprite_x0; j < sprite_x1; j++) {
            // change to pixels
            int x0 = static_cast<int>(x) + int(pos.x * scale_factor);
            int y0 = static_cast<int>(y) + int(pos.y * scale_factor);

            int x1 = static_cast<int>(x + size * scale_factor) +
                     int(pos.x * scale_factor);
            int y1 = static_cast<int>(y + size * scale_factor) +
                     int(pos.y * scale_factor);

            if (is_draw(pixels[i][j])) {
                draw_rect_in_pixels(x0, y0, x1, y1, func(pixels[i][j]));
            } else if (debug_mode) {
                draw_rect_in_pixels(x0, y0, x1, y1, Color(0xffffff, 60));
            }

            x += size * scale_factor;
        }
        y -= size * scale_factor;
    }*/

    /*{
        efloat y = (arena_half_size.y - size / 2) * scale_factor;

        for (int i = sprite_y0; i < sprite_y1; i++) {
            efloat x = (arena_half_size.x - size / 2) * scale_factor;
            for (int j = sprite_x0; j < sprite_x1; j++) {
                if (is_draw(pixels[i][j])) {
                    int x0 = static_cast<int>(x) + int(pos.x * scale_factor);
                    int y0 = static_cast<int>(y) + int(pos.y * scale_factor);

                    if (x + pos.x * scale_factor + 0.5 < x0) {
                        break;
                    }
                    x0--;

                    int x1 = static_cast<int>(x + size * scale_factor) +
                             int(pos.x * scale_factor);
                    int y1 = static_cast<int>(y + size * scale_factor) +
                             int(pos.y * scale_factor);

                    if (is_draw(pixels[i][j])) {
                        draw_rect_in_pixels(x0, y0, x1, y1, func(pixels[i][j]));
                    } else if (debug_mode) {
                        draw_rect_in_pixels(
                            x0, y0, x1, y1, Color(0xffffff, 60)
                        );
                    }

                    break;
                }
                x += size * scale_factor;
            }
            y -= size * scale_factor;
        }
    }*/
}

// рисует спрайт
template <typename func_t = Color(const Color &color)>
void draw_sprite_static(
    Dot pos,
    efloat size,
    sprite_t sprite,
    func_t &&func = standart_pixel_func
) {
    auto &pixels = Sprites[sprite].picture;

    if (arena_query(
            pos.x - size / 2, pos.x + pixels.width() * size + size / 2,
            pos.y + size / 2, pos.y - pixels.height() * size - size / 2
        )) {
        draw_sprite_basic(
            pos, size, pixels, 0, 0, pixels.width(), pixels.height(), func
        );
    }
}

template <typename func_t = Color(const Color &color)>
void draw_sprite(
    Dot pos,
    efloat size,
    sprite_t sprite,
    func_t &&func = standart_pixel_func
) {
    static_pos_update(pos);

    draw_sprite_static(pos, size, sprite, func);
}

// WARGNING NOT ARENA QUERY
// рисует текстуру
// x_cnt, y_cnt - колво спрайтов по координатам
template <typename func_t = Color(const Color &color)>
void draw_texture(
    Dot pos,
    unsigned int x_cnt,
    unsigned int y_cnt,
    efloat size,
    sprite_t texture,
    func_t &&func = standart_pixel_func
) {
    auto &pixels = Sprites[texture].picture;

    const efloat x_summary = pixels.width() * size;
    const efloat y_summary = pixels.height() * size;

    for (unsigned int i = 0; i < y_cnt; i++) {
        for (unsigned int j = 0; j < x_cnt; j++) {
            draw_sprite(Dot(pos.x + x_summary * j, pos.y), size, texture, func);
        }
        pos.y -= y_summary;
    }
}

// x_cnt, y_cnt - колво спрайтов по координатам
template <typename func_t = Color(const Color &color)>
void draw_texture_align(
    Dot pos,
    unsigned int x_cnt,
    unsigned int y_cnt,
    efloat size,
    sprite_t texture,
    func_t &&func = standart_pixel_func
) {
    auto &pixels = Sprites[texture].picture;

    const efloat x_summary = pixels.width() * size;
    const efloat y_summary = pixels.height() * size;

    draw_texture(
        pos + Dot(-x_summary * x_cnt * 0.5, y_summary * y_cnt * 0.5), x_cnt,
        y_cnt, size, texture, func
    );
}

// рисует спрайт из листа спрайтов
// len_x - длина спрайта по x
// sprite_count - идентификатор спрайта
template <typename func_t = Color(const Color &color)>
void draw_spritesheet_static(
    Dot pos,
    efloat size,
    sprite_t spritesheet,
    unsigned int len_x,
    unsigned int sprite_count,
    func_t &&func = standart_pixel_func
) {
    Dot half_size = Dot(0.5, 0.5) * size;

    auto &pixels = Sprites[spritesheet].picture;

    if (!arena_query(
            pos.x - half_size.x, pos.x + len_x * size + half_size.x,
            pos.y + half_size.y, pos.y - pixels.height() * size - half_size.y
        )) {
        return;
    }

    unsigned int begin_x = len_x * sprite_count;
    unsigned int end_x = begin_x + len_x;

    draw_sprite_basic(
        pos, size, pixels, begin_x, 0, end_x, pixels.height(), func
    );
}

// рисует спрайт из листа спрайтов
// len_x - длина спрайта по x
// sprite_count - идентификатор спрайта
template <typename func_t = Color(const Color &color)>
void draw_spritesheet(
    Dot pos,
    efloat size,
    sprite_t spritesheet,
    unsigned int len_x,
    unsigned int sprite_count,
    func_t &&func = standart_pixel_func
) {
    static_pos_update(pos);

    draw_spritesheet_static(pos, size, spritesheet, len_x, sprite_count, func);
}
