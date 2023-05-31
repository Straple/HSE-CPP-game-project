#ifndef GAME_ENGINE_MOVE_HPP
#define GAME_ENGINE_MOVE_HPP

#include "../utils.hpp"
#include "Geometry/dot.hpp"

/*
 * p = позиция
 *
 * dp = p' = инерция движения, которую мы накапливаем, чтобы например когда игрок идет вправо,
 * а потом резко поворачивает налево, то он еще некоторое время по инерции уходил вправо.
 * А также, чтобы когда он только начал идти, то сначала медленно, и с каждым разом чуть быстрее до некоторого значения.
 *
 * ddp = p'' = момент движения в кадре. Когда начинается кадр, то ddp = 0. Когда игрок нажимает кнопку идти вправо, то
 * ddp += C, а если влево то ddp -= C. Таким образом, если он нажал и вправо и влево, то он никуда в итоге не пойдет.
 *
 * После симуляции такого движения частичка ddp перейдет в dp, чтобы накопить инерцию движения
 *
 * delta_time = время между текущим кадром и предыдущим
 */

/*
 * симулирует движение по системе, описанной в начале файла
 */
void simulate_move(efloat &p, efloat &dp, efloat ddp, efloat delta_time) {
    ddp -= dp * 15;
    p = p + dp * delta_time + ddp * quare(delta_time) / 2;
    dp = dp + ddp * delta_time;
}

/*
 * аналогично simulate_move, только на плоскости
 */
void simulate_move2d(Dot &p, Dot &dp, Dot ddp, efloat delta_time) {
    simulate_move(p.x, dp.x, ddp.x, delta_time);
    simulate_move(p.y, dp.y, ddp.y, delta_time);
}

/*
 * Симулирует движение до точки p_to по системе из simulate_move
 * Если мы ее перешагнули или что-то сломалось, то мы окажемся в точке p_to.
 * Это нужно, чтобы когда мы были слишком близко к точке p_to, то не постоянно метались между ней, а сразу попали в неё
 */
void simulate_move_to(efloat &p, efloat p_to, efloat &dp, efloat ddp, efloat delta_time) {
    efloat result = p;

    simulate_move(result, dp, ddp, delta_time);

    auto is_spoiled = [](efloat num) -> bool { return std::isnan(num) || std::isinf(num); };

    if (is_spoiled(result) || is_spoiled(dp) || is_spoiled(ddp) || (p <= p_to && p_to <= result) ||
        (result <= p_to && p_to <= p)) {
        result = p_to;
        dp = 0;
    }

    p = result;
}

/*
 * аналогично simulate_move_to, только на плоскости
 */
void simulate_move_to2d(Dot &p, const Dot &p_to, Dot &dp, Dot ddp, efloat delta_time) {
    simulate_move_to(p.x, p_to.x, dp.x, ddp.x, delta_time);
    simulate_move_to(p.y, p_to.y, dp.y, ddp.y, delta_time);
}

//----------------------------------------------------------------------------------------------------------------------

enum class direction_t : u8 {
    NONE = 0,

    RIGHT = 1,
    UP = 2,
    LEFT = 3,
    DOWN = 4,

    COUNT = 5,
};

Dot get_direction(direction_t dir) {
    switch (dir) {
        case direction_t::NONE: {
            return {};
        }
        case direction_t::RIGHT: {
            return {1, 0};
        }
        case direction_t::UP: {
            return {0, 1};
        }
        case direction_t::LEFT: {
            return {-1, 0};
        }
        case direction_t::DOWN: {
            return {0, -1};
        }
        case direction_t::COUNT:
            break;
    }
    ASSERT(false, "incorrect direction type");
    return {0, 0};
}

//----------------------------------------------------------------------------------------------------------------------

Dot grid_start(-75, 35);
constexpr efloat step_size = 5;

#include <cmath>
#include <map>
#include <queue>
#include <set>
#include <utility>

using grid_pos_t = std::pair<int, int>;

Dot cast_grid_coord_to_game_coord(const grid_pos_t &grid_pos) {
    return grid_start + step_size * Dot(grid_pos.first, grid_pos.second);
}

