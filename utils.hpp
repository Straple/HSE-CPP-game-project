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

std::mt19937 rnd(13);

using s8 = int8_t;
using u8 = uint8_t;
using s64 = int64_t;
using u64 = uint64_t;

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

std::string to_string(efloat val) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4) << val;
    std::string s = oss.str();
    if (s == "nan(ind)" || s == "-nan(ind)") {
        s = "nan";
    }
    return s;
}

bool randomness(unsigned int percent) {
    ASSERT(0 <= percent && percent <= 100, "wrong percent");

    std::uniform_int_distribution<unsigned int> rnd_range(0, 100);

    return rnd_range(rnd) <= percent;
}

double game_engine_time_for_calc = 0;

#define CALC_TIME_START Timer timer_for_calc_time;

#define CALC_TIME_END game_engine_time_for_calc += timer_for_calc_time.get();

#endif  // GAME_ENGINE_UTILS_HPP