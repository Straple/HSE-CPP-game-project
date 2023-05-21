cmake_minimum_required(VERSION 3.23)
project(bulletproof)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512vl")

add_executable(game
        # UTILS
        utils.cpp
        # GEOMETRY
        Objects/Geometry/dot.cpp
        Objects/Geometry/circle.cpp
        Objects/Geometry/line.cpp
        # LIBPNG LIBRARY
        Objects/Sprite/png.cpp
        # COLLISION
        Objects/collision.cpp
        # MAIN
        win32_platform.cpp)
