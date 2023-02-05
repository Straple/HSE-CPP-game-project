#ifndef GAME_ENGINE_ASSERT
#define GAME_ENGINE_ASSERT

#include <iostream>

#ifdef GAME_ENGINE_DEBUG_MODE

#define ASSERT(condition, message)         \
    if (!(condition)) {                    \
        std::cerr << message << std::endl; \
        std::exit(0);                      \
    }

//_STL_VERIFY(condition, message) // don't work on CLion

#else

#define ASSERT(condition, message) condition

#endif  // GAME_ENGINE_DEBUG_MODE

#endif  // GAME_ENGINE_ASSERT