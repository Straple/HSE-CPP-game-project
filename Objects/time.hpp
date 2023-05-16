#ifndef GAME_ENGINE_TIME_HPP
#define GAME_ENGINE_TIME_HPP

#include "../utils.hpp"

// тики ведутся с 1-го января 1970 г. 00:00:00 Всемирного времени

#if defined(_WIN32)

#include <windows.h>
// windows.h defined min and max macros
// this is bad
#undef min
#undef max

// вернет частоту обновления устройства
u64 get_performance_frequency() {
    LARGE_INTEGER perf;
    ASSERT(
        QueryPerformanceFrequency(&perf),
        "call to QueryPerformanceFrequency fails"
    );
    return perf.QuadPart;
}

// вернет текущий тик
u64 get_ticks() {
    LARGE_INTEGER ticks;
    ASSERT(
        QueryPerformanceCounter(&ticks), "call to QueryPerformanceCounter fails"
    );
    return ticks.QuadPart;
}

#else

static_assert(false, "not supported operating system");

#endif

const efloat performance_frequency =
    static_cast<efloat>(get_performance_frequency());

class Timer {
    u64 start_tick;

public:
    Timer() : start_tick(get_ticks()) {
    }

    // вернет время между начальным тиком и текущим
    [[nodiscard]] efloat get() const {
        return static_cast<efloat>(get_ticks() - start_tick) /
               performance_frequency;
    }

    // обновит начальный тик
    void reset() {
        start_tick = get_ticks();
    }

    // вернет тик начала отсчета
    [[nodiscard]] u64 get_tick() const {
        return start_tick;
    }
};

std::ostream &operator<<(std::ostream &output, const Timer &time) {
    return output << std::fixed << std::setprecision(4) << time.get() << "s";
}

#endif  // GAME_ENGINE_TIME_HPP