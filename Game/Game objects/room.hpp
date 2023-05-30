#ifndef GAME_ROOM_HPP
#define GAME_ROOM_HPP

#include "../../objects.hpp"
#include "../../render.hpp"
#include "../../sprites.hpp"
#include "game_objects.hpp"

struct drawing_objects {
    Dot pos;
    efloat size;
    sprite_t sprite;
    int level;

    bool operator<(const drawing_objects &right) const {
        return level < right.level;
    }
};

struct interesting_dot {
    Dot pos;
    std::string name;
};

struct Room {
    // room sprites
    std::vector<drawing_objects> Draw_objects;

    // walls
    std::vector<CollisionBox> Walls;

    std::vector<interesting_dot> Interesting_dots;

    std::set<grid_pos_t> visitable_grid_dots;

    Dot grid_start_dot;

    bool player_spawned = false;
    int wave_number = 0;
    efloat wave_cooldown = 2;
    efloat wave_cooldown_accum = 0;

    void build_grid() {
        grid_start = grid_start_dot;  // use this grid start position

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

    void read(const std::string &filename) {
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

    void write(const std::string &filename) {
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

    void simulate(efloat delta_time) {
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

        if (Slimes.size() + Bats.size() == 0) {
            // new wave

            std::cout << "New wave!" << std::endl;

            for (auto [pos, name] : Interesting_dots) {
                if (name != "player") {
                    // Bats.emplace_back(pos);
                    // Slimes.emplace_back(pos);
                    // Slimes.emplace_back(pos);
                    // Slimes.emplace_back(pos);
                    // Slimes.emplace_back(pos);
                    // continue;
                    if (randomness(50)) {
//                        Bats.emplace_back(pos);
                        Bombers.emplace_back(pos);
                    } else {
                        Slimes.emplace_back(pos);
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
                            Bats.emplace_back(pos);
                        } else {
                            Slimes.emplace_back(pos);
                        }
                    }
                }
            }*/
        }

        for (auto &slime : Slimes) {
            slime.simulate(delta_time, visitable_grid_dots, Walls);
        }
        for (auto &bat : Bats) {
            bat.simulate(delta_time, visitable_grid_dots);
        }
        for (int i = 0; i < static_cast<int>(Bombers.size()); i++) {
            Bombers[i].simulate(delta_time, visitable_grid_dots);
            if (Bombers[i].is_deleted) {
                Bombers.erase(Bombers.begin() + i);
                i--;
            }
        }
        // simulate bullets
        {
            for (auto &bullet : Bullets) {
                bullet.simulate(delta_time);
            }

            // bullet hit enemies
            for (int i = 0; i < static_cast<int>(Bullets.size()); i++) {
                if (Bullets[i].simulate_attack_on_mob(Slimes) || Bullets[i].simulate_attack_on_mob(Bats) ||
                Bullets[i].simulate_attack_on_mob(Bombers) || Bullets[i].simulate_attack_on_player(Players)) {
                    Bullets.erase(Bullets.begin() + i);
                    i--;
                }
            }

            // bullet hit wall
            for (uint32_t i = 0; i < Bullets.size(); i++) {
                bool need_delete = false;
                for (auto &collision_box : Walls) {
                    if (collision_box.trigger(Bullets[i].pos)) {
                        need_delete = true;
                    }
                }
                if (need_delete) {
                    add_hit_effect(Bullets[i].pos);
                    Bullets.erase(Bullets.begin() + i);
                    i--;
                }
            }
        }

        // simulate effects
        {
            for (int i = 0; i < Effects.size(); i++) {
                if (Effects[i].simulate(delta_time)) {
                    Effects.erase(Effects.begin() + i);
                    i--;
                }
            }
        }

        // simulate loot
        {
            // heart
            {
                for (auto &heart : Loot_hearts) {
                    heart.simulate(delta_time);
                }
                for (int i = 0; i < static_cast<int>(Loot_hearts.size()); i++) {
                    auto &heart = Loot_hearts[i];
                    if (heart.simulate_collection()) {
                        Loot_hearts.erase(Loot_hearts.begin() + i);
                        i--;
                    }
                }
            }
            // coin
            {
                for (auto &coin : Loot_coins) {
                    coin.simulate(delta_time);
                }
                for (int i = 0; i < static_cast<int>(Loot_coins.size()); i++) {
                    auto &coin = Loot_coins[i];
                    if (coin.simulate_collection()) {
                        Loot_coins.erase(Loot_coins.begin() + i);
                        i--;
                    }
                }
            }
        }

        simulate_game_collisions(delta_time, Walls);
    }

    void draw() {
        clear_screen(GREY);

        for (auto [pos, size, sprite, level] : Draw_objects) {
            if (level < 0) {
                draw_sprite(pos, size, sprite);
            }
        }

        // top sort
        {
            struct top_sort_object {
                enum type_object {
                    TO_UNDEFINED,

                    TO_PLAYER,
                    TO_SLIME,
                    TO_BAT,
                    TO_HEART,
                    TO_COIN,
                    TO_BUSH,
                    TO_TABLE,
                    TO_BOMBER,
                };

                type_object type;
                const void *ptr;

                top_sort_object() {
                    type = TO_UNDEFINED;
                    ptr = nullptr;
                }

                explicit top_sort_object(const Player &player) {
                    type = TO_PLAYER;
                    ptr = reinterpret_cast<const void *>(&player);
                }

                explicit top_sort_object(const Bat &player) {
                    type = TO_BAT;
                    ptr = reinterpret_cast<const void *>(&player);
                }

                explicit top_sort_object(const Slime &slime) {
                    type = TO_SLIME;
                    ptr = reinterpret_cast<const void *>(&slime);
                }

                explicit top_sort_object(const Heart &heart) {
                    type = TO_HEART;
                    ptr = reinterpret_cast<const void *>(&heart);
                }

                explicit top_sort_object(const Coin &coin) {
                    type = TO_COIN;
                    ptr = reinterpret_cast<const void *>(&coin);
                }

                explicit top_sort_object(const Bush &bush) {
                    type = TO_BUSH;
                    ptr = reinterpret_cast<const void *>(&bush);
                }

                explicit top_sort_object(const Table &table) {
                    type = TO_TABLE;
                    ptr = reinterpret_cast<const void *>(&table);
                }

                explicit top_sort_object(const Bomber &bomber) {
                    type = TO_BOMBER;
                    ptr = reinterpret_cast<const void*>(&bomber);
                }

                [[nodiscard]] efloat get_y() const {
                    switch (type) {
                        case TO_PLAYER: {
                            return reinterpret_cast<const Player *>(ptr)->pos.y;
                        }
                        case TO_SLIME: {
                            return reinterpret_cast<const Slime *>(ptr)->pos.y;
                        }
                        case TO_BAT: {
                            return reinterpret_cast<const Bat *>(ptr)->pos.y;
                        }
                        case TO_HEART: {
                            return reinterpret_cast<const Heart *>(ptr)->pos.y;
                        }
                        case TO_COIN: {
                            return reinterpret_cast<const Coin *>(ptr)->pos.y;
                        }
                        case TO_BUSH: {
                            return reinterpret_cast<const Bush *>(ptr)->pos.y;
                        }
                        case TO_TABLE: {
                            return reinterpret_cast<const Table *>(ptr)->pos.y;
                        }
                        case TO_BOMBER: {
                            return reinterpret_cast<const Bomber *>(ptr)->pos.y;
                        }
                        default:
                            ASSERT(false, "undefined object type");
                    }
                    return 0;
                }

                void draw() const {
                    switch (type) {
                        case TO_PLAYER: {
                            reinterpret_cast<const Player *>(ptr)->draw();
                        } break;
                        case TO_SLIME: {
                            reinterpret_cast<const Slime *>(ptr)->draw();
                        } break;
                        case TO_BAT: {
                            reinterpret_cast<const Bat *>(ptr)->draw();
                        } break;
                        case TO_HEART: {
                            reinterpret_cast<const Heart *>(ptr)->draw();
                        } break;
                        case TO_COIN: {
                            reinterpret_cast<const Coin *>(ptr)->draw();
                        } break;
                        case TO_BUSH: {
                            reinterpret_cast<const Bush *>(ptr)->draw();
                        } break;
                        case TO_TABLE: {
                            reinterpret_cast<const Table *>(ptr)->draw();
                        } break;
                        case TO_BOMBER: {
                            reinterpret_cast<const Bomber*>(ptr)->draw();
                        } break;
                        default: {
                            ASSERT(false, "undefind object type");
                        }
                    }
                }

                bool operator<(const top_sort_object &Rhs) const {
                    return get_y() > Rhs.get_y();
                }
            };

            std::vector<top_sort_object> Objects;

            for (auto &heart : Loot_hearts) {
                Objects.emplace_back(heart);
            }
            for (auto &coin : Loot_coins) {
                Objects.emplace_back(coin);
            }

            for (auto &bush : Bushes) {
                Objects.emplace_back(bush);
            }
            for (auto &table : Tables) {
                Objects.emplace_back(table);
            }

            for (auto &player : Players) {
                Objects.emplace_back(player);
            }
            for (auto &slime : Slimes) {
                Objects.emplace_back(slime);
            }
            for (auto &bat : Bats) {
                Objects.emplace_back(bat);
            }
            for (auto &bomber: Bombers) {
                if (!bomber.is_booming) {
                    Objects.emplace_back(bomber);
                }
            }
            std::stable_sort(Objects.begin(), Objects.end());

            for (auto &obj : Objects) {
                obj.draw();
            }
        }

        // draw bullets
        for (auto &bullet : Bullets) {
            bullet.draw();
        }

        // draw effects
        for (auto &Effect : Effects) {
            Effect.draw();
        }

        for (auto [pos, size, sprite, level] : Draw_objects) {
            if (level > 0) {
                draw_sprite(pos, size, sprite);
            }
        }

        for (auto &bomber: Bombers) {
            if (bomber.is_booming) {
                bomber.draw();
            }
        }

        // draw grid
        {
            // draw grid start
            draw_rect(grid_start_dot - global_variables::camera.pos, Dot(1, 1), YELLOW);

            /*for(auto grid_pos : visitable_grid_dots){
                Dot pos = grid_start_dot + step_size * Dot(grid_pos.first, grid_pos.second);
                draw_rect(pos - global_variables::camera.pos, Dot(0.5, 0.5), YELLOW);
            }*/
        }
    }
};

#endif  // GAME_ROOM_HPP