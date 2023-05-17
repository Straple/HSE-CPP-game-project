#include "../Game objects/game_objects.hpp"

void cast_to_top_left_and_bottom_right(Dot &p0, Dot &p1) {
    if (p0.x > p1.x) {
        std::swap(p0.x, p1.x);
    }
    if (p0.y < p1.y) {
        std::swap(p0.y, p1.y);
    }
}

Room current_room;

sprite_t current_type_of_sprite = static_cast<sprite_t>(0);
efloat current_size = 1;
Dot current_pos;
int current_level = 0;

enum level_maker_mode {
    LM_SPRITE,
    LM_COLLISION_BOX,
    LM_RENDER_LEVEL,
    LM_OBJECT,
    LM_INTERESTING_DOT
};

/*enum game_objects{
    GO_BUSH,
    GO_TREE,

    GO_COUNT,
};*/

level_maker_mode current_mode;

Dot downed_pos;

void render_game(const Input &input) {
    clear_screen(GREY);

    // draw level
    {
        int need_highlight = -1;

        if (current_mode == LM_SPRITE || current_mode == LM_RENDER_LEVEL) {
            for (int i = static_cast<int>(current_room.Draw_objects.size()) - 1;
                 i >= 0; i--) {
                auto [pos, size, sprite, level] = current_room.Draw_objects[i];
                if (collision_in_draw_sprite(pos, size, sprite, cursor.pos)) {
                    need_highlight = i;
                    break;
                }
            }
        }

        for (int i = 0; i < current_room.Draw_objects.size(); i++) {
            auto [pos, size, sprite, level] = current_room.Draw_objects[i];
            if (need_highlight == i) {
                draw_sprite(pos, size, sprite, [&](const Color &color) {
                    return color.combine(Color(0xffffff, 60));
                });
                if (current_mode == LM_RENDER_LEVEL) {
                    draw_object(
                        level, pos - global_variables::camera.pos, 0.5, RED
                    );
                }
            } else {
                draw_sprite(pos, size, sprite);
                if (current_mode == LM_RENDER_LEVEL) {
                    draw_object(
                        level, pos - global_variables::camera.pos, 0.5,
                        Color(0xff0000, 128)
                    );
                }
            }
        }

        if (current_mode == LM_INTERESTING_DOT) {
            for (auto [pos, name] : current_room.Interesting_dots) {
                if ((current_pos - pos).get_len() < 1) {
                    draw_rect(
                        pos - global_variables::camera.pos, Dot(0.5, 0.5),
                        YELLOW
                    );
                    draw_text(
                        name.c_str(), pos - global_variables::camera.pos, 0.5,
                        YELLOW
                    );
                } else {
                    draw_rect(
                        pos - global_variables::camera.pos, Dot(0.5, 0.5),
                        Color(0xffff00, 128)
                    );
                    draw_text(
                        name.c_str(), pos - global_variables::camera.pos, 0.5,
                        Color(0xffff00, 128)
                    );
                }
            }
        }
    }

    {
        if (current_mode == LM_SPRITE) {
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
                    return Color(static_cast<unsigned int>(color), 180);
                }
            );
        } else if (current_mode == LM_COLLISION_BOX) {
            for (auto [p0, p1] : current_room.Collision_boxes) {
                draw_rect2(
                    p0 - global_variables::camera.pos,
                    p1 - global_variables::camera.pos, Color(0x00fff0, 30)
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

    draw_rect(
        cursor.pos, Dot(1, 1) / global_variables::render_scale / 300, RED
    );
    // draw_rect(cursor.pos, Dot(1, 1) * 0.1, RED);
    if (current_mode == LM_RENDER_LEVEL) {
        draw_object(current_level, cursor.pos + Dot(3, 0), 0.7, RED);
    }

    if (current_mode == LM_SPRITE) {
        draw_object(
            sprite_type_to_string(current_type_of_sprite),
            Dot(20, -global_variables::arena_half_size.y + 5), 0.5, GREEN
        );
    }
}

void simulate_input(const Input &input) {
    // change mode
    if (pressed(BUTTON_C)) {
        if (current_mode == LM_SPRITE) {
            current_mode = LM_RENDER_LEVEL;
        } else if (current_mode == LM_RENDER_LEVEL) {
            current_mode = LM_COLLISION_BOX;
        } else if (current_mode == LM_COLLISION_BOX) {
            current_mode = LM_OBJECT;
        } else if (current_mode == LM_OBJECT) {
            current_mode = LM_INTERESTING_DOT;
        } else if (current_mode == LM_INTERESTING_DOT) {
            current_mode = LM_SPRITE;
        } else {
            ASSERT(false, "what is mode?");
        }
    }

    // change choose object
    if (pressed(BUTTON_Q)) {
        if (current_mode == LM_SPRITE) {
            int sprite_id = static_cast<int>(current_type_of_sprite);
            sprite_id--;
            if (sprite_id < 0) {
                sprite_id += SP_COUNT;
            }
            current_type_of_sprite = static_cast<sprite_t>(sprite_id);
        } else if (current_mode == LM_RENDER_LEVEL) {
            current_level--;
        }
    } else if (pressed(BUTTON_E)) {
        if (current_mode == LM_SPRITE) {
            int sprite_id = static_cast<int>(current_type_of_sprite);
            sprite_id++;
            if (sprite_id == SP_COUNT) {
                sprite_id = 0;
            }
            current_type_of_sprite = static_cast<sprite_t>(sprite_id);
        } else if (current_mode == LM_RENDER_LEVEL) {
            current_level++;
        }
    }

    if (is_down(BUTTON_SHIFT)) {
        Dot better = Dot(1e9, 1e9);
        //  найти близжайший рисуемый прямоугольник
        for (auto [pos, size, sprite, level] : current_room.Draw_objects) {
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

        cursor.pos -= current_pos - better;
        current_pos = better;
    }

    if (pressed(BUTTON_MOUSE_L)) {
        if (current_mode == LM_SPRITE) {
            Dot pos = current_pos;
            if (is_down(BUTTON_R)) {
                pos.x -= Sprites[current_type_of_sprite].width() * current_size;
            }

            if (is_down(BUTTON_T)) {
                pos.y +=
                    Sprites[current_type_of_sprite].height() * current_size;
            }

            current_room.Draw_objects.push_back(
                {pos, current_size, current_type_of_sprite, current_level}
            );
        } else if (current_mode == LM_RENDER_LEVEL) {
            int need_highlight = -1;

            for (int i = static_cast<int>(current_room.Draw_objects.size()) - 1;
                 i >= 0; i--) {
                auto [pos, size, sprite, level] = current_room.Draw_objects[i];
                if (collision_in_draw_sprite(pos, size, sprite, cursor.pos)) {
                    need_highlight = i;
                    break;
                }
            }

            if (need_highlight != -1) {
                current_room.Draw_objects[need_highlight].level = current_level;
            }
        } else if (current_mode == LM_INTERESTING_DOT) {
            current_room.Interesting_dots.push_back({current_pos, "none"});
        }
    }

    // remove
    if (pressed(BUTTON_MOUSE_R)) {
        if (current_mode == LM_SPRITE) {
            for (int i = static_cast<int>(current_room.Draw_objects.size()) - 1;
                 i >= 0; i--) {
                auto [pos, size, sprite, level] = current_room.Draw_objects[i];
                if (collision_in_draw_sprite(pos, size, sprite, cursor.pos)) {
                    current_room.Draw_objects.erase(
                        current_room.Draw_objects.begin() + i
                    );
                    break;
                }
            }
        } else if (current_mode == LM_COLLISION_BOX) {
            for (int i =
                     static_cast<int>(current_room.Collision_boxes.size()) - 1;
                 i >= 0; i--) {
                if (current_room.Collision_boxes[i].trigger(current_pos)) {
                    current_room.Collision_boxes.erase(
                        current_room.Collision_boxes.begin() + i
                    );
                    break;
                }
            }
        } else if (current_mode == LM_INTERESTING_DOT) {
            for (int i = 0;
                 i < static_cast<int>(current_room.Interesting_dots.size());
                 i++) {
                if ((current_pos - current_room.Interesting_dots[i].pos)
                        .get_len() < 1) {
                    current_room.Interesting_dots.erase(
                        current_room.Interesting_dots.begin() + i
                    );
                    break;
                }
            }
        }
    }

    if (pressed(BUTTON_MOUSE_L)) {
        downed_pos = current_pos;
    }

    // set collision_box
    if (current_mode == LM_COLLISION_BOX && released(BUTTON_MOUSE_L)) {
        Dot p0 = downed_pos;
        Dot p1 = current_pos;

        cast_to_top_left_and_bottom_right(p0, p1);

        current_room.Collision_boxes.emplace_back(p0, p1);
    }

    // save level
    if (pressed(BUTTON_V)) {
        std::sort(
            current_room.Draw_objects.begin(), current_room.Draw_objects.end()
        );
        current_room.write("level.txt");
    }
}

void simulate_game(const Input &input, efloat delta_time) {
    current_pos = global_variables::camera.pos + cursor.pos;

    simulate_input(input);
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

    current_size = global_variables::mouse_wheel / 10'000 + 1;

    render_game(input);

    draw_object(
        current_size, global_variables::arena_half_size + Dot(-20, -20), 0.5,
        BLUE
    );

    {
        std::string mode_str;
        if (current_mode == LM_SPRITE) {
            mode_str = "sprite";
        } else if (current_mode == LM_RENDER_LEVEL) {
            mode_str = "render level";
        } else if (current_mode == LM_COLLISION_BOX) {
            mode_str = "collision box";
        } else if (current_mode == LM_OBJECT) {
            mode_str = "object";
        } else if (current_mode == LM_INTERESTING_DOT) {
            mode_str = "interesting dot";
        }
        draw_object(
            mode_str, Dot(0, -global_variables::arena_half_size.y + 5), 0.5, RED
        );
    }
}