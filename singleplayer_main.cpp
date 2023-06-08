#include "game_mode.hpp"

// #include "window_handler.hpp"

int main() {
    setlocale(LC_ALL, "ru-RU");

    // initialize
    {
        std::cout << "performance_frequency: " << get_performance_frequency() << std::endl;

        ShowWindow(GetConsoleWindow(), global_variables::show_console ? SW_SHOW : SW_HIDE);
        ShowCursor(global_variables::show_cursor);

        read_sprites();
        read_spritesheets();

        test_room.read("level.txt");

        // создадим персонажа
        game_variables::Players.emplace_back(Dot(25, -100));
        game_variables::Players.back().client_id = 0;
    }

    WindowHandler window_handler;

    uint64_t time_tick_global_start = get_ticks();
    uint64_t time_tick_prev = time_tick_global_start;

    efloat delta_time = 0;

    Player customization_player;

    Typer typer;

    while (global_variables::running) {
        auto &player = game_variables::Players[0];

        simulate_game_mode(delta_time, player, customization_player, window_handler, typer);

        draw_game_mode(delta_time, 0, customization_player, window_handler, typer);

        // update time
        {
            uint64_t cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev) / get_performance_frequency();
            time_tick_prev = cur_time_tick;
        }
    }
}