#ifndef GLOBAL_VARIABLES_HPP
#define GLOBAL_VARIABLES_HPP

#include "Objects/camera.hpp"
#include "Objects/render_state.hpp"

struct global_variables {
    static inline bool camera_is_static = false;
    static inline bool debug_mode = false;
    static inline bool show_locator = false;
    static inline bool running = true;

    static inline bool show_console = true;
    static inline bool show_cursor = false;
    static inline bool show_fps = false;

    static inline bool fullscreen_mode = true;

    static inline Camera camera;

    static inline RenderState render_state;

    static inline Dot arena_half_size;

    // масштаб рендеринга
    static inline efloat render_scale = 0.007;

    // множитель масштабирования = global_variables::render_state.height() *
    // global_variables::render_scale
    static inline efloat scale_factor = 0;

    static inline efloat mouse_wheel = 0;
};

#endif  // GLOBAL_VARIABLES_HPP