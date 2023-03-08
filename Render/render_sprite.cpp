
void static_pos_update(Dot &pos, bool is_static) {
    if (!is_static) {
        pos -= camera.pos;
        /*efloat x = camera.pos.x;
        x *= scale_factor;
        x = int(x);
        x /= scale_factor;

        efloat y = camera.pos.y;
        y *= scale_factor;
        y = int(y);
        y /= scale_factor;

        pos -= Dot(x, y);*/
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

/*Sprite build_sprite_scaling(const Sprite &sprite, efloat size) {
    Sprite result(
        sprite.height() * size * scale_factor,
        sprite.width() * size * scale_factor
    );
    efloat y = 0;
    for (int i = 0; i < sprite.height(); i++) {
        efloat x = 0;
        for (int j = 0; j < sprite.width(); j++) {
            int x0 = int(x);
            int y0 = int(y);
            int x1 = int(x + size * scale_factor);
            int y1 = int(y + size * scale_factor);

            for (int a = y0; a < y1; a++) {
                for (int b = x0; b < x1; b++) {
                    result[a][b] = sprite[sprite.height() - i - 1][j];
                }
            }
            x += size * scale_factor;
        }
        y += size * scale_factor;
    }
    return result;
}*/

// вызывается из других функций draw_sprite
template <typename func_t = Color(const Color &color)>
void draw_sprite_matrix(
    Dot pos,
    efloat size,
    const Sprite &pixels,
    func_t &&func = standart_pixel_func
) {
    efloat rect_sz = size * scale_factor;

    // auto colors = build_sprite_scaling(pixels, size);

    /*pos.y *= scale_factor;
    pos.y = int(pos.y);
    pos.y /= scale_factor;

    pos.x *= scale_factor;
    pos.x = int(pos.x);
    pos.x /= scale_factor;*/

    /*int posx = (pos.x - size / 2 + arena_half_size.x) * scale_factor;
    int posy = (pos.y - size / 2 + arena_half_size.y) * scale_factor;
    posy -= pixels.height();

    for (int i = std::max(-posy, 0);
         i < std::min(int(render_state.height()) - posy, int(pixels.height()));
         i++) {
        for (int j = std::max(-posx, 0);
             j <
             std::min(int(render_state.width()) - posx, (int)pixels.width());
             j++) {
            ASSERT(
                0 <= posx + j && posx + j < render_state.width(),
                "something went wrong"
            );
            ASSERT(
                0 <= posy + i && posy + i < render_state.height(),
                "something went wrong"
            );

            if (is_draw(pixels[i][j])) {
                render_state[posy + i][posx + j] =
                    render_state[posy + i][posx + j].combine(func(pixels[i][j])
                    );
            } else if (debug_mode) {
                render_state[posy + i][posx + j] =
                    render_state[posy + i][posx + j].combine(Color(0xffffff, 60)
                    );
            }
        }
    }*/

    // efloat y = (pos.y + arena_half_size.y - size / 2) * scale_factor;

    /*{
        efloat x = (pos.x + arena_half_size.x - size / 2) * scale_factor;

        for (int id = 0; id < count_of_render_threads; id++) {
            Threads_vals[id].give_task(
                {x, y}, size, sprite, sprite_x0,
                sprite_y0 +
                    (sprite_y1 - sprite_y0) * id / count_of_render_threads,
                sprite_x1,
                sprite_y0 +
                    (sprite_y1 - sprite_y0) * (id + 1) / count_of_render_threads
            );
            y -= rect_sz *
                 ((sprite_y1 - sprite_y0) * (id + 1) / count_of_render_threads -
                  (sprite_y1 - sprite_y0) * id / count_of_render_threads);
        }
    }*/

    // против разрывов в изображении
    s64 end_x = 0;
    {
        efloat x = (pos.x + arena_half_size.x - size / 2) * scale_factor;
        for (int j = 0; j < pixels.width(); j++) {
            x += rect_sz;
        }
        end_x = static_cast<s64>(x + 0.1);
    }

    efloat y = (pos.y + arena_half_size.y - size / 2) * scale_factor;
    for (int i = 0; i < pixels.height(); i++) {
        efloat x = (pos.x + arena_half_size.x - size / 2) * scale_factor;

        s64 y0 = static_cast<s64>(y);
        s64 y1 = static_cast<s64>(y + rect_sz);

        for (int j = 0; j < pixels.width();) {
            s64 x0 = static_cast<s64>(x);
            int k = j;
            while (k < pixels.width() && pixels[i][k] == pixels[i][j]) {
                k++;
            }

            x += (k - j) * rect_sz;
            s64 x1 = static_cast<s64>(x);

            // против разрывов в изображении
            if (k == pixels.width()) {
                x1 = end_x;
            }

            if (is_draw(pixels[i][j])) {
                draw_rect_in_pixels(x0, y0, x1, y1, func(pixels[i][j]));
            } else if (debug_mode) {
                draw_rect_in_pixels(x0, y0, x1, y1, Color(0xffffff, 60));
            }

            j = k;
        }

        y -= rect_sz;
    }

    // std::this_thread::yield();
    // wait_all_render_threads();

    /*for (int i = sprite_y0; i < sprite_y1; i++) {
        efloat x = (pos.x + arena_half_size.x - size / 2) * scale_factor;

        if (!Threads_vals[0].is_busy()) {
            Threads_vals[0].give_task(
                {x, y}, size, sprite, i, sprite_x0, i, sprite_x1, i + 1
            );
        } else {
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
        }
        y -= rect_sz;
    }*/

    // pos += arena_half_size;
    /*{
        efloat x = arena_half_size.x;
        x *= scale_factor;
        x = int(x);
        x /= scale_factor;
        pos.x += x;

        efloat y = arena_half_size.y;
        y *= scale_factor;
        y = int(y);
        y /= scale_factor;
        pos.y += y;
    }

    efloat y = 0;

    int posx = pos.x * scale_factor;
    int posy = pos.y * scale_factor;

    for (int i = 0; i < pixels.height(); i++) {
        efloat x = 0;
        for (int j = 0; j < pixels.width(); j++) {
            // change to pixels
            int x0 = static_cast<int>(x - rect_sz / 2) + posx;
            int y0 = static_cast<int>(y - rect_sz / 2) + posy;

            int x1 = static_cast<int>(x + rect_sz / 2) + posx;
            int y1 = static_cast<int>(y + rect_sz / 2) + posy;

            if (is_draw(pixels[i][j])) {
                draw_rect_in_pixels(x0, y0, x1, y1, func(pixels[i][j]));
            } else if (debug_mode) {
                draw_rect_in_pixels(x0, y0, x1, y1, Color(0xffffff, 60));
            }

            x += rect_sz;
        }
        y -= rect_sz;
    }*/

    // не переливает пиксели, но трясет сам спрайт
    /*efloat y = (arena_half_size.y - size / 2) * scale_factor;

    int posx = pos.x * scale_factor;
    int posy = pos.y * scale_factor;

    for (int i = 0; i < pixels.height(); i++) {
        efloat x = (arena_half_size.x - size / 2) * scale_factor;
        for (int j = 0; j < pixels.width(); j++) {
            // change to pixels
            int x0 = static_cast<int>(x) + posx;
            int y0 = static_cast<int>(y) + posy;

            int x1 = static_cast<int>(x + size * scale_factor) + posx;
            int y1 = static_cast<int>(y + size * scale_factor) + posy;

            if (is_draw(pixels[i][j])) {
                draw_rect_in_pixels(x0, y0, x1, y1, func(pixels[i][j]));
            } else if (debug_mode) {
                draw_rect_in_pixels(x0, y0, x1, y1, Color(0xffffff, 60));
            }
            x += size * scale_factor;
        }
        y -= size * scale_factor;
    }*/
}

// #include <map>

// std::vector<std::map<efloat, Sprite>> Sprites_cache(SP_COUNT);
// std::vector<std::map<efloat, std::map<unsigned int, Sprite>>>
//     Spritesheets_cache(SS_COUNT);

// рисует спрайт
template <typename func_t = Color(const Color &color)>
void draw_sprite_static(
    Dot pos,
    efloat size,
    sprite_t sprite,
    func_t &&func = standart_pixel_func
) {
    auto &pixels = Sprites[sprite];

    if (arena_query(
            pos.x - size / 2, pos.x + pixels.width() * size + size / 2,
            pos.y + size / 2, pos.y - pixels.height() * size - size / 2
        )) {
        draw_sprite_matrix(pos, size, Sprites[sprite], func);
        /*if (!Sprites_cache[sprite].count(size)) {
            CALC_TIME_START
            Sprites_cache[sprite][size] = build_sprite_scaling(pixels, size);
            std::cout << "build" << std::endl;
            CALC_TIME_END
        }
        draw_sprite_matrix(pos, size, Sprites_cache[sprite][size], func);*/
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
    auto &pixels = Sprites[texture];
    const efloat x_summary = pixels.width() * size;
    const efloat y_summary = pixels.height() * size;

    for (unsigned int i = 0; i < y_cnt; i++) {
        for (unsigned int j = 0; j < x_cnt; j++) {
            draw_sprite(
                Dot(pos.x + j * x_summary, pos.y - i * y_summary), size,
                texture, func
            );
        }
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
    auto &pixels = Sprites[texture];

    const efloat x_summary = pixels.width() * size;
    const efloat y_summary = pixels.height() * size;

    draw_texture(
        pos + 0.5 * Dot(-x_summary * x_cnt, y_summary * y_cnt), x_cnt, y_cnt,
        size, texture, func
    );
}

// рисует спрайт из листа спрайтов
// sprite_count - идентификатор спрайта
template <typename func_t = Color(const Color &color)>
void draw_spritesheet_static(
    Dot pos,
    efloat size,
    spritesheet_t spritesheet,
    unsigned int sprite_count,
    func_t &&func = standart_pixel_func
) {
    Dot half_size = Dot(0.5, 0.5) * size;

    auto &pixels = Spritesheets[spritesheet][sprite_count];

    if (!arena_query(
            pos.x - half_size.x, pos.x + pixels.width() * size + half_size.x,
            pos.y + half_size.y, pos.y - pixels.height() * size - half_size.y
        )) {
        return;
    }

    draw_sprite_matrix(
        pos, size, Spritesheets[spritesheet][sprite_count], func
    );

    /*if (!Spritesheets_cache[spritesheet][size].count(sprite_count)) {
        CALC_TIME_START
        Spritesheets_cache[spritesheet][size][sprite_count] =
            build_sprite_scaling(pixels, size);
        CALC_TIME_END
        std::cout << "build" << std::endl;
    }

    draw_sprite_matrix(
        pos, size, Spritesheets_cache[spritesheet][size][sprite_count], func
    );*/
}

// рисует спрайт из листа спрайтов
// len_x - длина спрайта по x
// sprite_count - идентификатор спрайта
template <typename func_t = Color(const Color &color)>
void draw_spritesheet(
    Dot pos,
    efloat size,
    spritesheet_t spritesheet,
    unsigned int sprite_count,
    func_t &&func = standart_pixel_func
) {
    static_pos_update(pos);

    draw_spritesheet_static(pos, size, spritesheet, sprite_count, func);
}
