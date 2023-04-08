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

struct Room {
    std::vector<drawing_objects> Draw_objects;
    std::vector<collision_box> Collision_boxes;

    void read_room(const std::string &filename) {
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
            file >> str >> str;  // COLLISION BOXES

            int count;
            file >> count;
            Collision_boxes.assign(count, {});
            for (auto &[p0, p1] : Collision_boxes) {
                file >> p0 >> p1;
            }
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
        for(auto [pos, size, sprite, level] : Draw_objects){
            if(level < 0){
                draw_sprite(pos, size, sprite);
            }
        }

        for (auto &player : Players) {
            player.draw();
        }

        for(auto [pos, size, sprite, level] : Draw_objects){
            if(level >= 0){
                draw_sprite(pos, size, sprite);
            }
        }
    }
};

#endif  // GAME_ROOM_HPP