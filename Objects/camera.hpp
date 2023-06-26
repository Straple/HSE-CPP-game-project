#ifndef GAME_ENGINE_CAMERA_HPP
#define GAME_ENGINE_CAMERA_HPP

#include "Geometry/dot.hpp"

struct Camera {
  Dot pos;
  Dot dp;

  // симулирует движение камеры к точке to
  void simulate(const Dot &to, efloat delta_time);
};

#endif // GAME_ENGINE_CAMERA_HPP