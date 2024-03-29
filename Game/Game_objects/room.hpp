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

enum object_type {
    OT_BUSH,
    OT_TREE,
    OT_TABLE,

    OT_PILLAR,
    OT_NUN_STATUE,
    OT_KNIGHT,
    OT_BARREL,

    OT_COUNT,
};

std::string object_type_to_string(object_type type);

object_type string_to_object_type(const std::string &str);

struct Room {
    // room sprites
    std::vector<drawing_objects> Draw_objects;

    // walls
    std::vector<CollisionBox> Walls;

    std::vector<interesting_dot> Interesting_dots;

    std::set<grid_pos_t> visitable_grid_dots_for_ground_mob;
    std::set<grid_pos_t> visitable_grid_dots_for_air_mob;

    // нужен, чтобы убрать косяки стен: мобы и игрок могут выпирать из стены
    // (left top, bottom right, Color)
    std::vector<std::tuple<Dot, Dot, Color>> ColorBoxes;

    Dot grid_start_dot;

    int wave_number = 0;
    static const inline efloat wave_cooldown = 5;
    efloat wave_cooldown_accum = 0;

    std::string room_name = "none";

    void build_grid();

    std::vector<std::pair<Dot, object_type>> read(const std::string &filename);

    void write(const std::string &filename, const std::vector<std::pair<Dot, object_type>> &Objects);

    void simulate(efloat delta_time);

    void draw();
};

#endif  // GAME_ROOM_HPP