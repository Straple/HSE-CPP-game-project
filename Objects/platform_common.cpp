#include "platform_common.hpp"

//================//
//==ButtonsState==//
//================//

[[nodiscard]] bool ButtonsState::is_down(button_t button_id) const {
    return (button_is_down[button_id / 64] >> (button_id % 64)) & 1;
}

void ButtonsState::set_button(button_t button_id, bool value) {
    if (is_down(button_id) != value) {
        button_is_down[button_id / 64] ^=
            (static_cast<uint64_t>(1) << (button_id % 64));
    }
}

ButtonsState &ButtonsState::operator|=(const ButtonsState &rhs) {
    for (uint32_t index = 0; index < size; index++) {
        button_is_down[index] |= rhs.button_is_down[index];
    }
    return *this;
}

[[nodiscard]] ButtonsState
ButtonsState::operator|(const ButtonsState &rhs) const {
    return ButtonsState(*this) |= rhs;
}

//=========//
//==Input==//
//=========//

[[nodiscard]] bool Input::is_down(button_t b) const {
    return current.is_down(b);
}

[[nodiscard]] bool Input::is_changed(button_t b) const {
    return current.is_down(b) != previous.is_down(b);
}