std::vector<grid_pos_t> build_optional_grid_dots(Dot pos) {
    pos -= grid_start;
    pos /= step_size;

    int x1 = floor(pos.x);
    int x2 = ceil(pos.x);
    int y1 = floor(pos.y);
    int y2 = ceil(pos.y);

    if (x1 == x2) {
        if (y1 == y2) {
            return {{x1, y1}};
        } else {
            return {{x1, y1}, {x1, y2}};
        }
    } else if (y1 == y2) {
        return {{x1, y1}, {x2, y1}};
    } else {
        return {{x1, y1}, {x2, y1}, {x1, y2}, {x2, y2}};
    }
}

/*template <typename visitable_t>
grid_pos_t find_the_closest_to_grid(Dot pos, Dot to, visitable_t visitable) {
    pos -= grid_start;
    pos /= step_size;

    int x1 = std::floor(pos.x);
    int x2 = std::ceil(pos.x);
    int y1 = std::floor(pos.y);
    int y2 = std::ceil(pos.y);

    const std::vector<grid_pos_t> options = {{x1, y1}, {x2, y1}, {x1, y2}, {x2, y2}};

    efloat min_delta = INFINITY;
    grid_pos_t closest;
    for (auto &opt : options) {
        Dot opt_pos = cast_grid_coord_to_game_coord(opt);
        if (visitable(opt_pos) && (to - opt_pos).get_len() < min_delta) {
            min_delta = (to - opt_pos).get_len();
            closest = opt;
        }
    }
    return closest;
}*/

// ищет путь из from в to
// вернет true, если нашли такой путь, иначе false
// если есть путь, то result_dir будет равен необходимому направлению движения
//
// visitable(grid_pos_t) -> bool
// если вернет true, то эта точка грида доступна для посещения (например не мешают коллизии)
//
// suitable(Dot) -> bool
// если вернет true, то эта точка нам подходит и мы нашли путь

template <typename visitable_t, typename suitable_t>
bool get_direction_to_shortest_path_bfs(
    const Dot &from,
    [[maybe_unused]] const Dot &to,
    Dot &result_dir,
    visitable_t visitable,
    suitable_t suitable
    //',std::vector<Dot> &shortest_path,
    // std::vector<Dot> &grid
) {
    const static std::vector<grid_pos_t> steps = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    //{{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    // shortest_path.clear();
    // grid.clear();

    // стартовые точки грида
    std::vector<grid_pos_t> starting_points;
    {
        std::vector<grid_pos_t> optional = build_optional_grid_dots(from);
        for (auto opt_pos : optional) {
            if (visitable(opt_pos)) {
                starting_points.push_back(opt_pos);
            }
        }
    }

    std::vector<grid_pos_t> queue;
    std::map<grid_pos_t, efloat> dist;
    std::map<grid_pos_t, grid_pos_t> previous;

    for (auto &start_pos : starting_points) {
        queue.push_back(start_pos);
        dist[start_pos] = 0;
        previous[start_pos] = start_pos;
    }

    bool find_answer = false;
    grid_pos_t answer_pos;

    for(uint32_t top = 0; top < queue.size(); top++){
        auto grid_pos = queue[top];

        if (suitable(cast_grid_coord_to_game_coord(grid_pos))) {
            find_answer = true;
            answer_pos = grid_pos;
            break;
        }

        for (const auto &step : steps) {
            grid_pos_t grid_neighbour = {grid_pos.first + step.first, grid_pos.second + step.second};
            // мы еще не были в этой точке и можем ее посетить
            if (!dist.count(grid_neighbour) && visitable(grid_neighbour)) {
                // посещаем
                dist[grid_neighbour] = dist[grid_pos] + 1;
                previous[grid_neighbour] = grid_pos;
                queue.push_back(grid_neighbour);
            }
        }
    }
    if (find_answer) {
        // пока предыдущая точка не является стартовой
        while (find(starting_points.begin(), starting_points.end(), previous[answer_pos]) == starting_points.end()) {
            // shortest_path.push_back(cast_grid_coord_to_game_coord(answer_pos));
            answer_pos = previous[answer_pos];
        }

        // shortest_path.push_back(cast_grid_coord_to_game_coord(previous[answer_pos]));
        // reverse(shortest_path.begin(), shortest_path.end());

        result_dir = (cast_grid_coord_to_game_coord(answer_pos) - from).normalize();
    }
    // for (auto [grid_pos, d] : dist) {
    //     grid.push_back(cast_grid_coord_to_game_coord(grid_pos));
    // }
    return find_answer;
}

