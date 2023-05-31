#include "window_handler.hpp"

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

    Player save_player_for_customization;

    while (global_variables::running) {
        // simulate frame
        {
            window_handler.update();

            int index = find_player_index(0);
            Players[index].input = window_handler.input;
            auto &input = Players[index].input;

            if (PRESSED(BUTTON_G)) {
                if (game_mode == GM_GAME) {
                    game_mode = GM_CUSTOMIZATION;
                    save_player_for_customization = Players[index];
                    Players[index].now_is_customization = true;
                } else if (game_mode = GM_CUSTOMIZATION) {
                    game_mode = GM_GAME;
                    save_player_for_customization.t_shirt_color_id = Players[index].t_shirt_color_id;
                    save_player_for_customization.cloack_color_id = Players[index].cloack_color_id;
                    Players[index] = save_player_for_customization;
                } else {
                    ASSERT(false, "game_mode = ?");
                }
            }

            if (game_mode == GM_CUSTOMIZATION) {
                simulate_player(delta_time, 0);
                global_variables::camera.simulate(Players[index].pos, delta_time);
            } else {
                simulate_player(delta_time, 0);
                simulate_game(delta_time);

                Players[index].cursor_dir =
                    window_handler.cursor.pos + global_variables::camera.pos - Players[index].pos;
                global_variables::camera.simulate(Players[index].pos, delta_time);
            }

            window_handler.draw_frame(delta_time, 0);
            window_handler.release_frame();
        }

        // update time
        {
            u64 cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev) / performance_frequency;
            time_tick_prev = cur_time_tick;
        }
    }
}