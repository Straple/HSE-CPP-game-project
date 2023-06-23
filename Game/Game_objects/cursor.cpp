#include "cursor.hpp"

Cursor::Cursor(sprite_t sprite, sprite_t focus_sprite, efloat size)
        : sprite(sprite), focus_sprite(focus_sprite), size(size) {
}

void Cursor::simulate(const Input &input) {
    if (PRESSED(BUTTON_MOUSE_L)) {
        focus_pos = pos;
    }
    is_down = IS_DOWN(BUTTON_MOUSE_L);
    focus = false;
}

void Cursor::draw() {
    if (is_down) {
        Dot pos0(std::min(focus_pos.x, pos.x), std::max(focus_pos.y, pos.y));
        Dot pos1(std::max(focus_pos.x, pos.x), std::min(focus_pos.y, pos.y));
        //draw_rect2(pos0, pos1, Color(0xffffff, 64));
    }

    if (focus) {
        draw_sprite_static(pos - Dot(16, 0) * size, size, focus_sprite);
    } else {
        draw_sprite_static(pos, size, sprite);
    }
}
