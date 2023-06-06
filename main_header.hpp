#ifndef GAME_ENGINE_MAIN_HEADER_HPP
#define GAME_ENGINE_MAIN_HEADER_HPP
#else
// я предполагаю, что это должно быть только в одном месте
// потому что <game.cpp> очень сложная структура с кучей глобальных переменных
static_assert(false);
#endif  // GAME_ENGINE_MAIN_HEADER_HPP
//-----------------------------------------------

// use LPCSTR or LPCWSTR
#define GAME_ENGINE_MY_LPCSTR LPCSTR

#define GAME_ENGINE_DRAW_FPS_PLOT

// if defined, then level maker, otherwise game mode
//#define LEVEL_MAKER_MODE

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

#ifdef LEVEL_MAKER_MODE
#include "Game/level_maker.cpp"
#else
#include "Game/game.cpp"
#endif