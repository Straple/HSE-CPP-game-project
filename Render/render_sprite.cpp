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

struct dummy_color_func {
    inline Color operator()(const Color &color) const {
        return color;
    }
};

template <u8 alpha>
struct alpha_color_func {
    inline Color operator()(const Color &color) const {
        return {color.r, color.g, color.b, alpha};
    }
};

using shadow_color_func = alpha_color_func<100>;

// вызывается из других функций draw_sprite
template <typename func_t = dummy_color_func>
void draw_sprite_matrix(
    Dot pos,
    const efloat size,
    const Sprite &pixels,
    func_t &&func = dummy_color_func()
) {
    pos += global_variables::arena_half_size;
    pos *= global_variables::scale_factor;

    efloat rect_sz = size * global_variables::scale_factor;

    std::vector<s64> posy(pixels.height() + 1), posx(pixels.width() + 1);

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
}

// рисует спрайт
template <typename func_t = dummy_color_func>
void draw_sprite_static(
    Dot pos,
    efloat size,
    sprite_t sprite,
    func_t &&func = dummy_color_func()
) {
    auto &pixels = Sprites[sprite];

    if (arena_query(
            pos.x - size / 2, pos.x + pixels.width() * size + size / 2,
            pos.y + size / 2, pos.y - pixels.height() * size - size / 2
        )) {
        draw_sprite_matrix(pos, size, pixels, func);
    }
}

template <typename func_t = dummy_color_func>
void draw_sprite(
    Dot pos,
    efloat size,
    sprite_t sprite,
    func_t &&func = dummy_color_func()
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
template <typename func_t = dummy_color_func>
void draw_texture(
    Dot pos,
    unsigned int x_cnt,
    unsigned int y_cnt,
    efloat size,
    sprite_t texture,
    func_t &&func = dummy_color_func()
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
template <typename func_t = dummy_color_func>
void draw_texture_align(
    Dot pos,
    unsigned int x_cnt,
    unsigned int y_cnt,
    efloat size,
    sprite_t texture,
    func_t &&func = dummy_color_func()
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
template <typename func_t = dummy_color_func>
void draw_spritesheet_static(
    Dot pos,
    efloat size,
    spritesheet_t spritesheet,
    unsigned int sprite_count,
    func_t &&func = dummy_color_func()
) {
    Dot half_size = Dot(0.5, 0.5) * size;

    auto &pixels = Spritesheets[spritesheet][sprite_count];

    if (arena_query(
            pos.x - half_size.x, pos.x + pixels.width() * size + half_size.x,
            pos.y + half_size.y, pos.y - pixels.height() * size - half_size.y
        )) {
        draw_sprite_matrix(
            pos, size, Spritesheets[spritesheet][sprite_count], func
        );
    }
}

// рисует спрайт из листа спрайтов
// sprite_count - идентификатор спрайта
template <typename func_t = dummy_color_func>
void draw_spritesheet(
    Dot pos,
    efloat size,
    spritesheet_t spritesheet,
    unsigned int sprite_count,
    func_t &&func = dummy_color_func()
) {
    static_pos_update(pos);

    draw_spritesheet_static(pos, size, spritesheet, sprite_count, func);
}
