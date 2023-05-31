#include "Game objects/game_objects.hpp"

enum game_mode_t {
    GM_GAME,
    GM_CUSTOMIZATION,
};

game_mode_t game_mode = GM_GAME;

void simulate_player(efloat delta_time, int client_id) {
    ASSERT(client_id >= 0, "is incorrect client_id");

    int index = find_player_index(client_id);
    ASSERT(index != -1, "where this player?");

    auto &player = Players[index];

    auto &input = player.input;

    // накопление вектора движения
    auto accum_ddp = [&input](button_t left, button_t right, button_t top, button_t bottom) -> Dot {
        return Dot(IS_DOWN(right) - IS_DOWN(left), IS_DOWN(top) - IS_DOWN(bottom));
    };

    player.simulate(delta_time, accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S), PRESSED(BUTTON_SPACE));

    // if (PRESSED(BUTTON_MOUSE_L)) {
    //     add_flower_dome_effect(Players[index].cursor_dir + Players[index].pos);
    // }

    if (PRESSED(BUTTON_MOUSE_L) && !player.is_paralyzed && !player.is_jumped && player.coins > 0 &&
        player.weapon.may_shot() && !player.is_dead()) {
        player.weapon.shot(player.pos, player.cursor_dir + player.pos, BulletHostType::PLAYER, SP_COIN);
        player.coins--;
    }

    // customize colors
    if (game_mode == GM_CUSTOMIZATION) {
        // false = t-shirt
        // true = cloack
        static bool change_mode = false;
        if (PRESSED(BUTTON_UP) || PRESSED(BUTTON_DOWN)) {
            change_mode = !change_mode;
        }

        if (PRESSED(BUTTON_LEFT)) {
            if (change_mode) {
                if (player.cloack_color_id == 0) {
                    player.cloack_color_id = Player::customization_colors.size() / 2 - 1;
                } else {
                    player.cloack_color_id--;
                }
            } else {
                if (player.t_shirt_color_id == 0) {
                    player.t_shirt_color_id = Player::customization_colors.size() / 2 - 1;
                } else {
                    player.t_shirt_color_id--;
                }
            }
        }

        if (PRESSED(BUTTON_RIGHT)) {
            if (change_mode) {
                player.cloack_color_id++;
                if (player.cloack_color_id == Player::customization_colors.size() / 2) {
                    player.cloack_color_id = 0;
                }
            } else {
                player.t_shirt_color_id++;
                if (player.t_shirt_color_id == Player::customization_colors.size() / 2) {
                    player.t_shirt_color_id = 0;
                }
            }
        }
    }

    // текущий кадр инпута так и оставим, а вот предыдущий смениться текущим
    // таким образом, мы будем считать, что игрок все еще нажимает на кнопку до тех пор,
    // пока не получим информацию от него об обратном
    input.previous = input.current;
}

Room test_room;

void simulate_game(efloat delta_time) {
    test_room.simulate(delta_time);
}

void draw_game(int client_id) {
    switch (game_mode) {
        case GM_GAME: {
            test_room.draw();

            // draw UI
            {
                int index = find_player_index(client_id);
                auto &player = Players[index];
                // draw hp
                for (int i = 0; i < player.hp; i++) {
                    draw_sprite(
                        Dot(-global_variables::arena_half_size.x, global_variables::arena_half_size.y) * 0.95 +
                            global_variables::camera.pos + Dot(i, 0) * 7,
                        0.5, SP_HEART
                    );
                }

                // draw coins
                for (int i = 0; i < player.coins; i++) {
                    draw_sprite(
                        Dot(-global_variables::arena_half_size.x, global_variables::arena_half_size.y) * 0.8 +
                            global_variables::camera.pos + Dot(-14 - i * 2 + Players[0].coins * 2, 0),
                        0.5, SP_COIN
                    );
                }
            }
        } break;
        case GM_CUSTOMIZATION: {
            clear_screen(GREY);
            int index = find_player_index(client_id);
            Players[index].draw();
        } break;
    }
}
