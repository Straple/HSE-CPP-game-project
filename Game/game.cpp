#include "Game objects/game_objects.hpp"

void simulate_player(const Input &input, efloat delta_time) {
    // накопление вектора движения
    auto accum_ddp = [&input](button_t left, button_t right, button_t top, button_t bottom) -> Dot {
        return Dot(
            is_down(right) - is_down(left), is_down(top) - is_down(bottom)
        );
    };

    Players[0].simulate(delta_time, accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S), pressed(BUTTON_SPACE));

    // player attack
    {
        /*if (player.simulate_attack(Logs) |  Players[0].simulate_attack(
                Slimes
            ) |
            Players[0].simulate_attack(Bats) |
            Players[0].simulate_attack(Trees)) {
            Players[0].now_is_attached = false;
        }*/
    }
}

Room test_room;

void simulate_game(const Input &input, efloat delta_time) {
    if (!global_variables::running) {
        return;
    }

    global_variables::camera.simulate(Players[0].pos, delta_time);

    // simulate players
    {
        simulate_player(input, delta_time);

        // simulate_game_collision(player);
    }

    cursor.simulate(input);

    test_room.simulate(delta_time, input);

    test_room.draw();

    // draw_object(global_variables::render_scale, Dot(), 1, BLACK);

    cursor.draw();
}
