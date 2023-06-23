#include "room.hpp"
//
#include "../../render.hpp"
#include "game_objects.hpp"
#include "game_variables.hpp"

bool drawing_objects::operator<(const drawing_objects &right) const {
    return level < right.level;
}

std::string object_type_to_string(object_type type) {
    if (type == OT_BUSH) {
        return "BUSH";
    } else if (type == OT_TREE) {
        return "TREE";
    } else if (type == OT_TABLE) {
        return "TABLE";
    } else if (type == OT_PILLAR) {
        return "PILLAR";
    } else if (type == OT_NUN_STATUE) {
        return "NUN_STATUE";
    } else if (type == OT_KNIGHT) {
        return "KNIGHT";
    } else if (type == OT_BARREL) {
        return "BARREL";
    }
    ASSERT(false, "type=?");
}

object_type string_to_object_type(const std::string &str) {
    if (str == "BUSH") {
        return OT_BUSH;
    } else if (str == "TREE") {
        return OT_TREE;
    } else if (str == "TABLE") {
        return OT_TABLE;
    } else if (str == "PILLAR") {
        return OT_PILLAR;
    } else if (str == "NUN_STATUE") {
        return OT_NUN_STATUE;
    } else if (str == "KNIGHT") {
        return OT_KNIGHT;
    } else if (str == "BARREL") {
        return OT_BARREL;
    }
    ASSERT(false, "str=?");
}

void Room::build_grid() {
    get_ref_grid_start() = grid_start_dot;  // use this grid start position

    const static std::vector<grid_pos_t> steps = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    //{{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    visitable_grid_dots_for_ground_mob.clear();
    visitable_grid_dots_for_air_mob.clear();
    std::queue<grid_pos_t> queue;

    queue.push({0, 0});

    while (!queue.empty()) {
        auto grid_pos = queue.front();
        queue.pop();

        for (const auto &step : steps) {
            grid_pos_t grid_neighbour = {grid_pos.first + step.first, grid_pos.second + step.second};
            Dot pos = grid_start_dot + step_size * Dot(grid_neighbour.first, grid_neighbour.second);
            // мы еще не были в этой точке и можем ее посетить
            if (!visitable_grid_dots_for_air_mob.count(grid_neighbour)) {
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
                    visitable_grid_dots_for_air_mob.insert(grid_neighbour);
                }
            }
        }
    }

    // мы построили гряд для воздушных
    // грид для наземных это грид для воздушных - те точки, в которых мы быть не можем

    for (auto grid : visitable_grid_dots_for_air_mob) {
        bool is_visitable = true;

        Dot pos = grid_start_dot + step_size * Dot(grid.first, grid.second);
        Slime slime_tester(pos);

        for (const auto &bush : game_variables::Bushes) {
            if (bush.get_collision()->trigger(*slime_tester.get_collision())) {
                is_visitable = false;
                break;
            }
        }
        for (const auto &tree : game_variables::Trees) {
            if (tree.get_collision()->trigger(*slime_tester.get_collision())) {
                is_visitable = false;
                break;
            }
        }

        if (is_visitable) {
            queue.push(grid);
            visitable_grid_dots_for_ground_mob.insert(grid);
        }
    }
}

