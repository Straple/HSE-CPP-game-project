#ifndef GAME_ENGINE_ASSERT_HPP
#define GAME_ENGINE_ASSERT_HPP

#include <iostream>

#define GAME_ENGINE_DEBUG_MODE  // use debug mode

#ifdef GAME_ENGINE_DEBUG_MODE

#define FAILED_ASSERT(message)                                                \
    {                                                                         \
        std::cerr << "assert failed at " __FILE__ << ":" << __LINE__ << '\n'; \
        std::cerr << "message: \"" << (message) << "\"\n";                    \
        std::exit(0);                                                         \
    }

#define ASSERT(condition, message) \
    if (!(condition))              \
    FAILED_ASSERT(message)

//_STL_VERIFY(condition, message) // don't work on CLion

#else

#define ASSERT(condition, message) condition

#endif  // GAME_ENGINE_DEBUG_MODE

#endif  // GAME_ENGINE_ASSERT_HPP