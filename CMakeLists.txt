cmake_minimum_required(VERSION 3.23)

project(bulletproof)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512vl")

add_library(SOURCES OBJECT

utils.cpp
sprites.cpp

    # OBJECTS
    Objects/render_state.cpp
    Objects/collision.cpp
    Objects/time.cpp
    Objects/platform_common.cpp
    Objects/move.cpp
    Objects/camera.cpp
    Objects/typer.cpp
        # GEOMETRY
        Objects/Geometry/dot.cpp
        Objects/Geometry/circle.cpp
        Objects/Geometry/line.cpp
        # SPRITE
        Objects/Sprite/png.cpp
        Objects/Sprite/color.cpp
        Objects/Sprite/sprite_object.cpp
    # RENDER
    Render/render_utils.cpp
    Render/basic_render.cpp
    Render/render_geometry.cpp
    Render/render_text.cpp
    Render/render_sprite.cpp
    Render/animation.cpp
    Render/render_object.cpp
    # GAME OBJECTS
    Game/Game_objects/abstract_object.cpp
    Game/Game_objects/abstract_physical_object.cpp
    Game/Game_objects/bullet.cpp
    Game/Game_objects/bat.cpp
    Game/Game_objects/bomber.cpp
    Game/Game_objects/bush.cpp
    Game/Game_objects/button.cpp
    Game/Game_objects/cursor.cpp
    Game/Game_objects/effect.cpp
    Game/Game_objects/game_collision.cpp
    Game/Game_objects/heart_and_coin.cpp
    Game/Game_objects/log.cpp
    Game/Game_objects/loot.cpp
    Game/Game_objects/mob.cpp
    Game/Game_objects/player.cpp
    Game/Game_objects/room.cpp
    Game/Game_objects/slime.cpp
    Game/Game_objects/table.cpp
    Game/Game_objects/tree.cpp
    Game/Game_objects/weapon.cpp
    Game/Game_objects/pillar.cpp
    Game/Game_objects/nun_statue.cpp
    Game/Game_objects/knight.cpp
    Game/Game_objects/barrel.cpp

    Audio/audio.cpp
        )


add_executable(singleplayer singleplayer_main.cpp $<TARGET_OBJECTS:SOURCES>)
add_executable(levelmaker level_maker_main.cpp $<TARGET_OBJECTS:SOURCES>)

find_library(LIB_TO_INCLUDE audiere Audio)
find_path(LIB_INCLUDES audiere.h Audio)
include_directories(${LIB_INCLUDES})

target_link_libraries(singleplayer ${LIB_TO_INCLUDE})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(BOOST_ROOT "C:/Boost")
message("BOOST_ROOT is ${BOOST_ROOT}")

find_package(Boost REQUIRED)
#find_package(Boost COMPONENTS date_time regex)

IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})

    add_executable(client Multiplayer/client_main.cpp $<TARGET_OBJECTS:SOURCES>)
    target_link_libraries(client ${Boost_LIBRARIES})

    add_executable(server Multiplayer/server_main.cpp $<TARGET_OBJECTS:SOURCES>)
    target_link_libraries(server ${Boost_LIBRARIES})

    # add audiere
    target_link_libraries(client ${LIB_TO_INCLUDE})
    target_link_libraries(server ${LIB_TO_INCLUDE})
ENDIF ()
