#ifndef GAME_ROOM_HPP
#define GAME_ROOM_HPP

#include "../../Objects/platform_common.hpp"
#include "../../objects.hpp"
#include "../../render.hpp"
#include "../../sprites.hpp"

// #include "player.hpp"

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
    std::vector<drawing_objects> Draw_objects;
    std::vector<collision_box> Collision_boxes;
    std::vector<interesting_dot> Interesting_dots;

    std::vector<Slime> Slimes;

    std::vector<Bullet> Bullets;

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

    void simulate(efloat delta_time, const Input &input) {
        for (auto &player : Players) {
            for (auto collision_box : Collision_boxes) {
                player.pos = collision_box.bubble(player.get_collision());
            }
        }

        if (pressed(BUTTON_MOUSE_L)) {
            Bullets.emplace_back(
                // player.pos,
                Players[0].get_collision().circle.pos +
                    Dot(6, 2),  // центрированная позиция относительно спрайта
                cursor.pos + global_variables::camera.pos, 1000000000, 1000
            );
        }

        if (Slimes.empty()) {
            // new wave
            for (auto [pos, name] : Interesting_dots) {
                if (name == "enemy") {
                    Slimes.emplace_back(pos);
                    Slimes.emplace_back(pos);
                }
            }
        }

        // simulate slime
        {
            for (auto &slime : Slimes) {
                slime.simulate(delta_time);
            }

            // room collision bubbling slime
            for (auto &slime : Slimes) {
                for (auto collision_box : Collision_boxes) {
                    slime.pos = collision_box.bubble(slime.get_collision());
                }
            }

            // slime bubbling slime
            for (auto &slime1 : Slimes) {
                for (auto &slime2 : Slimes) {
                    if (&slime1 !=
                        &slime2) {  // чтобы не выталкивать самих себя
                        slime1.pos =
                            slime2.get_collision().bubble(slime1.get_collision()
                            );
                    }
                }
            }
        }

        // simulate bullets
        {
            for (auto &bullet : Bullets) {
                bullet.simulate(delta_time);
            }

            // simulate bullet hit enemies
            for (int i = 0; i < static_cast<int>(Bullets.size()); i++) {
                if (Bullets[i].simulate_attack(Players[0], Slimes)) {
                    Bullets.erase(Bullets.begin() + i);
                    i--;
                }
            }

            // simulate bullet hit wall
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

            for (auto &player : Players) {
                Objects.emplace_back(player);
            }
            for (auto &slime : Slimes) {
                Objects.emplace_back(slime);
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

        // draw effects
        for (auto &Effect : Effects) {
            Effect.draw();
        }
    }
};

#endif  // GAME_ROOM_HPP