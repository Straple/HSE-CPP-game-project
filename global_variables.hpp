#ifndef GLOBAL_VARIABLES_HPP
#define GLOBAL_VARIABLES_HPP

#include "Objects\objects.h"

struct global_variables {
    static inline bool camera_is_static = false;
    static inline bool debug_mode = false;
    static inline bool show_locator = false;
    static inline bool running = true;

    static inline bool show_console = true;
    static inline bool show_cursor = false;
    static inline bool show_fps = true;

    static inline bool fullscreen_mode = true;

    static inline Camera camera;

    static inline Render_state render_state;

    static inline Dot arena_half_size;

    // масштаб рендеринга
    static inline efloat render_scale = 0.01;

    // множитель масштабирования = global_variables::render_state.height *
    // global_variables::render_scale
    static inline efloat scale_factor = 0;
};

#endif  // GLOBAL_VARIABLES_HPP