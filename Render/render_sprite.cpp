#define GAME_ENGINE_STANDARD_RENDER_SYSTEM

#define GAME_ENGINE_MULTITHREAD_RENDER

void static_pos_update(Dot &pos, bool is_static) {
    if (!is_static) {
        pos -= global_variables::camera.pos;
        /*efloat x = global_variables::camera.pos.x;
        x *= global_variables::scale_factor;
        x = int(x);
        x /= global_variables::scale_factor;

        efloat y = global_variables::camera.pos.y;
        y *= global_variables::scale_factor;
        y = int(y);
        y /= global_variables::scale_factor;

        pos -= Dot(x, y);*/
    }
}

void static_pos_update(Dot &pos) {
    static_pos_update(pos, global_variables::camera_is_static);
}

// проверяет пересечение экрана и этого прямоугольника
bool arena_query(efloat left, efloat right, efloat top, efloat bottom) {
    return !(
        right < -global_variables::arena_half_size.x ||
        global_variables::arena_half_size.x < left ||  // x
        top < -global_variables::arena_half_size.y ||
        global_variables::arena_half_size.y < bottom  // y
    );
}

inline Color standart_pixel_func(const Color &color) {
    return color;
}

template <u8 alpha>
Color alpha_pixel_func(const Color &color) {
    return {color.r, color.g, color.b, alpha};
}

#define shadow_pixel_func alpha_pixel_func<100>

#ifndef GAME_ENGINE_STANDARD_RENDER_SYSTEM
Sprite build_sprite_scaling(const Sprite &sprite, efloat size) {
    Sprite result(
        static_cast<int>(
            sprite.height() * size * global_variables::scale_factor
        ),
        static_cast<int>(sprite.width() * size * global_variables::scale_factor)
    );
    efloat y = 0;
    for (int i = 0; i < sprite.height(); i++) {
        efloat x = 0;
        for (int j = 0; j < sprite.width(); j++) {
            int x0 = int(x);
            int y0 = int(y);
            int x1 = int(x + size * global_variables::scale_factor);
            int y1 = int(y + size * global_variables::scale_factor);

            for (int a = y0; a < y1; a++) {
                for (int b = x0; b < x1; b++) {
                    result[a][b] = sprite[sprite.height() - i - 1][j];
                }
            }
            x += size * global_variables::scale_factor;
        }
        y += size * global_variables::scale_factor;
    }
    return result;
}
#endif

// вызывается из других функций draw_sprite
template <typename func_t = Color(const Color &color)>
void draw_sprite_matrix(
    Dot pos,
    const efloat size,
    const Sprite &pixels,
    func_t &&func = standart_pixel_func
) {
#ifdef GAME_ENGINE_STANDARD_RENDER_SYSTEM

    pos += global_variables::arena_half_size;
    pos *= global_variables::scale_factor;

    efloat rect_sz = size * global_variables::scale_factor;

    // std::vector<s64> posy(pixels.height() + 1), posx(pixels.width() + 1);
    static s64 posy[10'000], posx[10'000];

    ASSERT(pixels.height() + 1 < sizeof(posy) / sizeof(s64), "out of range");
    ASSERT(pixels.width() + 1 < sizeof(posy) / sizeof(s64), "out of range");

    {
        efloat x = pos.x;
        for (int j = 0; j <= pixels.width(); j++) {
            posx[j] = static_cast<s64>(x + 0.1);
            x += rect_sz;
        }
        efloat y = pos.y;
        for (int i = 0; i <= pixels.height(); i++) {
            posy[i] = static_cast<s64>(y + 0.1);
            y -= rect_sz;
        }
    }

    for (int i = 0; i < pixels.height(); i++) {
        for (int j = 0; j < pixels.width();) {
            int k = j;
            while (k < pixels.width() && pixels[i][k] == pixels[i][j]) {
                k++;
            }

            if (is_draw(pixels[i][j])) {
                draw_rect_in_pixels(
                    posx[j], posy[i + 1], posx[k], posy[i], func(pixels[i][j])
                );
            } else if (global_variables::debug_mode) {
                draw_rect_in_pixels(
                    posx[j], posy[i + 1], posx[k], posy[i], Color(0xffffff, 60)
                );
            }

            j = k;
        }
    }

#else
    /*efloat rect_sz = size * global_variables::scale_factor;

    pos.y *= global_variables::scale_factor;
    pos.y = int(pos.y);
    pos.y /= global_variables::scale_factor;

    pos.x *= global_variables::scale_factor;
    pos.x = int(pos.x);
    pos.x /= global_variables::scale_factor;*/

    int posx = (pos.x - size / 2 + global_variables::arena_half_size.x) *
               global_variables::scale_factor;
    int posy = (pos.y - size / 2 + global_variables::arena_half_size.y) *
               global_variables::scale_factor;
    posy -= pixels.height();

    int i0 = std::max(-posy, 0);
    int i1 = std::min(
        int(global_variables::render_state.height()) - posy,
        int(pixels.height())
    );

    int j0 = std::max(-posx, 0);
    int j1 = std::min(
        int(global_variables::render_state.width()) - posx, (int)pixels.width()
    );

    for (int i = i0; i < i1; i++) {
        for (int j = j0; j < j1; j++) {
            /*ASSERT(
                0 <= posx + j && posx + j <
            global_variables::render_state.width(), "something went wrong"
            );
            ASSERT(
                0 <= posy + i && posy + i <
            global_variables::render_state.height(), "something went wrong"
            );*/

            if (is_draw(pixels[i][j])) {
                // more faster
                // global_variables::render_state[posy + i][posx + j] =
                // pixels[i][j];
                global_variables::render_state[posy + i][posx + j] =
                    global_variables::render_state[posy + i][posx + j].combine(
                        func(pixels[i][j])
                    );
            } else if (global_variables::debug_mode) {
                global_variables::render_state[posy + i][posx + j] =
                    global_variables::render_state[posy + i][posx + j].combine(
                        Color(0xffffff, 60)
                    );
            }
        }
    }
#endif

    // efloat y = (pos.y + global_variables::arena_half_size.y - size / 2) *
    // global_variables::scale_factor;

    /*{
        efloat x = (pos.x + global_variables::arena_half_size.x - size / 2) *
    global_variables::scale_factor;

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

    // std::this_thread::yield();
    // wait_all_render_threads();

    /*for (int i = sprite_y0; i < sprite_y1; i++) {
        efloat x = (pos.x + global_variables::arena_half_size.x - size / 2) *
    global_variables::scale_factor;

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
                } else if (global_variables::debug_mode) {
                    draw_rect_in_pixels(x0, y0, x1, y1, Color(0xffffff, 60));
                }

                j = k;
            }
        }
        y -= rect_sz;
    }*/

    // pos += global_variables::arena_half_size;
    /*{
        efloat x = global_variables::arena_half_size.x;
        x *= global_variables::scale_factor;
        x = int(x);
        x /= global_variables::scale_factor;
        pos.x += x;

        efloat y = global_variables::arena_half_size.y;
        y *= global_variables::scale_factor;
        y = int(y);
        y /= global_variables::scale_factor;
        pos.y += y;
    }

    efloat y = 0;

    int posx = pos.x * global_variables::scale_factor;
    int posy = pos.y * global_variables::scale_factor;

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
            } else if (global_variables::debug_mode) {
                draw_rect_in_pixels(x0, y0, x1, y1, Color(0xffffff, 60));
            }

            x += rect_sz;
        }
        y -= rect_sz;
    }*/

    // не переливает пиксели, но трясет сам спрайт
    /*efloat y = (global_variables::arena_half_size.y - size / 2) *
    global_variables::scale_factor;

    int posx = pos.x * global_variables::scale_factor;
    int posy = pos.y * global_variables::scale_factor;

    for (int i = 0; i < pixels.height(); i++) {
        efloat x = (global_variables::arena_half_size.x - size / 2) *
    global_variables::scale_factor; for (int j = 0; j < pixels.width(); j++) {
            // change to pixels
            int x0 = static_cast<int>(x) + posx;
            int y0 = static_cast<int>(y) + posy;

            int x1 = static_cast<int>(x + size * global_variables::scale_factor)
    + posx; int y1 = static_cast<int>(y + size * global_variables::scale_factor)
    + posy;

            if (is_draw(pixels[i][j])) {
                draw_rect_in_pixels(x0, y0, x1, y1, func(pixels[i][j]));
            } else if (global_variables::debug_mode) {
                draw_rect_in_pixels(x0, y0, x1, y1, Color(0xffffff, 60));
            }
            x += size * global_variables::scale_factor;
        }
        y -= size * global_variables::scale_factor;
    }*/
}

