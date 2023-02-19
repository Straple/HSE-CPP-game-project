cmake_minimum_required(VERSION 3.23)
project(bulletproof)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CXX_FLAGS "-O3")

add_executable(game
        Objects/Sprite/png.cpp
        win32_platform.cpp)
