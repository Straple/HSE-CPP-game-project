cmake_minimum_required(VERSION 3.23)
project(game-engine)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CXX_FLAGS "-O3")

add_executable(game

        assert.h
        Objects/Geometry/geometry_utils.h
        utils.h

        Objects/Geometry/dot.h
        Objects/Geometry/line.h
        Objects/Geometry/circle.h
        Objects/Geometry/geometry.h

        Objects/matrix.h

        Objects/Sprite/color.h
        Objects/Sprite/sprite.h
        Objects/Sprite/png.cpp

        Objects/move.h
        Objects/time.h
        Objects/render_state.h
        Objects/platform_common.h
        Objects/camera.h
        Objects/collision.h
        Objects/objects.h

        win32_platform.cpp

        )
