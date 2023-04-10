static const Dot world_half_size(500, 500);

// don't shuffle
#include "Game objects\game_objects.hpp"
// don't shuffle
#include "game_collision.cpp"

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

void simulate_physics(efloat delta_time) {
    // add new world objects
    {
        std::uniform_int_distribution<int> random_x(
            -world_half_size.x, world_half_size.x
        );
        std::uniform_int_distribution<int> random_y(
            -world_half_size.y, world_half_size.y
        );
        std::uniform_int_distribution<int> range(1, 3);

        auto random_dot = [&]() -> Dot {
            return Dot(random_x(rnd), random_y(rnd));
        };
    }

    // simulate logs
    {
        for (auto &log : Logs) {
            log.simulate(delta_time);
        }
        simulate_game_collision(Logs);
    }

    // simulate slimes
    {
        for (auto &slime : Slimes) {
            slime.simulate(delta_time);
        }

        simulate_game_collision(Slimes);
    }

    // simulate bats
    {
        for (auto &bat : Bats) {
            bat.simulate(delta_time);
        }

        simulate_game_collision(Bats);
    }

    // simulate fireplaces
    {
        //        for (auto &fireplace : Fireplaces) {
        //            fireplace.simulate(delta_time);
        //        }
    }

    // simulate effects
    {
        for (unsigned int i = 0; i < Effects.size(); i++) {
            if (Effects[i].simulate(delta_time)) {
                Effects.erase(Effects.begin() + i);
                i--;
            }
        }
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

    simulate_physics(delta_time);

    cursor.simulate(input);

    test_room.draw();

//        draw_sprite(player.pos, 0.1, SP_TREE);
//        draw_sprite(mouse.pos + camera.pos, 0.1, SP_TREE);
        static efloat last_hit_time = 3;
        if (pressed(BUTTON_MOUSE_L) && last_hit_time >= PLAYER_ATTACK_COOLDOWN ) {
            // new bullet!
            if (-1 > player.random_shot+player.random_shot_multiplyer || player.random_shot+player.random_shot_multiplyer > 1) {
                player.random_shot_multiplyer*=-1;
            }
            player.random_shot += player.random_shot_multiplyer;
//            std::cout << player.ind << ' ' << player.bullet_delta << ' ';
//            std::cout << player.ind << ' ' << player.bullet_delta << std::endl;
            last_hit_time = 0;
            Bullets.emplace_back(
                // player.pos,
                player.get_collision()
                    .circle.pos + BulletDeltas[(player.ind+player.random_shot+24)%24],  // центрированная позиция относительно спрайта
                cursor.pos + global_variables::camera.pos, 1000000000, 1000
            );
        }
        last_hit_time += delta_time;
    test_room.simulate(delta_time, input);

    for (auto &loot : Loot_objects) {
        loot.draw();
    }

    for (int i = 0; i < Loot_objects.size(); i++) {
        auto &object = Loot_objects[i];
        object.simulate(delta_time);
        if (object.simulate_collection()) {
            Loot_objects.erase(Loot_objects.begin() + i);
            i--;
        }
    }

    cursor.draw();

    draw_object(Players[0].dp, Dot(), 1, BLACK);
}
