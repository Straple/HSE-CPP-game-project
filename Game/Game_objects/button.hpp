#ifndef GAME_BUTTON_HPP
#define GAME_BUTTON_HPP

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
    );

    // обновит состояние фокуса курсора
    void simulate(Cursor &cursor);

    void draw();
};

#endif  // GAME_BUTTON_HPP