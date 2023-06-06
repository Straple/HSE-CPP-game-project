#include "Game_objects/game_objects.hpp"

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

enum level_maker_mode { LM_SPRITE, LM_COLLISION_BOX, LM_RENDER_LEVEL, LM_OBJECT, LM_INTERESTING_DOT, LM_COLOR_BOX };

level_maker_mode current_mode;

Dot downed_pos;

object_type current_object;

std::vector<std::pair<Dot, object_type>> Objects;

void render_game(const Input &input, const Dot &cursor_pos) {
    clear_screen(GREY);

    // draw level
    {
        int need_highlight = -1;

        if (current_mode == LM_SPRITE || current_mode == LM_RENDER_LEVEL) {
            for (int i = static_cast<int>(current_room.Draw_objects.size()) - 1; i >= 0; i--) {
                auto [pos, size, sprite, level] = current_room.Draw_objects[i];
                if (collision_in_draw_sprite(pos, size, sprite, cursor_pos)) {
                    need_highlight = i;
                    break;
                }
            }
        }

        for (int i = 0; i < current_room.Draw_objects.size(); i++) {
            auto [pos, size, sprite, level] = current_room.Draw_objects[i];
            if (need_highlight == i) {
                draw_sprite(pos, size, sprite, [&](const Color &color) { return color.combine(Color(0xffffff, 60)); });
                if (current_mode == LM_RENDER_LEVEL) {
                    draw_object(level, pos - global_variables::camera.pos, 0.5, RED);
                }
            } else {
                draw_sprite(pos, size, sprite);
                if (current_mode == LM_RENDER_LEVEL) {
                    draw_object(level, pos - global_variables::camera.pos, 0.5, Color(0xff0000, 128));
                }
            }
        }

        if (current_mode == LM_INTERESTING_DOT) {
            for (auto [pos, name] : current_room.Interesting_dots) {
                if ((current_pos - pos).get_len() < 1) {
                    draw_rect(pos - global_variables::camera.pos, Dot(0.5, 0.5), YELLOW);
                    draw_text(name.c_str(), pos - global_variables::camera.pos, 0.5, YELLOW);
                } else {
                    draw_rect(pos - global_variables::camera.pos, Dot(0.5, 0.5), Color(0xffff00, 128));
                    draw_text(name.c_str(), pos - global_variables::camera.pos, 0.5, Color(0xffff00, 128));
                }
            }
        }

        // TODO: а как highlight-ить эти объекты?
        /*need_highlight = -1;
        if (current_mode == LM_OBJECT) {
            for (int i = static_cast<int>(Objects.size()) - 1; i >= 0; i--) {
                if ((Objects[i].second == OT_BUSH && Bush(Objects[i].first).trigger_in_draw_sprite(current_pos)) ||
                    (Objects[i].second == OT_TREE && Tree(Objects[i].first).trigger_in_draw_sprite(current_pos)) ||
                    (Objects[i].second == OT_TABLE && Table(Objects[i].first).trigger_in_draw_sprite(current_pos))) {
                    need_highlight = i;
                    break;
                }
            }
        }*/

        for (auto [pos, object_type] : Objects) {
            if (object_type == OT_BUSH) {
                Bush(pos).draw();
            } else if (object_type == OT_TREE) {
                Tree(pos).draw();
            } else if (object_type == OT_TABLE) {
                Table(pos).draw();
            }
        }
    }

    // draw current choice object
    {
        if (current_mode == LM_SPRITE) {
            Dot pos = current_pos;
            if (IS_DOWN(BUTTON_R)) {
                pos.x -= get_sprite(current_type_of_sprite).width() * current_size;
            }

            if (IS_DOWN(BUTTON_T)) {
                pos.y += get_sprite(current_type_of_sprite).height() * current_size;
            }

            draw_sprite(pos, current_size, current_type_of_sprite, [&](const Color &color) {
                return Color(static_cast<unsigned int>(color), 180);
            });
        } else if (current_mode == LM_COLLISION_BOX) {
            for (auto [p0, p1] : current_room.Walls) {
                draw_rect2(p0 - global_variables::camera.pos, p1 - global_variables::camera.pos, Color(0x00fff0, 30));
            }

            if (IS_DOWN(BUTTON_MOUSE_L)) {
                Dot p0 = downed_pos - global_variables::camera.pos;
                Dot p1 = current_pos - global_variables::camera.pos;

                cast_to_top_left_and_bottom_right(p0, p1);
                draw_rect2(p0, p1, Color(0xff0000, 30));
            }
        } else if (current_mode == LM_OBJECT) {
            if (current_object == OT_BUSH) {
                Bush(current_pos).draw();
            } else if (current_object == OT_TREE) {
                Tree(current_pos).draw();
            } else if (current_object == OT_TABLE) {
                Table(current_pos).draw();
            }
        } else if (current_mode == LM_COLOR_BOX) {
            for (auto [p0, p1, color] : current_room.ColorBoxes) {
                draw_rect2(p0 - global_variables::camera.pos, p1 - global_variables::camera.pos, color);
            }

            if (IS_DOWN(BUTTON_MOUSE_L)) {
                Dot p0 = downed_pos - global_variables::camera.pos;
                Dot p1 = current_pos - global_variables::camera.pos;

                cast_to_top_left_and_bottom_right(p0, p1);
                draw_rect2(p0, p1, Color(0x00ff00, 30));
            }
        }
    }

    draw_rect(cursor_pos, Dot(1, 1) / global_variables::render_scale / 300, RED);
    // draw_rect(cursor.pos, Dot(1, 1) * 0.1, RED);
    if (current_mode == LM_RENDER_LEVEL) {
        draw_object(current_level, cursor_pos + Dot(3, 0), 0.7, RED);
    }

    if (current_mode == LM_SPRITE) {
        draw_object(
            sprite_type_to_string(current_type_of_sprite), Dot(20, -global_variables::arena_half_size.y + 5), 0.5, GREEN
        );
    }
}

