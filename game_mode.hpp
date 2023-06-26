#ifndef GAME_ENGINE_GAME_MODE_HPP
#define GAME_ENGINE_GAME_MODE_HPP

#include "Game/Game_objects/menu.hpp"
#include "window_handler.hpp"
#include <string>

std::string server_address;

struct chat_message_object {
  int client_id;
  std::string message;
  efloat time; // накопленное время существования этого сообщения
};

std::vector<chat_message_object> ChatMessages;

// симулирует игру или кастомизацию игрока
void simulate_game_mode(efloat delta_time, Player &player,
                        Player &customization_player,
                        WindowHandler &window_handler, Typer &typer) {
  window_handler.update_controls();
  const auto input = window_handler.input;
  switch (game_mode) {
  case (GM_GAME): {
    if (PRESSED(BUTTON_ESC)) {
      game_mode = GM_PAUSE_MENU;
      break;
    }
    if (PRESSED(BUTTON_ENTER)) {
      typer.clear();
      now_is_typing = !now_is_typing;
    } else if (!now_is_typing) {
      customization_player.input = player.input = input;
      window_handler.simulate_input(delta_time);
    }

    if (!now_is_typing && PRESSED(BUTTON_C)) {
      game_mode = GM_CUSTOMIZATION;
      customization_player = player;
      customization_player.now_is_customization = true;
      global_variables::camera.pos = player.pos;
    }

    // мы должны симулировать свою версию игры,
    // то есть мы пару кадров будем жить в своем собственном мире,
    // а когда получим от сервера его версию игры, то переключимся на нее
    // таким образом игра становиться более плавной и менее зависящей от сервера
    // если долго не будут приходить пакеты от сервера, то клиент еще сможет
    // играть

    player.cursor_dir =
        window_handler.cursor.pos + global_variables::camera.pos - player.pos;
    global_variables::camera.simulate(player.pos, delta_time);
    // симулируем игроков
    for (auto &player : game_variables::Players) {
      simulate_player(delta_time, player.client_id);
    }

    if (now_is_typing) {
      typer.simulate(input, delta_time);
      // ограничить размером окошка
      while (!typer.text.empty() &&
             text_len_in_pixels(typer.text.c_str()) > 150) {
        typer.text.pop_back();
      }
    }

    // симулируем игру
    simulate_game(delta_time);

    break;
  }
  case (GM_CUSTOMIZATION): {
    if (!now_is_typing && PRESSED(BUTTON_C)) {
      game_mode = GM_GAME;
      player.cloack_color_id = customization_player.cloack_color_id;
      player.t_shirt_color_id = customization_player.t_shirt_color_id;
      global_variables::camera.pos = player.pos;
    }

    customization_player.input = input;
    window_handler.simulate_input(delta_time);

    simulate_player(delta_time, customization_player);
    global_variables::camera.pos = customization_player.pos; // static camera

    // false = t-shirt
    // true = cloack
    static bool change_mode = false;
    if (PRESSED(BUTTON_T)) {
      change_mode = !change_mode;
    }

    if (PRESSED(BUTTON_Q)) {
      if (change_mode) {
        if (customization_player.cloack_color_id == 0) {
          customization_player.cloack_color_id =
              Player::customization_colors.size() / 2 - 1;
        } else {
          customization_player.cloack_color_id--;
        }
      } else {
        if (customization_player.t_shirt_color_id == 0) {
          customization_player.t_shirt_color_id =
              Player::customization_colors.size() / 2 - 1;
        } else {
          customization_player.t_shirt_color_id--;
        }
      }
    }

    if (PRESSED(BUTTON_E)) {
      if (change_mode) {
        customization_player.cloack_color_id++;
        if (customization_player.cloack_color_id ==
            Player::customization_colors.size() / 2) {
          customization_player.cloack_color_id = 0;
        }
      } else {
        customization_player.t_shirt_color_id++;
        if (customization_player.t_shirt_color_id ==
            Player::customization_colors.size() / 2) {
          customization_player.t_shirt_color_id = 0;
        }
      }
    }

    break;
  }
  case (GM_MAIN_MENU): {
    main_menu.simulate(window_handler.cursor, input);

    if (game_mode == GM_GAME) {
      test_room.read("0-lobby-level.txt");
      // test_room.read("1-forest-level.txt");
      // test_room.read("2-dungeon-level.txt");
      game_variables::Players = {Dot(25, -100)};
      game_variables::Players.back().client_id = 0;
    }
    break;
  }
  case (GM_PAUSE_MENU): {
    in_game_menu.simulate(window_handler.cursor, input);
    if (PRESSED(BUTTON_ESC)) {
      game_mode = GM_GAME;
    }
    break;
  }
  case (GM_SETTINGS_MENU): {
    settings_menu.simulate(window_handler.cursor, input);
    if (PRESSED(BUTTON_ESC)) {
      game_mode = GM_PAUSE_MENU;
    }
    break;
  }
  case (GM_MULTIPLAYER_MENU): {
    now_is_typing = true;
    if (PRESSED(BUTTON_ESC)) {
      game_mode = GM_MAIN_MENU;
      now_is_typing = false;
    }
    if (now_is_typing) {
      typer.simulate(input, delta_time);
      // ограничить размером окошка
      while (!typer.text.empty() &&
             text_len_in_pixels(typer.text.c_str()) > 150) {
        typer.text.pop_back();
      }
    }
    if (PRESSED(BUTTON_ENTER)) {
      // Тут надо вытащить адресс к которому попытаемся подключиться
      // Пока что с ним я ничего не делаю
      server_address = typer.text;
      typer.clear();
      now_is_typing = !now_is_typing;
      game_mode = GM_MULTIPLAYER;
    }

    multiplayer_menu.simulate(window_handler.cursor, input);
    break;
  }
  case (GM_MAIN_MENU_SETTINGS): {
    settings_menu_in_main.simulate(window_handler.cursor, input);
    break;
  }
  default: {
    ASSERT(false, "unknown_game_mode");
  }
  }

  // по истечению 10s сообщение из чата удаляется
  for (int index = 0; index < ChatMessages.size(); index++) {
    ChatMessages[index].time += delta_time;
    if (ChatMessages[index].time > 10) {
      ChatMessages.erase(ChatMessages.begin() + index);
      index--;
    }
  }
}

