#include "Game objects/game_objects.hpp"

void simulate_player(const Input &input, efloat delta_time, int client_id) {
    ASSERT(client_id >= 0, "is incorrect client_id");

    int index = find_player_index(client_id);
    if (index == -1) {
        return; // TODO: или ассерт
    }

    // накопление вектора движения
    auto accum_ddp = [&input](button_t left, button_t right, button_t top, button_t bottom) -> Dot {
        return Dot(IS_DOWN(right) - IS_DOWN(left), IS_DOWN(top) - IS_DOWN(bottom));
    };

    Players[index].simulate(delta_time, accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S), PRESSED(BUTTON_SPACE));

    if (PRESSED(BUTTON_MOUSE_L) && !Players[index].is_paralyzed &&
        !Players[index].is_jumped && Players[index].coins > 0) {
        Players[index].weapon.shot(Players[index].pos);
        Players[index].coins--;
    }
}

Room test_room;

void simulate_game(efloat delta_time) {
    test_room.simulate(delta_time);
}

void draw_game() {
    test_room.draw();
    cursor.draw();
}
