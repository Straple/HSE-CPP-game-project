
#include "../utils.h"

enum engine_state_type : u64 {

	RUN_SETUP = (static_cast<u64>(1) << 63),

	DEBUG = 1,
	CAMERA = 2,
	FULLSCREEN = 4,

	LOCATOR_VIS = 8,
	CURSOR_VIS = 16,
	CONSOLE_VIS = 32,
	FPS_VIS = 64,

	ALL_SETUPS = RUN_SETUP | DEBUG | CAMERA | FULLSCREEN | LOCATOR_VIS | CURSOR_VIS | CONSOLE_VIS | FPS_VIS,
};

class Engine_state {

	u64 mask;

public:

	Engine_state(u64 msk) {
		mask = msk;
	}

	Engine_state() {
		mask = 0;
	}

	bool debug_mode() const {
		return mask & DEBUG;
	}
	bool camera_mode() const {
		return mask & CAMERA;
	}
	bool fullscreen_mode() const {
		return mask & FULLSCREEN;
	}

	bool show_locator() const {
		return mask & LOCATOR_VIS;
	}
	bool show_cursor() const {
		return mask & CURSOR_VIS;
	}
	bool show_console() const {
		return mask & CONSOLE_VIS;
	}
	bool show_fps() const {
		return mask & FPS_VIS;
	}

	bool run() const {
		return mask & RUN_SETUP;
	}

	void flip(u64 msk) {
		mask ^= msk;
	}
};