void draw_chat_messages() {
  if (game_mode != GM_GAME) {
    return;
  }
  for (auto &player : game_variables::Players) {
    int i = 0;
    for (auto [sender_client_id, message, time] : ChatMessages) {
      // это сообщение отправил этот игрок
      if (player.client_id == sender_client_id) {
        efloat message_alpha_factor = 1;
        if (time > 5) {
          message_alpha_factor = (10 - time) / 5;
        }

        draw_text_align(
            message.c_str(),
            player.pos - global_variables::camera.pos + Dot(0, 30 + i * 10), 1,
            Color(0x00ff00, static_cast<uint8_t>(message_alpha_factor * 0xff)));
        i++;
      }
    }
  }
}

// рисует игру или кастомизацию игрока
void draw_game_mode(efloat delta_time, int client_id,
                    Player &customization_player, WindowHandler &window_handler,
                    Typer &typer) {
  switch (game_mode) {
  case (GM_GAME): {
    window_handler.draw_frame(delta_time, client_id);
    if (now_is_typing) {
      draw_rect2(-global_variables::arena_half_size + Dot(10, 20),
                 -global_variables::arena_half_size + Dot(162, 10), BLACK);
      efloat text_size = 1;
      /*efloat text_size = 150.0 / text_len_in_pixels(typer.get_text().c_str());
      if (text_size > 1) {
          text_size = 1;
      }*/
      draw_text(typer.text.c_str(),
                -global_variables::arena_half_size + Dot(12, 18), text_size,
                WHITE);
    }

    if (now_is_typing) {
      draw_rect2(-global_variables::arena_half_size + Dot(10, 20),
                 -global_variables::arena_half_size + Dot(162, 10), BLACK);
      efloat text_size = 1;
      /*efloat text_size = 150.0 / text_len_in_pixels(typer.get_text().c_str());
      if (text_size > 1) {
          text_size = 1;
      }*/
      draw_text(typer.text.c_str(),
                -global_variables::arena_half_size + Dot(12, 18), text_size,
                WHITE);
    }
    draw_chat_messages();
    break;
  }
  case (GM_CUSTOMIZATION): {
    clear_screen(GREY);
    customization_player.draw();

    // draw colors palette
    {
      int count_colors = Player::customization_colors.size() / 2;
      Dot start_pos = Dot(-30, -15);
      for (int color_id = 0; color_id < count_colors; color_id++) {
        if (customization_player.t_shirt_color_id == color_id) {
          draw_rect(start_pos + Dot(color_id * 8, 0), Dot(4, 4), WHITE);
        }
        if (customization_player.cloack_color_id == color_id) {
          draw_rect(start_pos + Dot(color_id * 8, 0), Dot(3, 3), BLACK);
        }
        draw_rect(start_pos + Dot(color_id * 8, 0), Dot(2, 2),
                  Player::customization_colors[2 * color_id]);
      }
    }

    draw_text_align("PLAYER CUSTOMIZATION", Dot(0, 35), 1, GREEN);

    draw_text_align(
        "press T to change the choice of customization (t-shirt or cloack)",
        Dot(0, -30), 0.5, GREEN);
    draw_text_align("press Q or E to change the choice of color", Dot(0, -40),
                    0.5, GREEN);
    draw_text_align("use W,A,S,D to walk and SPACE to jump", Dot(0, -50), 0.5,
                    GREEN);
    break;
  }
  case (GM_PAUSE_MENU): {
    window_handler.draw_frame(delta_time, client_id);
    clear_screen(Color(0x9f4C4C4C));
    in_game_menu.draw(window_handler.cursor);
    window_handler.cursor.draw();
    break;
  }
  case (GM_MULTIPLAYER_MENU): {
    clear_screen(Color(0xff4C4C4C));
    draw_rect2(-global_variables::arena_half_size + Dot(50, 80),
               -global_variables::arena_half_size + Dot(200, 70), BLACK);
    // if (now_is_typing) {
    draw_text(typer.text.c_str(),
              -global_variables::arena_half_size + Dot(52, 78), 1, WHITE);
    //}
    multiplayer_menu.draw(window_handler.cursor);
    window_handler.cursor.draw();
    break;
  }
  case (GM_MAIN_MENU): {
    clear_screen(Color(0xff4C4C4C));
    main_menu.draw(window_handler.cursor);
    window_handler.cursor.draw();
    break;
  }
  case (GM_MAIN_MENU_SETTINGS): {
    clear_screen(Color(0xff4C4C4C));
    settings_menu_in_main.draw(window_handler.cursor);
    window_handler.cursor.draw();
    break;
  }
  case (GM_SETTINGS_MENU): {
    window_handler.draw_frame(delta_time, client_id);
    clear_screen(Color(0x9f4C4C4C));
    settings_menu.draw(window_handler.cursor);
    window_handler.cursor.draw();
    break;
  }
  default: {
  }
  }

  window_handler.release_frame();
}

#endif // GAME_ENGINE_GAME_MODE_HPP