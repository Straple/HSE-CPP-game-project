#include "move.hpp"

void simulate_move(efloat &p, efloat &dp, efloat ddp, efloat delta_time) {
  ddp -= dp * 15;
  p = p + dp * delta_time + ddp * quare(delta_time) / 2;
  dp = dp + ddp * delta_time;
}

void simulate_move2d(Dot &p, Dot &dp, Dot ddp, efloat delta_time) {
  simulate_move(p.x, dp.x, ddp.x, delta_time);
  simulate_move(p.y, dp.y, ddp.y, delta_time);
}

void simulate_move_to(efloat &p, efloat p_to, efloat &dp, efloat ddp,
                      efloat delta_time) {
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

void simulate_move_to2d(Dot &p, const Dot &p_to, Dot &dp, Dot ddp,
                        efloat delta_time) {
  simulate_move_to(p.x, p_to.x, dp.x, ddp.x, delta_time);
  simulate_move_to(p.y, p_to.y, dp.y, ddp.y, delta_time);
}

//----------------------------------------------------------------------------------------------------------------------

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

#include <cmath>
#include <map>
#include <queue>
#include <set>
#include <utility>

Dot &get_ref_grid_start() {
  static Dot grid_start(-75, 35);
  return grid_start;
}

Dot cast_grid_coord_to_game_coord(const grid_pos_t &grid_pos) {
  return get_ref_grid_start() +
         step_size * Dot(grid_pos.first, grid_pos.second);
}

grid_pos_t cast_game_coord_to_grid_coord(Dot pos) {
  pos -= get_ref_grid_start();
  pos /= step_size;

  return std::make_pair<int, int>(std::round(pos.x), std::round(pos.y));
}

std::vector<grid_pos_t> build_optional_grid_dots(Dot pos) {
  pos -= get_ref_grid_start();
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
