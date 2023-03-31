
void cast_to_top_left_and_bottom_right(Dot &p0, Dot &p1) {
    if (p0.x > p1.x) {
        std::swap(p0.x, p1.x);
    }
    if (p0.y < p1.y) {
        std::swap(p0.y, p1.y);
    }
}

struct drawing_objects {
    Dot pos;
    efloat size;
    sprite_t sprite;
};

std::vector<drawing_objects> Draw_objects;

std::vector<collision_box> Collision_boxes;

void write_level() {
    std::ofstream file("level.txt");
    file << std::fixed << std::setprecision(10);

    file << "SPRITES\n";
    file << Draw_objects.size() << '\n';
    for (auto [pos, size, sprite] : Draw_objects) {
        file << sprite_type_to_string(sprite) << ' ' << pos << ' ' << size
             << '\n';
    }

    file << "COLLISION BOXES\n";
    file << Collision_boxes.size() << '\n';
    for (auto [p0, p1] : Collision_boxes) {
        file << p0 << ' ' << p1 << '\n';
    }
}

void read_level() {
    std::ifstream file("level.txt");

    {
        std::string str;
        file >> str;  // SPRITES

        int count;
        file >> count;
        Draw_objects.assign(count, {});
        for (auto &[pos, size, sprite] : Draw_objects) {
            std::string sprite_name;
            file >> sprite_name >> pos >> size;
            sprite = string_to_sprite_type(sprite_name);
        }
    }

    {
        std::string str;
        file >> str >> str;  // COLLISION BOXES

        int count;
        file >> count;
        Collision_boxes.assign(count, {});
        for (auto &[p0, p1] : Collision_boxes) {
            file >> p0 >> p1;
        }
    }
}

sprite_t current_type_of_sprite = static_cast<sprite_t>(0);
efloat current_size = 1;
Dot current_pos;

// "sprite" or "collision_box"
std::string current_mode = "sprite";

Dot downed_pos;

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
        if (current_mode == "sprite") {
            Dot pos = current_pos;
            if (is_down(BUTTON_R)) {
                pos.x -= Sprites[current_type_of_sprite].width() * current_size;
            }

            if (is_down(BUTTON_T)) {
                pos.y +=
                    Sprites[current_type_of_sprite].height() * current_size;
            }

            draw_sprite(
                pos, current_size, current_type_of_sprite,
                [&](const Color &color) {
                    return Color(color.operator unsigned int(), 180);
                }
            );
        } else {
            for (auto [p0, p1] : Collision_boxes) {
                draw_rect2(
                    p0 - global_variables::camera.pos, p1 - global_variables::camera.pos, Color(0x00fff0, 30)
                );
            }

            if (is_down(BUTTON_MOUSE_L)) {
                Dot p0 = downed_pos - global_variables::camera.pos;
                Dot p1 = current_pos - global_variables::camera.pos;

                cast_to_top_left_and_bottom_right(p0, p1);
                draw_rect2(p0, p1, Color(0xff0000, 30));
            }
        }
    }

    draw_rect(mouse.pos, Dot(1, 1) / global_variables::render_scale / 300, RED);
    // draw_rect(mouse.pos, Dot(1, 1) * 0.1, RED);
}

template <typename func_t>
void simulate_input(const Input &input, func_t &&window_mode_callback) {
    // standart buttons
    {
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

        /*if (pressed(BUTTON_K)) {
            global_variables::show_locator = !global_variables::show_locator;
        }*/

        if (pressed(BUTTON_F)) {
            global_variables::show_fps = !global_variables::show_fps;
        }

        if (is_down(BUTTON_UP)) {
            increase_window_scaling(mouse.pos);
        }

        if (is_down(BUTTON_DOWN)) {
            decrease_window_scaling(mouse.pos);
        }
    }

    // change mode
    if (pressed(BUTTON_C)) {
        if (current_mode == "sprite") {
            current_mode = "collision_box";
        } else if (current_mode == "collision_box") {
            current_mode = "sprite";
        } else {
            ASSERT(false, "what is mode?");
        }
    }

    // change choose sprite
    if (current_mode == "sprite") {
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
                    if (global_variables::debug_mode || is_draw(pixels[i][j])) {
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
    if (current_mode == "sprite" && pressed(BUTTON_MOUSE_L)) {
        Dot pos = current_pos;
        if (is_down(BUTTON_R)) {
            pos.x -= Sprites[current_type_of_sprite].width() * current_size;
        }

        if (is_down(BUTTON_T)) {
            pos.y += Sprites[current_type_of_sprite].height() * current_size;
        }

        Draw_objects.push_back({pos, current_size, current_type_of_sprite});
    }

    // pop object
    if (pressed(BUTTON_Z)) {
        if (current_mode == "sprite") {
            if (!Draw_objects.empty()) {
                Draw_objects.pop_back();
            }
        } else {
            if (!Collision_boxes.empty()) {
                Collision_boxes.pop_back();
            }
        }
    }

    // remove
    if (pressed(BUTTON_MOUSE_R)) {
        if (current_mode == "sprite") {
            for (int i = static_cast<int>(Draw_objects.size()) - 1; i >= 0;
                 i--) {
                auto [pos, size, sprite] = Draw_objects[i];
                if (collision_in_draw_sprite(pos, size, sprite, mouse.pos)) {
                    Draw_objects.erase(Draw_objects.begin() + i);
                    break;
                }
            }
        } else {
            for (int i = static_cast<int>(Collision_boxes.size()) - 1; i >= 0;
                 i--) {
                if (Collision_boxes[i].trigger(current_pos)) {
                    Collision_boxes.erase(Collision_boxes.begin() + i);
                    break;
                }
            }
        }
    }

    if (pressed(BUTTON_MOUSE_L)) {
        downed_pos = current_pos;
    }

    if (current_mode == "collision_box" && released(BUTTON_MOUSE_L)) {
        Dot p0 = downed_pos;
        Dot p1 = current_pos;

        cast_to_top_left_and_bottom_right(p0, p1);

        Collision_boxes.emplace_back(p0, p1);
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
    current_pos = global_variables::camera.pos + mouse.pos;

    simulate_input(input, window_mode_callback);
    if (!global_variables::running) {
        return;
    }

    // simulate camera move
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
            global_variables::camera.pos, dp,
            accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S) * 500, delta_time
        );
    }

    current_size = mouse_wheel / 10'000 + 1;

    render_game(input);

    draw_object(current_size, global_variables::arena_half_size + Dot(-20, -20), 0.5, BLUE);

    draw_object(current_mode, Dot(0, -global_variables::arena_half_size.y + 5), 0.5, RED);
}