template <typename visitable_t, typename suitable_t>
bool get_direction_to_shortest_path_Astar(
    const Dot &from,
    const Dot &to,
    Dot &result_dir,
    visitable_t visitable,
    suitable_t suitable
    //,std::vector<Dot> &shortest_path,
    // std::vector<Dot> &grid
) {
    const static std::vector<grid_pos_t> steps = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    //{{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    // shortest_path.clear();
    // grid.clear();

    // стартовые точек грида
    std::vector<grid_pos_t> starting_points;
    {
        std::vector<grid_pos_t> optional = build_optional_grid_dots(from);
        for (auto opt_pos : optional) {
            if (visitable(opt_pos)) {
                starting_points.push_back(opt_pos);
            }
        }
    }

    std::priority_queue<std::pair<efloat, grid_pos_t>, std::vector<std::pair<efloat, grid_pos_t>>, std::greater<>>
        queue;
    std::map<grid_pos_t, efloat> dist;
    std::map<grid_pos_t, grid_pos_t> previous;

    for (auto &start_pos : starting_points) {
        queue.push({0, start_pos});
        dist[start_pos] = 0;
        previous[start_pos] = start_pos;
    }

    bool find_answer = false;
    grid_pos_t answer_pos;

    std::set<grid_pos_t> visited;

    while (!queue.empty()) {
        auto [heuristic, grid_pos] = queue.top();
        queue.pop();

        if (visited.count(grid_pos)) {
            ASSERT(false, to_string(visited.size()) + "ol");
            continue;
        }
        visited.insert(grid_pos);

        if (suitable(cast_grid_coord_to_game_coord(grid_pos))) {
            find_answer = true;
            answer_pos = grid_pos;
            break;
        }

        for (const auto &step : steps) {
            grid_pos_t grid_neighbour = {grid_pos.first + step.first, grid_pos.second + step.second};
            if (!dist.count(grid_neighbour) && visitable(grid_neighbour)) {
                // посещаем
                dist[grid_neighbour] = dist[grid_pos] + step_size;
                previous[grid_neighbour] = grid_pos;
                queue.push(
                    {dist[grid_neighbour] + (cast_grid_coord_to_game_coord(grid_neighbour) - to).get_len(),
                     grid_neighbour}
                );
            }
        }
    }
    if (find_answer) {
        // пока предыдущая точка не является стартовой
        while (find(starting_points.begin(), starting_points.end(), previous[answer_pos]) == starting_points.end()) {
            // shortest_path.push_back(cast_grid_coord_to_game_coord(answer_pos));
            answer_pos = previous[answer_pos];
        }

        // shortest_path.push_back(cast_grid_coord_to_game_coord(previous[answer_pos]));
        // reverse(shortest_path.begin(), shortest_path.end());

        result_dir = (cast_grid_coord_to_game_coord(answer_pos) - from).normalize();
    }
    // for (auto [grid_pos, d] : dist) {
    //     grid.push_back(cast_grid_coord_to_game_coord(grid_pos));
    // }
    return find_answer;
}

template <typename visitable_t, typename suitable_t>
bool get_direction_to_shortest_path(
    const Dot &from,
    const Dot &to,
    Dot &result_dir,
    visitable_t visitable,
    suitable_t suitable
    //,std::vector<Dot> &shortest_path,
    // std::vector<Dot> &grid
) {
    return get_direction_to_shortest_path_Astar(from, to, result_dir, visitable, suitable);
}

#endif  // GAME_ENGINE_MOVE_HPP