#ifndef GAME_ENGINE_TYPER_HPP
#define GAME_ENGINE_TYPER_HPP

#include "Geometry/geometry_utils.hpp"
#include "platform_common.hpp"
#include <string>
#include <vector>

struct Typer {
  std::string text = "";
  std::vector<efloat> button_hold_time;

  Typer();

  void clear();

  void simulate(const Input &input, efloat delta_time);
};

#endif // GAME_ENGINE_TYPER_HPP