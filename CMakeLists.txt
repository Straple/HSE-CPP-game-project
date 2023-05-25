cmake_minimum_required(VERSION 3.23)
project(bulletproof)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512vl")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS date_time regex)

IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})

    add_executable(client Multiplayer/client_main.cpp
            utils.cpp

            Objects/Geometry/dot.cpp
            Objects/Geometry/circle.cpp
            Objects/Geometry/line.cpp

            Objects/Sprite/png.cpp

            Objects/Sprite/color.cpp

            Objects/Sprite/sprite_object.cpp

            Objects/render_state.cpp

            Objects/collision.cpp)

    target_link_libraries(client ${Boost_LIBRARIES})

    add_executable(server Multiplayer/server_main.cpp
            utils.cpp

            Objects/Geometry/dot.cpp
            Objects/Geometry/circle.cpp
            Objects/Geometry/line.cpp

            Objects/Sprite/png.cpp

            Objects/Sprite/color.cpp

            Objects/Sprite/sprite_object.cpp

            Objects/render_state.cpp

            Objects/collision.cpp)
    target_link_libraries(server ${Boost_LIBRARIES})
ENDIF()
