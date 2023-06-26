#ifndef GAME_ENGINE_PLATFORM_COMMON_HPP
#define GAME_ENGINE_PLATFORM_COMMON_HPP

#include <cstdint>

// Множество кнопок для удобной работы с ними
enum button_t : uint8_t {
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_ENTER,
  BUTTON_ESC,
  BUTTON_TAB,
  BUTTON_SPACE,
  BUTTON_MOUSE_L,
  BUTTON_MOUSE_R,
  BUTTON_DEL,
  BUTTON_SHIFT,
  BUTTON_MINUS,
  BUTTON_PLUS,
  BUTTON_DOT,
  BUTTON_COMMA,
  BUTTON_COLON,
  BUTTON_QUESTION_MARK,
  BUTTON_BACKSPACE,

  BUTTON_A,
  BUTTON_B,
  BUTTON_C,
  BUTTON_D,
  BUTTON_E,
  BUTTON_F,
  BUTTON_G,
  BUTTON_H,
  BUTTON_I,
  BUTTON_J,
  BUTTON_K,
  BUTTON_L,
  BUTTON_M,
  BUTTON_N,
  BUTTON_O,
  BUTTON_P,
  BUTTON_Q,
  BUTTON_Z,
  BUTTON_V,
  BUTTON_R,
  BUTTON_S,
  BUTTON_T,
  BUTTON_U,
  BUTTON_W,
  BUTTON_X,
  BUTTON_Y,

  BUTTON_NUMPAD0,
  BUTTON_NUMPAD1,
  BUTTON_NUMPAD2,
  BUTTON_NUMPAD3,
  BUTTON_NUMPAD4,
  BUTTON_NUMPAD5,
  BUTTON_NUMPAD6,
  BUTTON_NUMPAD7,
  BUTTON_NUMPAD8,
  BUTTON_NUMPAD9,
  BUTTON_1,
  BUTTON_2,
  BUTTON_3,
  BUTTON_4,
  BUTTON_5,
  BUTTON_6,
  BUTTON_7,
  BUTTON_8,
  BUTTON_9,
  BUTTON_0,

  BUTTON_COUNT,
};

// состояние кнопок (нажата или нет)
class ButtonsState {
  static const uint32_t size = (BUTTON_COUNT + 64 - 1) / 64;
  uint64_t button_is_down[size]{};

public:
  [[nodiscard]] bool is_down(button_t button_id) const;

  void set_button(button_t button_id, bool value);

  ButtonsState &operator|=(const ButtonsState &rhs);

  [[nodiscard]] ButtonsState operator|(const ButtonsState &rhs) const;
};

struct Input {
  // состояние кнопок текущего кадра
  ButtonsState current;

  // состояние кнопок предыдущего кадра
  ButtonsState previous;

  Input() = default;

  [[nodiscard]] bool is_down(button_t b) const;

  [[nodiscard]] bool is_changed(button_t b) const;
};

#define IS_DOWN(b) (input.is_down(b))
#define PRESSED(b) (IS_DOWN(b) && input.is_changed(b))
#define RELEASED(b) (!IS_DOWN(b) && input.is_changed(b))

#endif // GAME_ENGINE_PLATFORM_COMMON_HPP