#ifndef GAME_ENGINE_STANDARD_RENDER_SYSTEM
#include <map>

std::vector<std::map<efloat, Sprite>> Sprites_cache(SP_COUNT);
std::vector<std::map<efloat, std::map<unsigned int, Sprite>>>
    Spritesheets_cache(SS_COUNT);

void clear_sprites_cache() {
    for (auto &map : Sprites_cache) {
        map.clear();
    }
    for (auto &map : Spritesheets_cache) {
        map.clear();
    }
}

#endif

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
#ifdef GAME_ENGINE_STANDARD_RENDER_SYSTEM
        draw_sprite_matrix(pos, size, Sprites[sprite], func);
#else
        if (!Sprites_cache[sprite].count(size)) {
            global_time_accum++;
            // CALC_TIME_START
            Sprites_cache[sprite][size] = build_sprite_scaling(pixels, size);
            // CALC_TIME_END
            //  std::cout << "build" << std::endl;
        }
        draw_sprite_matrix(pos, size, Sprites_cache[sprite][size], func);
#endif
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

bool collision_in_draw_sprite(
    Dot pos,
    efloat size,
    sprite_t sprite,
    Dot collision_pos
) {
    static_pos_update(pos);

    const auto &pixels = Sprites[sprite];

    static const efloat epsilon = 1e-3;

    for (int i = 0; i < pixels.height(); i++) {
        for (int j = 0; j < pixels.width(); j++) {
            if (is_draw(pixels[i][j]) &&
                is_between(
                    pos.x + size * j - epsilon, collision_pos.x,
                    pos.x + size * (j + 1) + epsilon
                ) &&
                is_between(
                    pos.y - size * (i + 1) - epsilon, collision_pos.y,
                    pos.y - size * i + epsilon
                )) {
                return true;
            }
        }
    }
    return false;
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

#ifdef GAME_ENGINE_STANDARD_RENDER_SYSTEM
    draw_sprite_matrix(
        pos, size, Spritesheets[spritesheet][sprite_count], func
    );
#else
    if (!Spritesheets_cache[spritesheet][size].count(sprite_count)) {
        global_time_accum++;
        // CALC_TIME_START
        Spritesheets_cache[spritesheet][size][sprite_count] =
            build_sprite_scaling(pixels, size);
        // CALC_TIME_END
        // std::cout << "build" << std::endl;
    }

    draw_sprite_matrix(
        pos, size, Spritesheets_cache[spritesheet][size][sprite_count], func
    );
#endif
}

// рисует спрайт из листа спрайтов
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
