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

    Cursor(sprite_t sprite, sprite_t focus_sprite, efloat size);

    void simulate(const Input &input);

    void draw();
};

#endif  // GAME_CURSOR_HPP