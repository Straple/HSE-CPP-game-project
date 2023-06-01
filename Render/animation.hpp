#ifndef GAME_ENGINE_ANIMATION_HPP
#define GAME_ENGINE_ANIMATION_HPP

#include "../Objects/Geometry/dot.hpp"
#include "../sprites.hpp"
#include "render_utils.hpp"

struct animation {
    efloat frame_duration = 0;    // продолжительность кадра
    efloat frame_time_accum = 0;  // время накопления продолжительности кадра

    mutable spritesheet_t sprite_sheet = spritesheet_t::SS_COUNT;  // лист спрайтов
    uint8_t frame_begin = 0;      // начало кадра в листе спрайтов
    uint8_t frame_count = 0;      // количество кадров в анимации
    uint8_t frame_cur_count = 0;  // счетчик текущего кадра с 0

    animation() = default;

    animation(spritesheet_t sprite_sheet, uint8_t frame_begin, uint8_t frame_count, efloat frame_duration);

    void reset();

    // вернет true, если прошел полный веток анимации
    bool frame_update(efloat delta_time);

    template <typename func_t = dummy_color_func>
    void draw(Dot pos, efloat size, func_t &&func = dummy_color_func()) const {
        draw_spritesheet(pos, size, sprite_sheet, frame_begin + frame_cur_count, func);
    }
};

#endif  // GAME_ENGINE_ANIMATION_HPP