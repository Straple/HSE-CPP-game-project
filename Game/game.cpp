#include "Game objects/game_objects.hpp"

void simulate_player(efloat delta_time, Player &player) {
    auto &input = player.input;

    // накопление вектора движения
    auto accum_ddp = [&input](button_t left, button_t right, button_t top, button_t bottom) -> Dot {
        return Dot(IS_DOWN(right) - IS_DOWN(left), IS_DOWN(top) - IS_DOWN(bottom));
    };

    player.simulate(delta_time, (!global_variables::is_typing)?accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S):Dot(0,0), (!global_variables::is_typing)?PRESSED(BUTTON_SPACE):false);

    // if (PRESSED(BUTTON_MOUSE_L)) {
    //     add_flower_dome_effect(Players[index].cursor_dir + Players[index].pos);
    // }

    if (PRESSED(BUTTON_MOUSE_L) && !player.is_paralyzed && !player.is_jumped && player.coins > 0 &&
        player.weapon.may_shot() && !player.is_dead()) {
        player.weapon.shot(player.pos, player.cursor_dir + player.pos, BulletHostType::PLAYER, SP_COIN);
        player.coins--;
    }

    // текущий кадр инпута так и оставим, а вот предыдущий смениться текущим
    // таким образом, мы будем считать, что игрок все еще нажимает на кнопку до тех пор,
    // пока не получим информацию от него об обратном
    input.previous = input.current;
}

void simulate_player(efloat delta_time, int client_id) {
    ASSERT(client_id >= 0, "is incorrect client_id");

    int index = find_player_index(client_id);
    ASSERT(index != -1, "where this player?");

    simulate_player(delta_time, Players[index]);
}

Room test_room;

void simulate_game(efloat delta_time) {
    test_room.simulate(delta_time);
}

void draw_game(int client_id) {
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
}