std::vector<std::pair<Dot, object_type>> Room::read(const std::string &filename) {
    // clear
    {
        Draw_objects.clear();
        Walls.clear();
        Interesting_dots.clear();
        game_variables::Bushes.clear();
        game_variables::Trees.clear();
        game_variables::Tables.clear();
        game_variables::Pillars.clear();
        game_variables::NunStatues.clear();
        game_variables::Knights.clear();
        game_variables::Barrels.clear();
        ColorBoxes.clear();

        visitable_grid_dots_for_air_mob.clear();
        visitable_grid_dots_for_ground_mob.clear();

        wave_number = 0;
        wave_cooldown_accum = 0;

        game_variables::Loot_coins.clear();
        game_variables::Loot_hearts.clear();

        game_variables::Slimes.clear();
        game_variables::Bats.clear();
        game_variables::Bombers.clear();

        game_variables::Weapons.clear();

        for (auto &player : game_variables::Players) {
            player.weapon_ind = -1;
        }
    }

    {
        // filename = "room_name.txt"
        room_name = filename;
        room_name.pop_back();  // t
        room_name.pop_back();  // x
        room_name.pop_back();  // t
        room_name.pop_back();  // .
    }

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

    std::vector<std::pair<Dot, object_type>> Objects;

    {
        std::string str;
        file >> str;  // OBJECTS
        ASSERT(str == "OBJECTS", "failed read");
        int count;
        file >> count;
        Objects.resize(count);
        for (int index = 0; index < count; index++) {
            Dot pos;
            std::string str_object_type;
            file >> pos >> str_object_type;
            object_type type = string_to_object_type(str_object_type);
            Objects[index] = {pos, type};
            if (type == OT_BUSH) {
                game_variables::Bushes.emplace_back(pos);
            } else if (type == OT_TREE) {
                game_variables::Trees.emplace_back(pos);
            } else if (type == OT_TABLE) {
                game_variables::Tables.emplace_back(pos);
            } else if (type == OT_PILLAR) {
                game_variables::Pillars.emplace_back(pos);
            } else if (type == OT_NUN_STATUE) {
                game_variables::NunStatues.emplace_back(pos);
            } else if (type == OT_KNIGHT) {
                game_variables::Knights.emplace_back(pos);
            } else if (type == OT_BARREL) {
                game_variables::Barrels.emplace_back(pos);
            } else {
                ASSERT(false, "type=?");
            }
        }
    }

    {
        std::string str;
        file >> str;
        ASSERT(str == "COLOR_BOXES", "failed read");
        int count;
        file >> count;
        ColorBoxes.resize(count);
        for (auto &[top_left, bottom_right, color] : ColorBoxes) {
            file >> top_left >> bottom_right >> color;
        }
    }

    {
        std::string str;
        file >> str;  // GRID_START_DOT
        ASSERT(str == "GRID_START_DOT", "failed read");
        file >> grid_start_dot;
    }

    {
        std::vector<Dot> player_teleport_dots;
        for (auto [pos, name] : Interesting_dots) {
            if (name.size() >= 6 && name.substr(0, 6) == "player") {
                player_teleport_dots.emplace_back(pos);
            }
        }
        for (auto &player : game_variables::Players) {
            player.pos = player_teleport_dots[get_random_engine()() % player_teleport_dots.size()];
        }
    }

    // TODO: пока коллизии не построены такое не надо вызывать
    build_grid();

    std::cout << "visitable grid dots for air mob: " << visitable_grid_dots_for_air_mob.size() << std::endl;
    std::cout << "visitable grid dots for ground mob: " << visitable_grid_dots_for_ground_mob.size() << std::endl;

    return Objects;
}

