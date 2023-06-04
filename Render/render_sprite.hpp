#ifndef GAME_ENGINE_RENDER_SPRITE_HPP
#define GAME_ENGINE_RENDER_SPRITE_HPP

#include "../global_variables.hpp"
#include "../sprites.hpp"
#include "basic_render.hpp"
#include "render_utils.hpp"

// вызывается из других функций draw_sprite
template <typename func_t = dummy_color_func>
void draw_sprite_matrix(Dot pos, const efloat size, const Sprite &pixels, func_t &&func = dummy_color_func()) {
    pos += global_variables::arena_half_size;
    pos *= global_variables::scale_factor;

    efloat rect_sz = size * global_variables::scale_factor;

    std::vector<int64_t> posy(pixels.height() + 1), posx(pixels.width() + 1);

    {
        efloat x = pos.x;
        for (int j = 0; j <= pixels.width(); j++) {
            posx[j] = static_cast<int64_t>(x + 0.1);
            x += rect_sz;
        }
        efloat y = pos.y;
        for (int i = 0; i <= pixels.height(); i++) {
            posy[i] = static_cast<int64_t>(y + 0.1);
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
                draw_rect_in_pixels(posx[j], posy[i + 1], posx[k], posy[i], func(pixels[i][j]));
            } else if (global_variables::debug_mode) {
                draw_rect_in_pixels(posx[j], posy[i + 1], posx[k], posy[i], Color(0xffffff, 60));
            }

            j = k;
        }
    }
}

// рисует спрайт
template <typename func_t = dummy_color_func>
void draw_sprite_static(Dot pos, efloat size, sprite_t sprite, func_t &&func = dummy_color_func()) {
    auto &pixels = get_sprite(sprite);

    if (arena_query(
            pos.x - size / 2, pos.x + pixels.width() * size + size / 2, pos.y + size / 2,
            pos.y - pixels.height() * size - size / 2
        )) {
        draw_sprite_matrix(pos, size, pixels, func);
    }
}

template <typename func_t = dummy_color_func>
void draw_sprite(Dot pos, efloat size, sprite_t sprite, func_t &&func = dummy_color_func()) {
    static_pos_update(pos);

    draw_sprite_static(pos, size, sprite, func);
}

bool collision_in_draw_sprite(Dot pos, efloat size, sprite_t sprite, Dot collision_pos);

// WARNING NOT ARENA QUERY
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
    auto &pixels = get_sprite(texture);
    const efloat x_summary = pixels.width() * size;
    const efloat y_summary = pixels.height() * size;

    for (unsigned int i = 0; i < y_cnt; i++) {
        for (unsigned int j = 0; j < x_cnt; j++) {
            draw_sprite(Dot(pos.x + j * x_summary, pos.y - i * y_summary), size, texture, func);
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
    auto &pixels = get_sprite(texture);

    const efloat x_summary = pixels.width() * size;
    const efloat y_summary = pixels.height() * size;

    draw_texture(pos + 0.5 * Dot(-x_summary * x_cnt, y_summary * y_cnt), x_cnt, y_cnt, size, texture, func);
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

    auto &pixels = get_spritesheet(spritesheet)[sprite_count];

    if (arena_query(
            pos.x - half_size.x, pos.x + pixels.width() * size + half_size.x, pos.y + half_size.y,
            pos.y - pixels.height() * size - half_size.y
        )) {
        draw_sprite_matrix(pos, size, get_spritesheet(spritesheet)[sprite_count], func);
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

#endif  // GAME_ENGINE_RENDER_SPRITE_HPP