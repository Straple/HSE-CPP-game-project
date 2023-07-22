#ifndef GAME_ENGINE_TYPER_HPP
#define GAME_ENGINE_TYPER_HPP

#include <string>
#include <vector>
#include "Geometry/geometry_utils.hpp"
#include "platform_common.hpp"

struct Typer {
    std::string text = "";
    std::vector<efloat> button_hold_time;

    Typer();

    void clear();

    void simulate(const Input &input, efloat delta_time);
};

#endif  // GAME_ENGINE_TYPER_HPP