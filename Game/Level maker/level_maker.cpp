struct drawing_objects {
    Dot pos;
    efloat size;
    sprite_t sprite;
};

std::vector<drawing_objects> Draw_objects;

void write_level() {
    std::ofstream file("level.txt");
    file << Draw_objects.size() << '\n';
    file << std::fixed << std::setprecision(10);
    for (auto [pos, size, sprite] : Draw_objects) {
        file << "sprite ";
        file << sprite << ' ' << pos << ' ' << size << '\n';
    }
}

void read_level() {
    std::ifstream file("level.txt");
    int count;
    file >> count;
    Draw_objects.assign(count, {});
    for (auto &[pos, size, sprite] : Draw_objects) {
        std::string sprite_str;
        file >> sprite_str;
        int sprite_id;
        file >> sprite_id >> pos >> size;
        sprite = static_cast<sprite_t>(sprite_id);
    }
}

sprite_t current_type_of_sprite = static_cast<sprite_t>(0);
efloat current_size = 1;
Dot current_pos;

void render_game(const Input &input) {
    clear_screen(GREY);

    // draw level
    {
        int need_highlight = -1;

        for (int i = static_cast<int>(Draw_objects.size()) - 1; i >= 0; i--) {
            auto [pos, size, sprite] = Draw_objects[i];
            if (collision_in_draw_sprite(pos, size, sprite, mouse.pos)) {
                need_highlight = i;
                break;
            }
        }

        for (int i = 0; i < Draw_objects.size(); i++) {
            auto [pos, size, sprite] = Draw_objects[i];
            if (need_highlight == i) {
                draw_sprite(pos, size, sprite, [&](const Color &color) {
                    return color.combine(Color(0xffffff, 60));
                });
            } else {
                draw_sprite(pos, size, sprite);
            }
        }
    }

    {
        Dot pos = current_pos;
        if (is_down(BUTTON_R)) {
            pos.x -= Sprites[current_type_of_sprite].width() * current_size;
        }

        if (is_down(BUTTON_T)) {
            pos.y += Sprites[current_type_of_sprite].height() * current_size;
        }

        draw_sprite(
            pos, current_size, current_type_of_sprite,
            [&](const Color &color) {
                return Color(color.operator unsigned int(), 180);
            }
        );
    }

    draw_rect(mouse.pos, Dot(1, 1) / render_scale / 300, RED);
    // draw_rect(mouse.pos, Dot(1, 1) * 0.1, RED);
}

template <typename func_t>
void simulate_input(const Input &input, func_t &&window_mode_callback) {
    // standart buttons
    {
        if (pressed(BUTTON_ESC)) {
            running = false;
            return;
        }

        if (pressed(BUTTON_ENTER)) {
            window_mode_callback();
        }

        if (pressed(BUTTON_TAB)) {
            debug_mode = !debug_mode;
        }

        /*if (pressed(BUTTON_K)) {
            show_locator = !show_locator;
        }*/

        if (pressed(BUTTON_F)) {
            show_fps = !show_fps;
        }

        if (is_down(BUTTON_UP)) {
            increase_window_scaling(mouse.pos);
        }

        if (is_down(BUTTON_DOWN)) {
            decrease_window_scaling(mouse.pos);
        }
    }

    // change choose sprite
    {
        if (pressed(BUTTON_Q)) {
            int id = static_cast<int>(current_type_of_sprite);
            id--;
            if (id < 0) {
                id += SP_COUNT;
            }
            current_type_of_sprite = static_cast<sprite_t>(id);
        }

        if (pressed(BUTTON_E)) {
            int id = static_cast<int>(current_type_of_sprite);
            id++;
            if (id == SP_COUNT) {
                id -= SP_COUNT;
            }
            current_type_of_sprite = static_cast<sprite_t>(id);
        }
    }

    if (is_down(BUTTON_SHIFT)) {
        Dot better = Dot(1e9, 1e9);
        //  найти близжайший рисуемый прямоугольник
        for (auto [pos, size, sprite] : Draw_objects) {
            const auto &pixels = Sprites[sprite];

            for (int i = 0; i < pixels.height(); i++) {
                for (int j = 0; j < pixels.width(); j++) {
                    if (debug_mode || is_draw(pixels[i][j])) {
                        for (int x = 0; x <= 1; x++) {
                            for (int y = 0; y <= 1; y++) {
                                Dot draw_pos = pos + Dot(j + x, -i - y) * size;
                                if ((draw_pos - current_pos).get_len() <
                                    (better - current_pos).get_len()) {
                                    better = draw_pos;
                                }
                            }
                        }
                    }
                }
            }
        }

        mouse.pos -= current_pos - better;
        current_pos = better;
    }

    // set sprite
    if (pressed(BUTTON_MOUSE_L)) {
        Dot pos = current_pos;
        if (is_down(BUTTON_R)) {
            pos.x -= Sprites[current_type_of_sprite].width() * current_size;
        }

        if (is_down(BUTTON_T)) {
            pos.y += Sprites[current_type_of_sprite].height() * current_size;
        }

        Draw_objects.push_back({pos, current_size, current_type_of_sprite});
    }

    // pop back sprite
    if (pressed(BUTTON_Z)) {
        if (!Draw_objects.empty()) {
            Draw_objects.pop_back();
        }
    }

    // remove sprite
    if (pressed(BUTTON_MOUSE_R)) {
        for (int i = static_cast<int>(Draw_objects.size()) - 1; i >= 0; i--) {
            auto [pos, size, sprite] = Draw_objects[i];
            if (collision_in_draw_sprite(pos, size, sprite, mouse.pos)) {
                Draw_objects.erase(Draw_objects.begin() + i);
                break;
            }
        }
    }

    if (pressed(BUTTON_V)) {
        write_level();
    }
}

template <typename func_t>
void simulate_game(
    const Input &input,
    efloat delta_time,
    func_t &&window_mode_callback
) {
    current_pos = camera.pos + mouse.pos;

    simulate_input(input, window_mode_callback);
    if (!running) {
        return;
    }

    // simulate physics
    {
        // накопление вектора движения
        auto accum_ddp = [&input](
                             button_t left, button_t right, button_t top,
                             button_t bottom
                         ) -> Dot {
            return Dot(
                is_down(right) - is_down(left), is_down(top) - is_down(bottom)
            );
        };

        static Dot dp;
        simulate_move2d(
            camera.pos, dp,
            accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S) * 500, delta_time
        );
    }

    current_size = mouse_wheel / 10'000 + 1;

    render_game(input);

    draw_object(current_size, arena_half_size + Dot(-20, -20), 0.5, BLUE);
}