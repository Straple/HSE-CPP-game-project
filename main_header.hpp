#ifndef GAME_ENGINE_MAIN_HEADER_HPP
#define GAME_ENGINE_MAIN_HEADER_HPP
#else
// я предполагаю, что это должно быть только в одном месте
// потому что <game.cpp> очень сложная структура с кучей глобальных переменных
static_assert(false);
#endif  // GAME_ENGINE_MAIN_HEADER_HPP
//-----------------------------------------------

// use LPCSTR or LPCWSTR
#define GAME_ENGINE_MY_LPCSTR LPCWSTR

#define GAME_ENGINE_DRAW_FPS_PLOT

/*
WARNINGS:
Dot::get_len() uses hypot which is very slow
*/

//-----------------------------------------------
#include <windows.h>
// <windows.h> defined min and max macros
// this is bad
#undef min
#undef max
//-----------------------------------------------
#include "render.hpp"
//-----------------------------------------------
#include "Game/game.cpp"  // game

// #include "Game/Level maker/level_maker.cpp" // level maker