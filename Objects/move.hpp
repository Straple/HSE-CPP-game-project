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

    auto is_spoiled = [](efloat num) -> bool {
        return std::isnan(num) || std::isinf(num);
    };

    if (is_spoiled(result) || is_spoiled(dp) || is_spoiled(ddp) ||
        (p <= p_to && p_to <= result) || (result <= p_to && p_to <= p)) {
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

const static Dot grid_start(-75,35);
const static efloat step_size = 4;


#include <map>
#include <queue>
#include <utility>

// ищет путь из from в to
// вернет true, если нашли такой путь, иначе false
// если есть путь, то result_dir будет равен необходимому направлению движения
//
// visitable(Dot) -> bool
// если вернет true, то эта точка доступна для посещения (например не мешают коллизии)
//
// suitable(Dot) -> bool
// если вернет true, то эта точка нам подходит и мы нашли путь
template <typename visitable_t, typename suitable_t>
bool get_direction_to_shortest_path(Dot from, Dot to, Dot &result_dir, visitable_t visitable, suitable_t suitable) {

    const static std::vector<std::pair<int, int>> steps =
            //{{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
            {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

//    shortest_path.clear();
//    grid.clear();

    auto cast_grid_coord_to_game_coord = [&](const std::pair<int, int> &grid_pos) {
        return Dot(grid_start.x + grid_pos.first * step_size, grid_start.y + grid_pos.second * step_size);
    };

    auto get_the_closest_to_grid = [&](Dot pos, Dot to, visitable_t visitable) {
        pos.x -= grid_start.x;
        int x1 = floor(pos.x / step_size);
        int x2 = ceil(pos.x / step_size);
        pos.y -= grid_start.y;
        int y1 = floor(pos.y / step_size);
        int y2 = ceil(pos.y / step_size);
        const std::vector<std::pair<int,int>>options = {std::make_pair(x1,y1),std::make_pair(x2,y1),std::make_pair(x1,y2),std::make_pair(x2,y2)};
        efloat min_delta = INFINITY;
        std::pair<int,int>closest;
        for (auto& opt: options) {
            if (visitable(cast_grid_coord_to_game_coord(opt)) && (to - cast_grid_coord_to_game_coord(opt)).get_len() < min_delta) {
                min_delta = (to - cast_grid_coord_to_game_coord(opt)).get_len();
                closest = opt;
            }
        }
        return closest;
    };
    auto closest_start = get_the_closest_to_grid(from, to, visitable);
    Dot closest = cast_grid_coord_to_game_coord(closest_start);
    efloat delta_x = abs(from.x - closest.x);
    efloat delta_y = abs(from.y - closest.y);
    efloat eps = 1e-9;
    bool close_enough = false;
    if (delta_x < eps && delta_y < eps) {
        close_enough = true;
    }
    std::queue<std::pair<int, int>> queue;
    std::map<std::pair<int, int>, efloat> dist;
    std::map<std::pair<int, int>, std::pair<int, int>> previous;

    queue.push(closest_start);
    dist[closest_start] = 0;
    previous[closest_start] = closest_start;



    while (!queue.empty()) {
        auto grid_pos = queue.front();
        queue.pop();
        if (suitable(cast_grid_coord_to_game_coord(grid_pos))) {
            // ура! нашли подходящую точку
            while (previous[grid_pos] != closest_start) {
//                shortest_path.push_back(cast_grid_coord_to_game_coord(grid_pos));
                grid_pos = previous[grid_pos];
            }
//            shortest_path.push_back(cast_grid_coord_to_game_coord(closest_start));
//            if (!close_enough) {
//                shortest_path.push_back(from);
//            }
//            reverse(shortest_path.begin(), shortest_path.end());
            result_dir = (cast_grid_coord_to_game_coord(grid_pos) - from).normalize();

//            for (auto [grid_pos, d] : dist) {
//                grid.push_back(cast_grid_coord_to_game_coord(grid_pos));
//            }
            return true;
        }

        for (const auto &step : steps) {
            std::pair<int, int> grid_neighbour = {grid_pos.first + step.first, grid_pos.second + step.second};
            // мы еще не были в этой точке и можем ее посетить
            if (!dist.count(grid_neighbour) && visitable(cast_grid_coord_to_game_coord(grid_neighbour))) {
                // посещаем
                dist[grid_neighbour] = dist[grid_pos] + 1;
                previous[grid_neighbour] = grid_pos;
                queue.push(grid_neighbour);
            }
        }
    }
//    for (auto [grid_pos, d] : dist) {
//        grid.push_back(cast_grid_coord_to_game_coord(grid_pos));
//    }
    // не нашли пути
    return false;
}
template <typename visitable_t, typename suitable_t>
bool get_direction_to_shortest_path_Astar(Dot from, Dot to, Dot &result_dir, visitable_t visitable, suitable_t suitable) {

    const static std::vector<std::pair<int, int>> steps =
            //{{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
            {{1,  0},
             {-1, 0},
             {0,  1},
             {0,  -1}};

//    shortest_path.clear();
//    grid.clear();

    auto cast_grid_coord_to_game_coord = [&](const std::pair<int, int> &grid_pos) {
        return Dot(grid_start.x + grid_pos.first * step_size, grid_start.y + grid_pos.second * step_size);
    };

    auto get_the_closest_to_grid = [&](Dot pos, Dot to, visitable_t visitable) {
        pos.x -= grid_start.x;
        int x1 = floor(pos.x / step_size);
        int x2 = ceil(pos.x / step_size);
        pos.y -= grid_start.y;
        int y1 = floor(pos.y / step_size);
        int y2 = ceil(pos.y / step_size);
        const std::vector<std::pair<int, int>> options = {std::make_pair(x1, y1), std::make_pair(x2, y1),
                                                          std::make_pair(x1, y2), std::make_pair(x2, y2)};
        efloat min_delta = INFINITY;
        std::pair<int, int> closest;
        for (auto &opt: options) {
            if (visitable(cast_grid_coord_to_game_coord(opt)) &&
                (to - cast_grid_coord_to_game_coord(opt)).get_len() < min_delta) {
                min_delta = (to - cast_grid_coord_to_game_coord(opt)).get_len();
                closest = opt;
            }
        }
        return closest;
    };
    auto heuristic = [&](Dot from, Dot to) {
        return (from - to).get_len();
    };
    auto closest_start = get_the_closest_to_grid(from, to, visitable);
    Dot closest = cast_grid_coord_to_game_coord(closest_start);
    efloat delta_x = abs(from.x - closest.x);
    efloat delta_y = abs(from.y - closest.y);
    efloat eps = 1e-9;
    bool close_enough = false;
    if (delta_x < eps && delta_y < eps) {
        close_enough = true;
    }
    class Compare {
    public:
        bool
        operator()(const std::pair<efloat, std::pair<int, int>> &a, const std::pair<efloat, std::pair<int, int>> &b) {
            return (a.first > b.first);
        }
    };
    std::priority_queue<std::pair<efloat, std::pair<int, int>>, std::vector<std::pair<efloat, std::pair<int, int>>>, Compare> queue;
    std::map<std::pair<int, int>, efloat> dist;
    std::map<std::pair<int, int>, std::pair<int, int>> previous;

    dist[closest_start] = 0;
    queue.push({0, closest_start});
    previous[closest_start] = closest_start;


    while (!queue.empty()) {
        auto [heur, grid_pos] = queue.top();
        queue.pop();
        if (suitable(cast_grid_coord_to_game_coord(grid_pos))) {
            // ура! нашли подходящую точку
            while (previous[grid_pos] != closest_start) {
//                shortest_path.push_back(cast_grid_coord_to_game_coord(grid_pos));
                grid_pos = previous[grid_pos];
            }
//            shortest_path.push_back(cast_grid_coord_to_game_coord(closest_start));
//            if (!close_enough) {
//                shortest_path.push_back(from);
//            }
//            reverse(shortest_path.begin(), shortest_path.end());
            result_dir = (cast_grid_coord_to_game_coord(grid_pos) - from).normalize();

//            for (auto [grid_pos, d]: dist) {
//                grid.push_back(cast_grid_coord_to_game_coord(grid_pos));
//            }
            return true;
        }

        for (const auto &step: steps) {
            std::pair<int, int> grid_neighbour = {grid_pos.first + step.first, grid_pos.second + step.second};
            // мы еще не были в этой точке и можем ее посетить
            if (!dist.count(grid_neighbour) && visitable(cast_grid_coord_to_game_coord(grid_neighbour))) {
                // посещаем
                dist[grid_neighbour] = dist[grid_pos] + 1;
                previous[grid_neighbour] = grid_pos;
                queue.push({heuristic(cast_grid_coord_to_game_coord(grid_neighbour), to) + dist[grid_neighbour],
                            grid_neighbour});
            }
        }
    }
//    for (auto [grid_pos, d]: dist) {
//        grid.push_back(cast_grid_coord_to_game_coord(grid_pos));
//    }
    // не нашли пути
    return false;
}



#endif  // GAME_ENGINE_MOVE_HPP