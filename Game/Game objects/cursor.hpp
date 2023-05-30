#ifndef GAME_CURSOR_HPP
#define GAME_CURSOR_HPP

#include "../../Objects/platform_common.hpp"
#include "../../render.hpp"
#include "../../sprites.hpp"

struct Cursor {
    sprite_t sprite;
    sprite_t focus_sprite;

    efloat size;

    Dot pos;
    Dot focus_pos;  // позиция точки, когда мы нажали на мышку и удерживали

    bool focus = false;
    bool is_down = false;

    Cursor(sprite_t sprite, sprite_t focus_sprite, efloat size)
        : sprite(sprite), focus_sprite(focus_sprite), size(size) {
    }

    void simulate(const Input &input) {
        if (PRESSED(BUTTON_MOUSE_L)) {
            focus_pos = pos;
        }
        is_down = IS_DOWN(BUTTON_MOUSE_L);
        focus = false;
    }

    void draw() const {
        if (is_down) {
            Dot pos0(std::min(focus_pos.x, pos.x), std::max(focus_pos.y, pos.y));
            Dot pos1(std::max(focus_pos.x, pos.x), std::min(focus_pos.y, pos.y));
            draw_rect2(pos0, pos1, Color(0xffffff, 64));
        }

        if (focus) {
            draw_sprite_static(pos - Dot(16, 0) * size, size, focus_sprite);
        } else {
            draw_sprite_static(pos, size, sprite);
        }
    }
};

#endif  // GAME_CURSOR_HPP