#ifndef GAME_ENGINE_UTILS_HPP
#define GAME_ENGINE_UTILS_HPP

#include "Objects/Geometry/geometry_utils.hpp"
#include "assert.hpp"

// streams
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

// structures
#include <string>
#include <vector>

// algo
#include <algorithm>

// random
#include <random>

std::mt19937 &get_random_engine();

template <typename T>
T quare(const T &val) {
    return val * val;
}

template <typename T>
T clamp(const T &min, const T &val, const T &max) {
    ASSERT(min <= max, "bad border");

    if (val < min) {
        return min;
    } else if (val > max) {
        return max;
    } else {
        return val;
    }
}

template <typename T>
bool is_between(const T &min, const T &val, const T &max) {
    ASSERT(min <= max, "bad border");
    return min <= val && val <= max;
}

// object -> string
template <typename T>
std::string to_string(const T &val) {
    std::ostringstream oss;
    oss << val;
    std::string s = oss.str();
    return s;
}

// efloat -> string
std::string to_string(efloat val);

bool randomness(unsigned int percent);

#endif  // GAME_ENGINE_UTILS_HPP