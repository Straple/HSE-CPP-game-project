#include "typer.hpp"

Typer::Typer() {
    button_hold_time.resize(BUTTON_COUNT);
}

void Typer::clear() {
    text.clear();
}

void Typer::simulate(const Input &input, efloat delta_time) {
    bool shift_is_down = IS_DOWN(BUTTON_SHIFT);

#define BUTTON_HANDLER(button, with_shift, without_shift)         \
    if (PRESSED(button)) {                                        \
        text += (shift_is_down) ? with_shift : without_shift;     \
        button_hold_time[button] = 0;                             \
    } else if (IS_DOWN(button)) {                                 \
        button_hold_time[button] += delta_time;                   \
        if (button_hold_time[button] >= 0.25) {                   \
            button_hold_time[button] = 0;                         \
            text += (shift_is_down) ? with_shift : without_shift; \
        }                                                         \
    }

    // TODO: можно использовать макро циклы для еще более сжатого кода

    BUTTON_HANDLER(BUTTON_A, 'A', 'a');
    BUTTON_HANDLER(BUTTON_B, 'B', 'b');
    BUTTON_HANDLER(BUTTON_C, 'C', 'c');
    BUTTON_HANDLER(BUTTON_D, 'D', 'd');
    BUTTON_HANDLER(BUTTON_E, 'E', 'e');
    BUTTON_HANDLER(BUTTON_F, 'F', 'f');
    BUTTON_HANDLER(BUTTON_G, 'G', 'g');
    BUTTON_HANDLER(BUTTON_H, 'H', 'h');
    BUTTON_HANDLER(BUTTON_I, 'I', 'i');
    BUTTON_HANDLER(BUTTON_J, 'J', 'j');
    BUTTON_HANDLER(BUTTON_K, 'K', 'k');
    BUTTON_HANDLER(BUTTON_L, 'L', 'l');
    BUTTON_HANDLER(BUTTON_M, 'M', 'm');
    BUTTON_HANDLER(BUTTON_N, 'N', 'n');
    BUTTON_HANDLER(BUTTON_O, 'O', 'o');
    BUTTON_HANDLER(BUTTON_P, 'P', 'p');
    BUTTON_HANDLER(BUTTON_Q, 'Q', 'q');
    BUTTON_HANDLER(BUTTON_R, 'R', 'r');
    BUTTON_HANDLER(BUTTON_S, 'S', 's');
    BUTTON_HANDLER(BUTTON_T, 'T', 't');
    BUTTON_HANDLER(BUTTON_U, 'U', 'u');
    BUTTON_HANDLER(BUTTON_V, 'V', 'v');
    BUTTON_HANDLER(BUTTON_W, 'W', 'w');
    BUTTON_HANDLER(BUTTON_X, 'X', 'x');
    BUTTON_HANDLER(BUTTON_Y, 'Y', 'y');
    BUTTON_HANDLER(BUTTON_Z, 'Z', 'z');

    BUTTON_HANDLER(BUTTON_1, "!", "1");
    BUTTON_HANDLER(BUTTON_2, "", "2");
    BUTTON_HANDLER(BUTTON_3, "", "3");
    BUTTON_HANDLER(BUTTON_4, "", "4");
    BUTTON_HANDLER(BUTTON_5, "", "5");
    BUTTON_HANDLER(BUTTON_6, "", "6");
    BUTTON_HANDLER(BUTTON_7, "", "7");
    BUTTON_HANDLER(BUTTON_8, "", "8");
    BUTTON_HANDLER(BUTTON_9, "(", "9");
    BUTTON_HANDLER(BUTTON_0, ")", "0");

    BUTTON_HANDLER(BUTTON_NUMPAD1, "1", "!");
    BUTTON_HANDLER(BUTTON_NUMPAD2, "2", "");
    BUTTON_HANDLER(BUTTON_NUMPAD3, "3", "");
    BUTTON_HANDLER(BUTTON_NUMPAD4, "4", "");
    BUTTON_HANDLER(BUTTON_NUMPAD5, "5", "");
    BUTTON_HANDLER(BUTTON_NUMPAD6, "6", "");
    BUTTON_HANDLER(BUTTON_NUMPAD7, "7", "");
    BUTTON_HANDLER(BUTTON_NUMPAD8, "8", "");
    BUTTON_HANDLER(BUTTON_NUMPAD9, "9", "(");
    BUTTON_HANDLER(BUTTON_NUMPAD0, "0", ")");

    BUTTON_HANDLER(BUTTON_DOT, "", ".");
    BUTTON_HANDLER(BUTTON_MINUS, "_", "-");
    BUTTON_HANDLER(BUTTON_PLUS, "+", "");
    BUTTON_HANDLER(BUTTON_COMMA, ",", "");
    BUTTON_HANDLER(BUTTON_QUESTION_MARK, "?", "");
    BUTTON_HANDLER(BUTTON_SPACE, " ", " ");

    if (PRESSED(BUTTON_BACKSPACE) && !text.empty()) {
        text.pop_back();
    } else if (IS_DOWN(BUTTON_BACKSPACE)) {
        button_hold_time[BUTTON_BACKSPACE] += delta_time;
        if (button_hold_time[BUTTON_BACKSPACE] >= 0.25) {
            button_hold_time[BUTTON_BACKSPACE] = 0;
            text.pop_back();
        }
    }

#undef BUTTON_HANDLER
}
