#ifndef GAME_ENGINE_TIME
#define GAME_ENGINE_TIME

#include "utils.h"

// тики ведутся с 1-го января 1970 г. 00:00:00 Всемирного времени

#if defined(_WIN32)

#include <windows.h>

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

#elif defined(__linux__) || defined(__APPLE__)

#include <sys/time.h>

// вернет частоту обновления устройства
u64 get_performance_frequency() {
    return 1'000'000;  // колво микросекунд в секунде
}

// вернет текущий тик
u64 get_ticks() {
    timeval ticks;
    // вернет -1 в случае ошибки
    ASSERT(gettimeofdey(&ticks, NULL) == 0, "call to gettimeofday fails");
    return ticks.tv_usec;
}

#else

static_assert(false, "<time.h> not supported operating system");

#endif

const efloat performance_frequency =
    static_cast<efloat>(get_performance_frequency());

class Timer {
    u64 start_tick;

public:
    Timer() : start_tick(get_ticks()) {
    }

    // вернет время между начальным тиком и текущим
    [[nodiscard]] efloat time() const {
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
    return output << std::fixed << std::setprecision(4) << time.time() << "s";
}

#endif  // GAME_ENGINE_TIME