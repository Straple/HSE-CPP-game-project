#include "Game objects/game_objects.hpp"

void simulate_player(const Input &input, efloat delta_time) {
    // накопление вектора движения
    auto accum_ddp = [&input](
                         button_t left, button_t right, button_t top,
                         button_t bottom
                     ) -> Dot {
        return Dot(
            is_down(right) - is_down(left), is_down(top) - is_down(bottom)
        );
    };

    Players[0].simulate(
        delta_time, accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S),
        pressed(BUTTON_SPACE)
    );

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

template <typename func_t>
void simulate_input(const Input &input, func_t &&window_mode_callback) {
    if (pressed(BUTTON_ESC)) {
        global_variables::running = false;
        return;
    }

    if (pressed(BUTTON_ENTER)) {
        window_mode_callback();
    }

    if (pressed(BUTTON_TAB)) {
        global_variables::debug_mode = !global_variables::debug_mode;
    }

    if (pressed(BUTTON_K)) {
        global_variables::show_locator = !global_variables::show_locator;
    }

    if (pressed(BUTTON_F)) {
        global_variables::show_fps = !global_variables::show_fps;
    }

    // update render_scale
    {
        if (is_down(BUTTON_UP)) {
            increase_window_scaling(cursor.pos);
        }

        if (is_down(BUTTON_DOWN)) {
            decrease_window_scaling(cursor.pos);
        }
    }

    cursor.simulate(input);
}

Room test_room;

template <typename func_t>
void simulate_game(
    const Input &input,
    efloat delta_time,
    func_t &&window_mode_callback
) {
    simulate_input(input, window_mode_callback);

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

    //draw_object(global_variables::render_scale, Dot(), 1, BLACK);

    cursor.draw();
}
