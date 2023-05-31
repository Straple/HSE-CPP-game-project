#ifndef GAME_ENEMY_STATES_HPP
#define GAME_ENEMY_STATES_HPP

#include "../../utils.hpp"
#include "player.hpp"

struct Mob : AbstractPhysicalObject {
    // cooldowns
    inline const static efloat target_change_cooldown = 5;
    inline const static efloat paralyzed_cooldown = 0.3;

    // accums
    efloat paralyzed_accum = 0;
    efloat target_change_accum = 0;
    efloat update_move_dir_accum = 0;

    Dot move_dir_to_target;

    int target_client_id = -1;

    void update_move_dir(efloat delta_time, Dot player_pos, const std::set<grid_pos_t> &visitable_grid_dots) {
        update_move_dir_accum -= delta_time;
        if (update_move_dir_accum <= 0) {
            if (randomness(50)) {
                update_move_dir_accum = 0.15;
            } else {
                update_move_dir_accum = 0.1;
            }
            simulate_move_to_player(player_pos, visitable_grid_dots);
        }
    }

    void simulate_move_to_player(Dot player_pos, const std::set<grid_pos_t> &visitable_grid_dots) {
        // чтобы моб был поверх игрока, а не под ним
        Dot to = player_pos - Dot(0, 0.1);

        if (!get_direction_to_shortest_path(
                pos, to, move_dir_to_target,
                [&](const grid_pos_t &request) { return visitable_grid_dots.count(request); },
                [&](const Dot &request) { return (to - request).get_len() < 10; }
            )) {
            // TODO: можно телепортировать его куда-то, в грид
            // ASSERT(false, "oh ho, way not found");
        }
    }

    void update_target(efloat delta_time) {
        target_change_accum -= delta_time;
        int index = find_player_index(target_client_id);
        if (
            // если у нас нет цели
            index == -1 ||
            // или цель кокнулась
            Players[index].is_dead() ||
            // или мы уже долго гонялись за ней
            target_change_accum <= 0) {
            target_client_id = find_best_player(pos);
            target_change_accum = target_change_cooldown;
        }
    }
};

#endif  // GAME_ENEMY_STATES_HPP