#ifndef GAME_ROOM_HPP
#define GAME_ROOM_HPP

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

    void simulate() {
        for (auto &player : Players) {
            for (auto collision_box : Collision_boxes) {
                player.pos = collision_box.bubble(player.get_collision());
            }
        }
    }

    void draw() {
        clear_screen(GREY);
        for (auto [pos, size, sprite, level] : Draw_objects) {
            Dot bottom_pos = pos - Dot(0, Sprites[sprite].height() * size);
            if (level < 0) {
                draw_sprite(pos, size, sprite);
            } else if (level == 0 && Players[0].pos.y < bottom_pos.y) {
                draw_sprite(pos, size, sprite);
            }
        }

        for (auto &player : Players) {
            player.draw();
        }

        for (auto [pos, size, sprite, level] : Draw_objects) {
            Dot bottom_pos = pos - Dot(0, Sprites[sprite].height() * size);
            if (level > 0) {
                draw_sprite(pos, size, sprite);
            } else if (level == 0 && Players[0].pos.y >= bottom_pos.y) {
                draw_sprite(pos, size, sprite);
            }
        }
    }
};

#endif  // GAME_ROOM_HPP