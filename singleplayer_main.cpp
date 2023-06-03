
enum game_mode_t {
  GM_GAME,
  GM_CUSTOMIZATION,
  GM_MAIN_MENU,
  GM_IN_GAME_MENU,
};

static inline game_mode_t game_mode = GM_MAIN_MENU;
#include "window_handler.hpp"

int main() {
  setlocale(LC_ALL, "ru-RU");

  // initialize
  {
    std::cout << "performance_frequency: " << performance_frequency
              << std::endl;

    ShowWindow(GetConsoleWindow(),
               global_variables::show_console ? SW_SHOW : SW_HIDE);
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
      auto &player = Players[index];
      player.input = window_handler.input;
      auto &input = player.input;
      switch (game_mode) {
      case GM_MAIN_MENU: {
        clear_screen(Color(0xff4C4C4C));
        main_menu.simulate(window_handler.cursor, input);
        window_handler.cursor.draw();
        break;
      }
      case (GM_IN_GAME_MENU): {
        // test_room.draw();
        window_handler.draw_frame(delta_time, 0);
        clear_screen(Color(0x9f4C4C4C));
        // player.draw();
        // player.cursor_dir = window_handler.cursor.pos +
        // global_variables::camera.pos - player.pos;
        in_game_menu.simulate(window_handler.cursor, input);
        window_handler.cursor.draw();

        if (PRESSED(BUTTON_ESC)) {
          game_mode = GM_GAME;
          break;
        }

        break;
      }
      case GM_GAME: {
        if (PRESSED(BUTTON_ESC)) {
          game_mode = GM_IN_GAME_MENU;
          clear_screen(Color(0x7f4C4C4C));
          break;
        }
        if (PRESSED(BUTTON_C)) {
          game_mode = GM_CUSTOMIZATION;
          save_player_for_customization = player;
          player.now_is_customization = true;
          global_variables::camera.pos = player.pos;
        }
        simulate_player(delta_time, 0);
        simulate_game(delta_time);

        player.cursor_dir = window_handler.cursor.pos +
                            global_variables::camera.pos - player.pos;
        global_variables::camera.simulate(player.pos, delta_time);

        window_handler.draw_frame(delta_time, 0);
        break;
      }
      case GM_CUSTOMIZATION: {
        if (PRESSED(BUTTON_C)) {
          game_mode = GM_GAME;
          save_player_for_customization.t_shirt_color_id =
              player.t_shirt_color_id;
          save_player_for_customization.cloack_color_id =
              player.cloack_color_id;
          player = save_player_for_customization;
          global_variables::camera.pos = player.pos;
        }
        // false = t-shirt
        // true = cloack
        static bool change_mode = false;
        if (PRESSED(BUTTON_T)) {
          change_mode = !change_mode;
        }

        if (PRESSED(BUTTON_Q)) {
          if (change_mode) {
            if (player.cloack_color_id == 0) {
              player.cloack_color_id =
                  Player::customization_colors.size() / 2 - 1;
            } else {
              player.cloack_color_id--;
            }
          } else {
            if (player.t_shirt_color_id == 0) {
              player.t_shirt_color_id =
                  Player::customization_colors.size() / 2 - 1;
            } else {
              player.t_shirt_color_id--;
            }
          }
        }

        if (PRESSED(BUTTON_E)) {
          if (change_mode) {
            player.cloack_color_id++;
            if (player.cloack_color_id ==
                Player::customization_colors.size() / 2) {
              player.cloack_color_id = 0;
            }
          } else {
            player.t_shirt_color_id++;
            if (player.t_shirt_color_id ==
                Player::customization_colors.size() / 2) {
              player.t_shirt_color_id = 0;
            }
          }
        }
        simulate_player(delta_time, 0);
        global_variables::camera.pos = player.pos; // static camera

        clear_screen(GREY);
        player.draw();
        break;
      }
      default: {
        ASSERT(false, "unknown game mode");
      }
      }
      //--------------------------------------------------------
      // if (PRESSED(BUTTON_C)) {
      //    if (game_mode == GM_GAME) {
      //        game_mode = GM_CUSTOMIZATION;
      //        save_player_for_customization = player;
      //        player.now_is_customization = true;
      //        global_variables::camera.pos = player.pos;
      //    } else if (game_mode == GM_CUSTOMIZATION) {
      //        game_mode = GM_GAME;
      //        save_player_for_customization.t_shirt_color_id =
      //        player.t_shirt_color_id;
      //        save_player_for_customization.cloack_color_id =
      //        player.cloack_color_id; player = save_player_for_customization;
      //        global_variables::camera.pos = player.pos;
      //    } else {
      //        ASSERT(false, "game_mode = ?");
      //    }

      //}

      //// customize colors
      // if (game_mode == GM_CUSTOMIZATION) {
      //     // false = t-shirt
      //     // true = cloack
      //     static bool change_mode = false;
      //     if (PRESSED(BUTTON_T)) {
      //         change_mode = !change_mode;
      //     }

      //    if (PRESSED(BUTTON_Q)) {
      //        if (change_mode) {
      //            if (player.cloack_color_id == 0) {
      //                player.cloack_color_id =
      //                Player::customization_colors.size() / 2 - 1;
      //            } else {
      //                player.cloack_color_id--;
      //            }
      //        } else {
      //            if (player.t_shirt_color_id == 0) {
      //                player.t_shirt_color_id =
      //                Player::customization_colors.size() / 2 - 1;
      //            } else {
      //                player.t_shirt_color_id--;
      //            }
      //        }
      //    }

      //    if (PRESSED(BUTTON_E)) {
      //        if (change_mode) {
      //            player.cloack_color_id++;
      //            if (player.cloack_color_id ==
      //            Player::customization_colors.size() / 2) {
      //                player.cloack_color_id = 0;
      //            }
      //        } else {
      //            player.t_shirt_color_id++;
      //            if (player.t_shirt_color_id ==
      //            Player::customization_colors.size() / 2) {
      //                player.t_shirt_color_id = 0;
      //            }
      //        }
      //    }
      //}

      // if (game_mode == GM_GAME) {
      //     simulate_player(delta_time, 0);
      //     simulate_game(delta_time);

      //    player.cursor_dir = window_handler.cursor.pos +
      //    global_variables::camera.pos - player.pos;
      //    global_variables::camera.simulate(player.pos, delta_time);

      //    window_handler.draw_frame(delta_time, 0);
      //} else if (game_mode == GM_CUSTOMIZATION) {
      //    simulate_player(delta_time, 0);
      //    global_variables::camera.pos = player.pos; // static camera

      //    clear_screen(GREY);
      //    player.draw();
      //} else {
      //    ASSERT(false, "game_mode = ?");
      //}

      window_handler.release_frame();
    }

    // update time
    {
      u64 cur_time_tick = get_ticks();
      delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev) /
                   performance_frequency;
      time_tick_prev = cur_time_tick;
    }
  }
}