void simulate_input(const Input &input, Dot &cursor_pos) {
    // change mode
    if (PRESSED(BUTTON_C)) {
        if (current_mode == LM_SPRITE) {
            current_mode = LM_RENDER_LEVEL;
        } else if (current_mode == LM_RENDER_LEVEL) {
            current_mode = LM_COLLISION_BOX;
        } else if (current_mode == LM_COLLISION_BOX) {
            current_mode = LM_OBJECT;
        } else if (current_mode == LM_OBJECT) {
            current_mode = LM_INTERESTING_DOT;
        } else if (current_mode == LM_INTERESTING_DOT) {
            current_mode = LM_COLOR_BOX;
        } else if (current_mode == LM_COLOR_BOX) {
            current_mode = LM_SPRITE;
        } else {
            ASSERT(false, "what is mode?");
        }
    }

    // change choose object
    if (PRESSED(BUTTON_Q)) {
        if (current_mode == LM_SPRITE) {
            int sprite_id = static_cast<int>(current_type_of_sprite);
            sprite_id--;
            if (sprite_id < 0) {
                sprite_id += SP_COUNT;
            }
            current_type_of_sprite = static_cast<sprite_t>(sprite_id);
        } else if (current_mode == LM_RENDER_LEVEL) {
            current_level--;
        } else if (current_mode == LM_OBJECT) {
            int type_id = static_cast<int>(current_object);
            type_id--;
            if (type_id < 0) {
                type_id += OT_COUNT;
            }
            current_object = static_cast<object_type>(type_id);
        }
    } else if (PRESSED(BUTTON_E)) {
        if (current_mode == LM_SPRITE) {
            int sprite_id = static_cast<int>(current_type_of_sprite);
            sprite_id++;
            if (sprite_id == SP_COUNT) {
                sprite_id = 0;
            }
            current_type_of_sprite = static_cast<sprite_t>(sprite_id);
        } else if (current_mode == LM_RENDER_LEVEL) {
            current_level++;
        } else if (current_mode == LM_OBJECT) {
            int type_id = static_cast<int>(current_object);
            type_id++;
            if (type_id == OT_COUNT) {
                type_id = 0;
            }
            current_object = static_cast<object_type>(type_id);
        }
    }

    if (IS_DOWN(BUTTON_SHIFT)) {
        Dot better = Dot(1e9, 1e9);
        // найти близжайший рисуемый прямоугольник
        for (auto [pos, size, sprite, level] : current_room.Draw_objects) {
            const auto &pixels = get_sprite(sprite);

            for (int i = 0; i < pixels.height(); i++) {
                for (int j = 0; j < pixels.width(); j++) {
                    if (global_variables::debug_mode || is_draw(pixels[i][j])) {
                        for (int x = 0; x <= 1; x++) {
                            for (int y = 0; y <= 1; y++) {
                                Dot draw_pos = pos + Dot(j + x, -i - y) * size;
                                if ((draw_pos - current_pos).get_len() < (better - current_pos).get_len()) {
                                    better = draw_pos;
                                }
                            }
                        }
                    }
                }
            }
        }

        cursor_pos -= current_pos - better;
        current_pos = better;
    }

    // set object
    if (PRESSED(BUTTON_MOUSE_L)) {
        if (current_mode == LM_SPRITE) {
            Dot pos = current_pos;
            if (IS_DOWN(BUTTON_R)) {
                pos.x -= get_sprite(current_type_of_sprite).width() * current_size;
            }

            if (IS_DOWN(BUTTON_T)) {
                pos.y += get_sprite(current_type_of_sprite).height() * current_size;
            }

            current_room.Draw_objects.push_back({pos, current_size, current_type_of_sprite, current_level});
        } else if (current_mode == LM_RENDER_LEVEL) {
            int need_highlight = -1;

            for (int i = static_cast<int>(current_room.Draw_objects.size()) - 1; i >= 0; i--) {
                auto [pos, size, sprite, level] = current_room.Draw_objects[i];
                if (collision_in_draw_sprite(pos, size, sprite, cursor_pos)) {
                    need_highlight = i;
                    break;
                }
            }

            if (need_highlight != -1) {
                current_room.Draw_objects[need_highlight].level = current_level;
            }
        } else if (current_mode == LM_INTERESTING_DOT) {
            current_room.Interesting_dots.push_back({current_pos, "none"});
        } else if (current_mode == LM_OBJECT) {
            Objects.push_back({current_pos, current_object});
        }
    }

    // remove object
    if (PRESSED(BUTTON_MOUSE_R)) {
        if (current_mode == LM_SPRITE) {
            for (int i = static_cast<int>(current_room.Draw_objects.size()) - 1; i >= 0; i--) {
                auto [pos, size, sprite, level] = current_room.Draw_objects[i];
                if (collision_in_draw_sprite(pos, size, sprite, cursor_pos)) {
                    current_room.Draw_objects.erase(current_room.Draw_objects.begin() + i);
                    break;
                }
            }
        } else if (current_mode == LM_COLLISION_BOX) {
            for (int i = static_cast<int>(current_room.Walls.size()) - 1; i >= 0; i--) {
                if (current_room.Walls[i].trigger(current_pos)) {
                    current_room.Walls.erase(current_room.Walls.begin() + i);
                    break;
                }
            }
        } else if (current_mode == LM_INTERESTING_DOT) {
            for (int i = 0; i < static_cast<int>(current_room.Interesting_dots.size()); i++) {
                if ((current_pos - current_room.Interesting_dots[i].pos).get_len() < 1) {
                    current_room.Interesting_dots.erase(current_room.Interesting_dots.begin() + i);
                    break;
                }
            }
        } else if (current_mode == LM_OBJECT) {
            Dot pos = current_pos - global_variables::camera.pos;
            for (int i = static_cast<int>(Objects.size()) - 1; i >= 0; i--) {
                if ((Objects[i].second == OT_BUSH && Bush(Objects[i].first).trigger_in_draw_sprite(pos)) ||
                    (Objects[i].second == OT_TREE && Tree(Objects[i].first).trigger_in_draw_sprite(pos)) ||
                    (Objects[i].second == OT_TABLE && Table(Objects[i].first).trigger_in_draw_sprite(pos))) {
                    Objects.erase(Objects.begin() + i);
                    break;
                }
            }
        } else if (current_mode == LM_COLOR_BOX) {
            for (int i = static_cast<int>(current_room.ColorBoxes.size()) - 1; i >= 0; i--) {
                if (CollisionBox(std::get<0>(current_room.ColorBoxes[i]), std::get<1>(current_room.ColorBoxes[i]))
                        .trigger(current_pos)) {
                    current_room.ColorBoxes.erase(current_room.ColorBoxes.begin() + i);
                    break;
                }
            }
        }
    }

    if (PRESSED(BUTTON_MOUSE_L)) {
        downed_pos = current_pos;
    }

    // set collision_box
    if (current_mode == LM_COLLISION_BOX && RELEASED(BUTTON_MOUSE_L)) {
        Dot p0 = downed_pos;
        Dot p1 = current_pos;

        cast_to_top_left_and_bottom_right(p0, p1);

        current_room.Walls.emplace_back(p0, p1);
    } else if (current_mode == LM_COLOR_BOX && RELEASED(BUTTON_MOUSE_L)) {
        Dot p0 = downed_pos;
        Dot p1 = current_pos;

        cast_to_top_left_and_bottom_right(p0, p1);

        current_room.ColorBoxes.emplace_back(p0, p1, RED);
    }

    // save level
    if (PRESSED(BUTTON_V)) {
        std::sort(current_room.Draw_objects.begin(), current_room.Draw_objects.end());
        current_room.write("level.txt", Objects);
    }
}

void simulate_leve_maker(const Input &input, Dot cursor_pos, efloat delta_time) {
    current_pos = global_variables::camera.pos + cursor_pos;

    simulate_input(input, cursor_pos);
    if (!global_variables::running) {
        return;
    }

    // simulate camera move
    {
        // накопление вектора движения
        auto accum_ddp = [&input](button_t left, button_t right, button_t top, button_t bottom) -> Dot {
            return Dot(IS_DOWN(right) - IS_DOWN(left), IS_DOWN(top) - IS_DOWN(bottom));
        };

        static Dot dp;
        simulate_move2d(
            global_variables::camera.pos, dp,
            accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S) * 10 / global_variables::render_scale, delta_time
        );
    }

    current_size = global_variables::mouse_wheel / 10'000 + 1;

    render_game(input, cursor_pos);

    draw_object(current_size, global_variables::arena_half_size + Dot(-20, -20), 0.5, BLUE);

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
        } else if (current_mode == LM_COLOR_BOX) {
            mode_str = "color box";
        }
        draw_object(mode_str, Dot(0, -global_variables::arena_half_size.y + 5), 0.5, RED);
    }
}