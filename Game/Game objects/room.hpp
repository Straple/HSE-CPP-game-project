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
    std::vector<CollisionBox> Collision_boxes;

    std::vector<interesting_dot> Interesting_dots;

    bool player_spawned = false;
    int wave_number = 0;
    efloat wave_cooldown = 2;
    efloat wave_cooldown_accum = 0;

    void read(const std::string &filename) {
        std::ifstream file(filename);
        {
            std::string str;
            file >> str;  // SPRITES

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

            int count;
            file >> count;
            Collision_boxes.assign(count, {});
            for (auto &[p0, p1] : Collision_boxes) {
                file >> p0 >> p1;
            }
        }

        {
            std::string str;
            file >> str;  // INTERESTING_DOTS

            int count;
            file >> count;
            Interesting_dots.assign(count, {});
            for (auto &[pos, name] : Interesting_dots) {
                file >> pos >> name;
            }
        }
    }

    void write(const std::string &filename) {
        std::ofstream file(filename);
        file << std::fixed << std::setprecision(10);

        file << "SPRITES\n";
        file << Draw_objects.size() << '\n';
        for (auto [pos, size, sprite, level] : Draw_objects) {
            file << sprite_type_to_string(sprite) << ' ' << pos << ' ' << size
                 << ' ' << level << '\n';
        }

        file << "COLLISION_BOXES\n";
        file << Collision_boxes.size() << '\n';
        for (auto [p0, p1] : Collision_boxes) {
            file << p0 << ' ' << p1 << '\n';
        }

        file << "INTERESTING_DOTS\n";
        file << Interesting_dots.size() << '\n';
        for (auto [pos, name] : Interesting_dots) {
            file << pos << ' ' << name << '\n';
        }
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

            /*std::cout << "New wave!" << std::endl;

            for (auto [pos, name] : Interesting_dots) {
                if (name != "player") {
                    Bats.emplace_back(pos);
                    Bats.emplace_back(pos);
                    Slimes.emplace_back(pos);
                    Slimes.emplace_back(pos);
                }
            }*/

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
            slime.simulate(delta_time, Collision_boxes);
        }
        for (auto &bat : Bats) {
            bat.simulate(delta_time);
        }

        // simulate bullets
        {
            for (auto &bullet : Bullets) {
                bullet.simulate(delta_time);
            }

            // bullet hit enemies
            for (int i = 0; i < static_cast<int>(Bullets.size()); i++) {
                if (Bullets[i].simulate_attack(Slimes) ||
                    Bullets[i].simulate_attack(Bats)/* ||
                    Bullets[i].simulate_attack_on_player(Players)*/) {
                    Bullets.erase(Bullets.begin() + i);
                    i--;
                }
            }

            // bullet hit wall
            for (int i = 0; i < static_cast<int>(Bullets.size()); i++) {
                bool need_delete = false;
                for (auto &collision_box : Collision_boxes) {
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

        simulate_game_collisions(Collision_boxes);
    }

    void draw() {
        clear_screen(GREY);

        for (auto [pos, size, sprite, level] : Draw_objects) {
            if (level < 0) {
                draw_sprite(pos, size, sprite);
            }
            // Dot bottom_pos = pos - Dot(0, Sprites[sprite].height() * size);
            // if (level < 0) {
            //     draw_sprite(pos, size, sprite);
            // } else if (level == 0 && Players[0].pos.y < bottom_pos.y) {
            //     draw_sprite(pos, size, sprite);
            // }
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
            // Dot bottom_pos = pos - Dot(0, Sprites[sprite].height() * size);
            // if (level > 0) {
            //     draw_sprite(pos, size, sprite);
            // } else if (level == 0 && Players[0].pos.y >= bottom_pos.y) {
            //     draw_sprite(pos, size, sprite);
            // }
        }

        // draw UI
        /*{
            // draw hp
            for (int i = 0; i < Players[0].hp; i++) {
                draw_sprite(
                    Dot(-global_variables::arena_half_size.x,
                        global_variables::arena_half_size.y) *
                            0.95 +
                        global_variables::camera.pos + Dot(i, 0) * 7,
                    0.5, SP_HEART
                );
            }

            // draw coins
            for (int i = 0; i < Players[0].coins; i++) {
                draw_sprite(
                    Dot(-global_variables::arena_half_size.x,
                        global_variables::arena_half_size.y) *
                            0.8 +
                        global_variables::camera.pos +
                        Dot(-14 - i * 2 + Players[0].coins * 2, 0),
                    0.5, SP_COIN
                );
            }
        }*/
    }
};

#endif  // GAME_ROOM_HPP