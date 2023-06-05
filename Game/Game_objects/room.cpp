#include "room.hpp"



    bool drawing_objects::operator<(const drawing_objects &right) const {
        return level < right.level;
    }





    void Room::build_grid() {
        get_ref_grid_start() = grid_start_dot;  // use this grid start position

        const static std::vector<grid_pos_t> steps = {{1, 0}, {-1, 0}, {0, 1},  {0, -1},
                                                      {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        //{{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

        visitable_grid_dots.clear();
        std::queue<grid_pos_t> queue;

        queue.push({0, 0});

        while (!queue.empty()) {
            auto grid_pos = queue.front();
            queue.pop();

            for (const auto &step : steps) {
                grid_pos_t grid_neighbour = {grid_pos.first + step.first, grid_pos.second + step.second};
                Dot pos = grid_start_dot + step_size * Dot(grid_neighbour.first, grid_neighbour.second);
                // мы еще не были в этой точке и можем ее посетить
                if (!visitable_grid_dots.count(grid_neighbour)) {
                    bool is_visitable = true;

                    Slime slime_tester(pos);

                    for (const auto &wall : Walls) {
                        if (wall.trigger(*slime_tester.get_collision())) {
                            is_visitable = false;
                            break;
                        }
                    }

                    if (is_visitable) {
                        queue.push(grid_neighbour);
                        visitable_grid_dots.insert(grid_neighbour);
                    }
                }
            }
        }
    }

    void Room::read(const std::string &filename) {
        std::ifstream file(filename);
        {
            std::string str;
            file >> str;  // SPRITES
            ASSERT(str == "SPRITES", "failed read");

            int count;
            file >> count;
            Draw_objects.assign(count, {});
            for (auto &[pos, size, sprite, level] : Draw_objects) {
                std::string sprite_name;
                file >> sprite_name >> pos >> size >> level;
                sprite = string_to_sprite_type(sprite_name);
            }
        }
        {
            std::string str;
            file >> str;  // COLLISION_BOXES
            ASSERT(str == "COLLISION_BOXES", "failed read");

            int count;
            file >> count;
            Walls.assign(count, {});
            for (auto &[p0, p1] : Walls) {
                file >> p0 >> p1;
            }
        }
        {
            std::string str;
            file >> str;  // INTERESTING_DOTS
            ASSERT(str == "INTERESTING_DOTS", "failed read");

            int count;
            file >> count;
            Interesting_dots.assign(count, {});
            for (auto &[pos, name] : Interesting_dots) {
                file >> pos >> name;
            }
        }

        {
            std::string str;
            file >> str;  // GRID_START_DOT
            ASSERT(str == "GRID_START_DOT", "failed read");
            file >> grid_start_dot;
        }

        build_grid();

        std::cout << visitable_grid_dots.size() << std::endl;
    }

    void Room::write(const std::string &filename) {
        std::ofstream file(filename);
        file << std::fixed << std::setprecision(10);

        file << "SPRITES\n";
        file << Draw_objects.size() << '\n';
        for (auto [pos, size, sprite, level] : Draw_objects) {
            file << sprite_type_to_string(sprite) << ' ' << pos << ' ' << size << ' ' << level << '\n';
        }

        file << "COLLISION_BOXES\n";
        file << Walls.size() << '\n';
        for (auto [p0, p1] : Walls) {
            file << p0 << ' ' << p1 << '\n';
        }

        file << "INTERESTING_DOTS\n";
        file << Interesting_dots.size() << '\n';
        for (auto [pos, name] : Interesting_dots) {
            file << pos << ' ' << name << '\n';
        }

        ASSERT(false, "need to update");
    }

    void Room::simulate(efloat delta_time) {
        /*if (!player_spawned) {
            for (auto &player : Players) {
                for (auto [pos, name] : Interesting_dots) {
                    if (name == "player") {
                        player.pos = pos;
                        player_spawned = true;
                        global_variables::camera.pos = pos;
                        break;
                    }
                }
            }
        }*/

        if (game_variables::Slimes.size() + game_variables:: Bats.size() + game_variables::Bombers.size() == 0) {
            // new wave

            std::cout << "New wave!" << std::endl;

            for (auto [pos, name] : Interesting_dots) {
                if (name != "player") {
                    if (randomness(40)) {
                        game_variables::Bombers.emplace_back(pos);
                    } else if (randomness(30)) {
                        game_variables:: Bats.emplace_back(pos);
                    } else {
                        game_variables::Slimes.emplace_back(pos);
                    }
                }
            }

            /*wave_cooldown_accum += delta_time;
            if (wave_cooldown_accum >= wave_cooldown) {
                wave_cooldown_accum = 0;
                ++wave_number;

                for (auto [pos, name] : Interesting_dots) {
                    if (name == "enemy") {
                        if (randomness(50)) {
                            game_variables:: Bats.emplace_back(pos);
                        } else {
                            game_variables::Slimes.emplace_back(pos);
                        }
                    }
                }
            }*/
        }

        for (auto &slime : game_variables::Slimes) {
            slime.simulate(delta_time, visitable_grid_dots, Walls);
        }
        for (auto &bat : game_variables:: Bats) {
            bat.simulate(delta_time, visitable_grid_dots);
        }
        for (int i = 0; i < static_cast<int>(game_variables::Bombers.size()); i++) {
            game_variables::Bombers[i].simulate(delta_time, visitable_grid_dots);
            if (game_variables::Bombers[i].hp <= 0) {
                game_variables::Bombers.erase(game_variables::Bombers.begin() + i);
                i--;
            }
        }
        for (auto &bomber1 : game_variables::Bombers) {
            if (bomber1.is_booming && bomber1.anim.frame_cur_count > 5 && bomber1.anim.frame_cur_count < 12) {
                for (auto &bomber2 : game_variables::Bombers) {
                    if (!bomber2.is_booming && (bomber1.pos - bomber2.pos).get_len() < Bomber::boom_radius) {
                        // цепная реакция!
                        bomber2.do_boom();
                    }
                }
            }
        }

        // simulate bullets
        {
            for (auto &bullet : game_variables:: game_variables::Bullets) {
                bullet.simulate(delta_time);
            }

            // bullet hit enemies
            for (uint32_t i = 0; i < game_variables::Bullets.size(); i++) {
                if (game_variables::Bullets[i].simulate_attack_on_mob(game_variables::Slimes) || game_variables::Bullets[i].simulate_attack_on_mob(game_variables:: Bats) ||
                    game_variables::Bullets[i].simulate_attack_on_mob(game_variables::Bombers) || game_variables::Bullets[i].simulate_attack_on_player(game_variables::Players)) {
                    game_variables::Bullets.erase(game_variables::Bullets.begin() + i);
                    i--;
                }
            }
            // bullet hit wall
            for (uint32_t i = 0; i < game_variables::Bullets.size(); i++) {
                bool need_delete = false;
                for (auto &collision_box : Walls) {
                    if (collision_box.trigger(game_variables::Bullets[i].pos)) {
                        need_delete = true;
                    }
                }
                if (need_delete) {
                    add_hit_effect(game_variables::Bullets[i].pos);
                    game_variables::Bullets.erase(game_variables::Bullets.begin() + i);
                    i--;
                }
            }
        }

        // simulate effects
        {
            for (int i = 0; i < game_variables::Effects.size(); i++) {
                if (game_variables::Effects[i].simulate(delta_time)) {
                    game_variables::Effects.erase(game_variables::Effects.begin() + i);
                    i--;
                }
            }
        }

        // simulate loot
        {
            // heart
            {
                for (auto &heart : game_variables::Loot_hearts) {
                    heart.simulate(delta_time);
                }
                for (int i = 0; i < static_cast<int>(game_variables::Loot_hearts.size()); i++) {
                    auto &heart = game_variables::Loot_hearts[i];
                    if (heart.simulate_collection()) {
                        game_variables::Loot_hearts.erase(game_variables::Loot_hearts.begin() + i);
                        i--;
                    }
                }
            }
            // coin
            {
                for (auto &coin : game_variables::Loot_coins) {
                    coin.simulate(delta_time);
                }
                for (int i = 0; i < static_cast<int>(game_variables::Loot_coins.size()); i++) {
                    auto &coin = game_variables::Loot_coins[i];
                    if (coin.simulate_collection()) {
                        game_variables::Loot_coins.erase(game_variables::Loot_coins.begin() + i);
                        i--;
                    }
                }
            }
        }

        simulate_game_collisions(delta_time, Walls);
    }

    void Room::draw() {
        clear_screen(GREY);

        for (auto [pos, size, sprite, level] : Draw_objects) {
            if (level < 0) {
                draw_sprite(pos, size, sprite);
            }
        }

        // top sort
        {
            std::vector<AbstractObject *> Objects;

            for (auto &heart : game_variables::Loot_hearts) {
                Objects.emplace_back(&heart);
            }
            for (auto &coin : game_variables::Loot_coins) {
                Objects.emplace_back(&coin);
            }
            for (auto &bush : game_variables::Bushes) {
                Objects.emplace_back(&bush);
            }
            for (auto &table : game_variables::Tables) {
                Objects.emplace_back(&table);
            }
            for (auto &player : game_variables::Players) {
                Objects.emplace_back(&player);
            }
            for (auto &slime : game_variables::Slimes) {
                Objects.emplace_back(&slime);
            }
            for (auto &bat : game_variables:: Bats) {
                Objects.emplace_back(&bat);
            }
            for (auto &bomber : game_variables::Bombers) {
                if (!bomber.is_booming) {
                    Objects.emplace_back(&bomber);
                }
            }
            for (auto &effect : game_variables::Effects) {
                if (effect.anim.sprite_sheet == SS_FLOWER_EFFECT) {
                    Objects.emplace_back(&effect);
                }
            }
            std::stable_sort(Objects.begin(), Objects.end(), [](AbstractObject *lhs, AbstractObject *rhs) {
                return lhs->pos.y > rhs->pos.y;
            });

            for (auto &obj : Objects) {
                obj->draw();
            }
        }

        // draw bullets
        for (auto &bullet : game_variables::Bullets) {
            bullet.draw();
        }

        // draw effects
        for (auto &effect : game_variables::Effects) {
            if (effect.anim.sprite_sheet != SS_FLOWER_EFFECT) {
                effect.draw();
            }
        }

        for (auto [pos, size, sprite, level] : Draw_objects) {
            if (level > 0) {
                draw_sprite(pos, size, sprite);
            }
        }
        for (auto &bomber : game_variables::Bombers) {
            if (bomber.is_booming) {
                bomber.draw();
            }
        }

        // draw grid
        {
            // draw grid start
            /*draw_rect(grid_start_dot - global_variables::camera.pos, Dot(1, 1), YELLOW);

            for(auto grid_pos : visitable_grid_dots){
                Dot pos = grid_start_dot + step_size * Dot(grid_pos.first, grid_pos.second);
                draw_rect(pos - global_variables::camera.pos, Dot(0.5, 0.5), YELLOW);
            }*/
        }
    }


