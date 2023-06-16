#include "mob.hpp"
//
#include "game_variables.hpp"

void Mob::update_move_dir(efloat delta_time, Dot player_pos, const std::set<grid_pos_t> &visitable_grid_dots) {
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

void Mob::simulate_move_to_player(Dot player_pos, const std::set<grid_pos_t> &visitable_grid_dots) {
    // чтобы моб был поверх игрока, а не под ним
    Dot to = player_pos - Dot(0, 0.1);

    if ((pos - to).get_len() < 20) {
        // мы очень близко. можем дойти и так
        move_dir_to_target = (to - pos).normalize();
    } else {
        std::vector<grid_pos_t> optional = build_optional_grid_dots(pos);
        bool have_visitable_dot_near = false;
        for (auto opt_pos : optional) {
            if (visitable_grid_dots.count(opt_pos)) {
                have_visitable_dot_near = true;
            }
        }

        if (!have_visitable_dot_near) {
            // мы черт знает где
            // телепортируем моба в ближайщую точку грида
            std::cout << "Teleported Mob to visitable grid" << std::endl;

            grid_pos_t cur_grid_pos = cast_game_coord_to_grid_coord(pos);

            grid_pos_t best_grid_pos;
            int best_dist = 1e9;

            for (auto &visitable_grid_pos : visitable_grid_dots) {
                int cur_dist = std::abs(cur_grid_pos.first - visitable_grid_pos.first) +
                               std::abs(cur_grid_pos.second - visitable_grid_pos.second);
                if (cur_dist < best_dist) {
                    best_dist = cur_dist;
                    best_grid_pos = visitable_grid_pos;
                }
            }

            ASSERT(best_dist != 1e9, "why is 1e9?");
            pos = cast_grid_coord_to_game_coord(best_grid_pos);
        } else if (!get_direction_to_shortest_path(
                       pos, to, move_dir_to_target,
                       [&](const grid_pos_t &request) { return visitable_grid_dots.count(request); },
                       [&](const Dot &request) { return (to - request).get_len() < 15; }
                   )) {
            // ASSERT(false, "oh ho, way not found");
        }
    }
}

void Mob::update_target(efloat delta_time) {
    target_change_accum -= delta_time;
    int index = find_player_index(target_client_id);
    if (
        // если у нас нет цели
        index == -1 ||
        // или цель кокнулась
        game_variables::Players[index].is_dead() ||
        // или мы уже долго гонялись за ней
        target_change_accum <= 0) {
        target_client_id = find_best_player(pos);
        target_change_accum = target_change_cooldown;
    }
}
