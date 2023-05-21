cmake_minimum_required(VERSION 3.23)
project(bulletproof)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512vl")

add_executable(game

        utils.cpp
        
        Objects/Geometry/dot.cpp
        Objects/Geometry/circle.cpp
        Objects/Geometry/line.cpp

        Objects/Sprite/png.cpp

        Objects/Sprite/color.cpp

        Objects/Sprite/sprite_object.cpp

        Objects/render_state.cpp

        Objects/collision.cpp

        win32_platform.cpp)
