#ifndef GAME_ENGINE_GAME_MODE_HPP
#define GAME_ENGINE_GAME_MODE_HPP

#include "window_handler.hpp"

enum game_mode_t {
    GM_GAME,
    GM_CUSTOMIZATION,
};

game_mode_t game_mode = GM_GAME;

// симулирует игру или кастомизацию игрока
void simulate_game_mode(
    efloat delta_time,
    Player &player,
    Player &customization_player,
    WindowHandler &window_handler
) {
    window_handler.update();
    customization_player.input = player.input = window_handler.input;

    auto &input = player.input;

    if (PRESSED(BUTTON_C)) {
        if (game_mode == GM_GAME) {
            game_mode = GM_CUSTOMIZATION;
            customization_player = player;
            customization_player.now_is_customization = true;
            global_variables::camera.pos = player.pos;
        } else if (game_mode == GM_CUSTOMIZATION) {
            game_mode = GM_GAME;
            player.cloack_color_id = customization_player.cloack_color_id;
            player.t_shirt_color_id = customization_player.t_shirt_color_id;
            global_variables::camera.pos = player.pos;
        } else {
            ASSERT(false, "game_mode = ?");
        }
    }

    if (game_mode == GM_GAME) {
        // мы должны симулировать свою версию игры,
        // то есть мы пару кадров будем жить в своем собственном мире,
        // а когда получим от сервера его версию игры, то переключимся на нее
        // таким образом игра становиться более плавной и менее зависящей от сервера
        // если долго не будут приходить пакеты от сервера, то клиент еще сможет играть

        player.cursor_dir = window_handler.cursor.pos + global_variables::camera.pos - player.pos;
        global_variables::camera.simulate(player.pos, delta_time);

        // симулируем игроков
        for (auto &player : game_variables::Players) {
            simulate_player(delta_time, player.client_id);
        }

        // симулируем игру
        simulate_game(delta_time);
    } else if (game_mode == GM_CUSTOMIZATION) {
        simulate_player(delta_time, customization_player);
        global_variables::camera.pos = customization_player.pos;  // static camera

        // false = t-shirt
        // true = cloack
        static bool change_mode = false;
        if (PRESSED(BUTTON_T)) {
            change_mode = !change_mode;
        }

        if (PRESSED(BUTTON_Q)) {
            if (change_mode) {
                if (customization_player.cloack_color_id == 0) {
                    customization_player.cloack_color_id = Player::customization_colors.size() / 2 - 1;
                } else {
                    customization_player.cloack_color_id--;
                }
            } else {
                if (customization_player.t_shirt_color_id == 0) {
                    customization_player.t_shirt_color_id = Player::customization_colors.size() / 2 - 1;
                } else {
                    customization_player.t_shirt_color_id--;
                }
            }
        }

        if (PRESSED(BUTTON_E)) {
            if (change_mode) {
                customization_player.cloack_color_id++;
                if (customization_player.cloack_color_id == Player::customization_colors.size() / 2) {
                    customization_player.cloack_color_id = 0;
                }
            } else {
                customization_player.t_shirt_color_id++;
                if (customization_player.t_shirt_color_id == Player::customization_colors.size() / 2) {
                    customization_player.t_shirt_color_id = 0;
                }
            }
        }
    } else {
        ASSERT(false, "game_mode = ?");
    }

    /*if (!this_frame_from_server) {
        // этот фрейм уже не от сервера
        // мы должны симулировать свою версию игры
        //
        // то есть мы пару кадров будем жить в своем собственном мире,
        // а когда получим от сервера его версию игры, то переключимся на нее
        // таким образом игра становиться более плавной и менее зависящей от сервера
        // если долго не будут приходить пакеты от сервера, то клиент еще сможет играть

        // симулируем игроков
        for (auto &player : Players) {
            simulate_player(delta_time, player.client_id);
        }

        // симулируем игру
        simulate_game(delta_time);
    }*/
}

// рисует игру или кастомизацию игрока
void draw_game_mode(efloat delta_time, Player &player, Player &customization_player, WindowHandler &window_handler) {
    if (game_mode == GM_GAME) {
        window_handler.draw_frame(delta_time, 0);
    } else if (game_mode == GM_CUSTOMIZATION) {
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
                draw_rect(start_pos + Dot(color_id * 8, 0), Dot(2, 2), Player::customization_colors[2 * color_id]);
            }
        }

        draw_text_align("PLAYER CUSTOMIZATION", Dot(0, 35), 1, GREEN);

        draw_text_align("press T to change the choice of customization (t-shirt or cloack)", Dot(0, -30), 0.5, GREEN);
        draw_text_align("press Q or E to change the choice of color", Dot(0, -40), 0.5, GREEN);
        draw_text_align("use W,A,S,D to walk and SPACE to jump", Dot(0, -50), 0.5, GREEN);

    } else {
        ASSERT(false, "game_mode = ?");
    }

    window_handler.release_frame();
}

#endif  // GAME_ENGINE_GAME_MODE_HPP