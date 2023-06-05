#ifndef GAME_ROOM_HPP
#define GAME_ROOM_HPP

#include "../../objects.hpp"
#include "../../sprites.hpp"

struct drawing_objects {
    Dot pos;
    efloat size;
    sprite_t sprite;
    int level;

    bool operator<(const drawing_objects &right) const;
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

    void build_grid();

    void read(const std::string &filename);

    void write(const std::string &filename);

    void simulate(efloat delta_time);

    void draw();
};

#endif  // GAME_ROOM_HPP