#ifndef GAME_ENGINE_BUTTON_HPP
#define GAME_ENGINE_BUTTON_HPP

#include "Sprite/color.hpp"
#include "collision.hpp"
#include "render.hpp"

// текст с колизией
struct button {
    std::string text;
    Dot pos;  // центр текста по x
    efloat size;
    collision_box coll;
    Color color,      // usual color
        focus_color;  // color when the button doesn't focus
    bool is_align, is_focus;

    button() {
    }

    button(
        std::string text,
        Dot pos,
        efloat size,
        Color color,
        Color focus_color,
        bool is_align = false
    )
        : text(text),
          pos(pos),
          color(color),
          focus_color(focus_color),
          size(size),
          is_align(is_align) {
        // create collision box
        /*{
            int len = text_len(_text);

            coll.p0 = Dot(_pos.x + (is_align ? -(size * len) * 0.5 : 0), _pos.y);
            coll.p1.x =
                _pos.x + size * len - size + (is_align ? -(size * len) * 0.5 : 0);
            coll.p1.y = _pos.y - size * 6 - size;
        }*/
    }

    void draw() {
        if (is_align) {
            draw_text_align(text.c_str(), pos, size, (is_focus ? focus_color : color));
        } else {
            draw_text(text.c_str(), pos, size, (is_focus ? focus_color : color));
        }

        if (global_variables::debug_mode) {
            draw_rect(coll.p0, Dot(0.5, 0.5), 0xff00ff00);
            draw_rect(coll.p1, Dot(0.5, 0.5), 0xff00ff00);
        }
    }

    // обновит состояние фокуса курсора
    void simulate(Dot &cursor_pos) {
        if (coll.trigger(cursor_pos)) {
            //cursor.focus = true;
            is_focus = true;
        } else {
            is_focus = false;
        }
    }
};

#endif  // GAME_ENGINE_BUTTON_HPP