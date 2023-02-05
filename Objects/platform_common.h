#pragma once

// КНОПКИ
enum button_t : u8 {
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

	BUTTON_A,
	BUTTON_B,
	BUTTON_C,
	BUTTON_D,
	BUTTON_E,
	BUTTON_F,
	BUTTON_W,
	BUTTON_S,
	BUTTON_J,
	BUTTON_K,

	BUTTON_0,
	BUTTON_1,
	BUTTON_2,
	BUTTON_3,
	BUTTON_4,
	BUTTON_5,
	BUTTON_6,
	BUTTON_7,
	BUTTON_8,
	BUTTON_9,

	BUTTON_COUNT,
};

struct Input {
	u64 buttons_state[(BUTTON_COUNT * 2 + 64 - 1) / 64];

	Input() {
		for (u32 i = 0; i < (BUTTON_COUNT * 2 + 64 - 1) / 64; i++) {
			buttons_state[i] = 0;
		}
	}

	bool button_is_down(button_t b) const {
		return (buttons_state[b / 32] >> b % 32 * 2) & 1;
	}

	bool button_changed(button_t b) const {
		return (buttons_state[b / 32] >> b % 32 * 2 + 1) & 1;
	}

	void set_button(button_t b, bool is_down, bool changed) {
		if (button_is_down(b) != is_down) {
			buttons_state[b / 32] ^= (1ULL << b % 32 * 2);
		}
		if (button_changed(b) != changed) {
			buttons_state[b / 32] ^= (1ULL << b % 32 * 2 + 1);
		}
	}
};

#define is_down(b)  (input.button_is_down(b))
#define pressed(b)  (is_down(b) && input.button_changed(b))
#define released(b) (!is_down(b) && input.button_changed(b))
