#ifndef GAME_MOB_HPP
#define GAME_MOB_HPP

#include <set>
#include "abstract_physical_object.hpp"

struct Mob : AbstractPhysicalObject {
private:
    inline const static efloat target_change_cooldown = 5;
    efloat target_change_accum = 0;
    efloat update_move_dir_accum = 0;

public:
    inline const static efloat paralyzed_cooldown = 0.3;

    efloat paralyzed_accum = 0;

    Dot path_prefix[5];
    size_t path_size = 0;
    int target_client_id = -1;
    void update_move_dir(
        efloat delta_time,
        Dot player_pos,
        const std::set<grid_pos_t> &visitable_grid_dots
    );

    void simulate_move_to_player(
        Dot player_pos,
        const std::set<grid_pos_t> &visitable_grid_dots,
        efloat delta_time
    );

    void update_target(efloat delta_time);
};

#endif  // GAME_MOB_HPP