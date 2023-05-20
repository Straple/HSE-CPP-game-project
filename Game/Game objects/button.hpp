#ifndef GAME_BUTTON_HPP
#define GAME_BUTTON_HPP

// TODO: refactor this code, he is so old

#include "../../Objects/Sprite/color.hpp"
#include "../../Objects/collision.hpp"
#include "../../global_variables.hpp"
#include "../../render.hpp"
#include "cursor.hpp"

// текст с колизией
struct button {
    std::string text;
    Dot pos;  // центр текста по x
    efloat size;
    CollisionBox coll;
    Color color,      // usual color
        focus_color;  // color when the button doesn't focus
    bool is_focus;

    button() {
    }

    button(
        std::string text,
        Dot pos,
        efloat size,
        Color color,
        Color focus_color
    )
        : text(text),
          pos(pos),
          color(color),
          focus_color(focus_color),
          size(size) {
        // create collision box
        {
            int len = text_len_in_pixels(text.c_str());

            coll.p0 = Dot(pos.x, pos.y);
            coll.p1 =
                Dot(pos.x + size * len - size, coll.p1.y = pos.y - size * 6);
        }
    }

    // обновит состояние фокуса курсора
    void simulate(Cursor &cursor) {
        if (coll.trigger(cursor.pos)) {
            cursor.focus = true;
            is_focus = true;
        } else {
            is_focus = false;
        }
    }

    void draw() {
        draw_text(text.c_str(), pos, size, (is_focus ? focus_color : color));

        draw_collision(coll);
    }
};

#endif  // GAME_BUTTON_HPP