void Room::write(const std::string &filename, const std::vector<std::pair<Dot, object_type>> &Objects) {
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

    file << "OBJECTS\n";
    file << Objects.size() << '\n';
    for (auto [pos, object_type] : Objects) {
        file << pos << ' ' << object_type_to_string(object_type) << '\n';
    }

    file << "COLOR_BOXES\n";
    file << ColorBoxes.size() << '\n';
    for (auto [top_left, bottom_right, color] : ColorBoxes) {
        file << top_left << ' ' << bottom_right << ' ' << color << '\n';
    }

    file << "GRID_START_DOT\n";
    file << grid_start_dot << '\n';
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

    if (game_variables::Slimes.size() + game_variables::Bats.size() + game_variables::Bombers.size() == 0) {
        wave_cooldown_accum += delta_time;
        if (wave_cooldown_accum > wave_cooldown) {
            std::cout << "New wave!" << std::endl;
            wave_cooldown_accum = 0;
            wave_number++;

            for (auto [pos, name] : Interesting_dots) {
                if (name.size() < 5 || name.substr(0, 6) != "player") {
                    if (name == "sniper_rifle") {
                        if (randomness(30)) {
                            game_variables::Weapons.emplace_back(pos, weapon_t::SNIPER_RIFLE, 3);
                        }
                    } else if (name == "staff") {
                        if (randomness(50)) {
                            game_variables::Weapons.emplace_back(pos, STAFF);
                        }
                    } else if (name == "golden_gun") {
                        if (randomness(70)) {
                            game_variables::Weapons.emplace_back(pos, GOLDEN_GUN);
                        }
                    } else if (name == "rifle") {
                        if (randomness(40)) {
                            game_variables::Weapons.emplace_back(pos, RIFLE, 0.1);
                        }
                    } else {  // enemy
                        int enemy_cnt = 1;
                        game_variables::Slimes.emplace_back(pos);
                        break;
                        if (randomness(std::min(100, 20 * wave_number))) {
                            enemy_cnt++;
                            if (randomness(std::min(100, 5 * wave_number))) {
                                enemy_cnt++;
                            }
                        }

                        while (enemy_cnt--) {
                            if (randomness(40)) {
                                game_variables::Bombers.emplace_back(pos);
                            } else if (randomness(30)) {
                                game_variables::Bats.emplace_back(pos);
                            } else {
                                game_variables::Slimes.emplace_back(pos);
                            }
                        }
                    }
                }
            }
        }

        /*for (auto [pos, name] : Interesting_dots) {
            if (name.size() < 5 || name.substr(0, 6) != "player") {
                if (name == "sniper_rifle") {
                    game_variables::Weapons.emplace_back(pos, weapon_t::SNIPER_RIFLE, 3);
                } else if (name == "staff") {
                    game_variables::Weapons.emplace_back(pos, STAFF);
                } else if (name == "golden_gun") {
                    game_variables::Weapons.emplace_back(pos, GOLDEN_GUN);
                } else if (name == "rifle") {
                    game_variables::Weapons.emplace_back(pos, RIFLE, 0.1);
                } else {
                    if (randomness(40)) {
                        game_variables::Bombers.emplace_back(pos);
                    } else if (randomness(30)) {
                        game_variables::Bats.emplace_back(pos);
                    } else {
                        game_variables::Slimes.emplace_back(pos);
                    }
                }
            }
        }*/

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

    for (auto &knight : game_variables::Knights) {
        simulate_move2d(knight.pos, knight.dp, Dot(), delta_time);
    }
    for (auto &barrel : game_variables::Barrels) {
        simulate_move2d(barrel.pos, barrel.dp, Dot(), delta_time);
    }
    for (auto &table : game_variables::Tables) {
        simulate_move2d(table.pos, table.dp, Dot(), delta_time);
    }

    for (auto &slime : game_variables::Slimes) {
        slime.simulate(delta_time, visitable_grid_dots_for_ground_mob, Walls);
    }
    for (auto &bat : game_variables::Bats) {
        bat.simulate(delta_time, visitable_grid_dots_for_air_mob);
    }
    for (int i = 0; i < static_cast<int>(game_variables::Bombers.size()); i++) {
        game_variables::Bombers[i].simulate(delta_time, visitable_grid_dots_for_ground_mob);
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
        for (auto &bullet : game_variables::game_variables::Bullets) {
            bullet.simulate(delta_time);
        }

        // bullet hit enemies
        for (uint32_t i = 0; i < game_variables::Bullets.size(); i++) {
            if (game_variables::Bullets[i].simulate_attack_on_mob(game_variables::Slimes) ||
                game_variables::Bullets[i].simulate_attack_on_mob(game_variables::Bats) ||
                game_variables::Bullets[i].simulate_attack_on_mob(game_variables::Bombers) ||
                game_variables::Bullets[i].simulate_attack_on_player(game_variables::Players)) {
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
            } else if (game_variables::Effects[i].anim.sprite_sheet == SS_FLOWER_EFFECT) {
                Circle heal(game_variables::Effects[i].pos, 5);
                for (auto &player : game_variables::Players) {
                    if (heal.isin(player.pos) && player.healing_cooldown <= 0) {
                        player.hp++;
                        player.healing_cooldown += 1;
                    }
                }
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
        for (auto &tree : game_variables::Trees) {
            Objects.emplace_back(&tree);
        }
        for (auto &pillar : game_variables::Pillars) {
            Objects.emplace_back(&pillar);
        }
        for (auto &nun_statue : game_variables::NunStatues) {
            Objects.emplace_back(&nun_statue);
        }
        for (auto &knight : game_variables::Knights) {
            Objects.emplace_back(&knight);
        }
        for (auto &barrel : game_variables::Barrels) {
            Objects.emplace_back(&barrel);
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
        for (auto &bat : game_variables::Bats) {
            Objects.emplace_back(&bat);
        }
        for (auto &bomber : game_variables::Bombers) {
            if (!bomber.is_booming) {
                Objects.emplace_back(&bomber);
            }
        }
        for (auto &weapon : game_variables::Weapons) {
            if (!weapon.is_picked) {
                Objects.emplace_back(&weapon);
            }
        }
        for (auto &effect : game_variables::Effects) {
            if (effect.anim.sprite_sheet == SS_FLOWER_EFFECT) {
                draw_circle(Circle(effect.pos - global_variables::camera.pos, 5), Color(0x00ff00, 128));
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

    for (auto [top_left, bottom_right, color] : ColorBoxes) {
        draw_rect2(top_left - global_variables::camera.pos, bottom_right - global_variables::camera.pos, color);
    }
}
