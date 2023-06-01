#include "game_mode.hpp"
//#include "window_handler.hpp"

int main() {
    setlocale(LC_ALL, "ru-RU");

    // initialize
    {
        std::cout << "performance_frequency: " << performance_frequency << std::endl;

        ShowWindow(GetConsoleWindow(), global_variables::show_console ? SW_SHOW : SW_HIDE);
        ShowCursor(global_variables::show_cursor);

        read_sprites();
        read_spritesheets();

        test_room.read("level.txt");

        // создадим персонажа
        Players.emplace_back(Dot(25, -100));
        Players.back().client_id = 0;
    }

    WindowHandler window_handler;

    u64 time_tick_global_start = get_ticks();
    u64 time_tick_prev = time_tick_global_start;

    efloat delta_time = 0;

    Player customization_player;

    while (global_variables::running) {
        auto &player = Players[0];

        simulate_game_mode(delta_time, player, customization_player, window_handler);

        draw_game_mode(delta_time, player, customization_player, window_handler);

        // update time
        {
            u64 cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev) / performance_frequency;
            time_tick_prev = cur_time_tick;
        }
    }
}