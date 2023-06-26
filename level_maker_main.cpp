#include "window_handler.hpp"

int main() {
    setlocale(LC_ALL, "ru-RU");

    // initialize
    {
        std::cout << "performance_frequency: " << get_performance_frequency()
                  << std::endl;

        ShowWindow(
            GetConsoleWindow(),
            global_variables::show_console ? SW_SHOW : SW_HIDE
        );
        ShowCursor(global_variables::show_cursor);

        read_sprites();
        read_spritesheets();

        Objects = current_room.read("level.txt");
    }

    WindowHandler window_handler;

    uint64_t time_tick_global_start = get_ticks();
    uint64_t time_tick_prev = time_tick_global_start;

    efloat delta_time = 0;

    Player customization_player;

    while (global_variables::running) {
        window_handler.update_controls();
        window_handler.simulate_input(delta_time);

        simulate_leve_maker(
            window_handler.input, window_handler.cursor.pos, delta_time
        );

        draw_debug_info(delta_time, window_handler.cursor);

        window_handler.release_frame();

        // update time
        {
            uint64_t cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev) /
                         get_performance_frequency();
            time_tick_prev = cur_time_tick;
        }
    }
}