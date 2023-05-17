#ifndef GAME_ENGINE_RENDER_HPP
#define GAME_ENGINE_RENDER_HPP

// 0 = без многопоточности
// 1 = многопоточность
#define MULTITHREAD_RENDER 0

#define GAME_ENGINE_USE_SSE

#include "objects.hpp"
//
#include "global_variables.hpp"
//
#include "sprites.hpp"
//
#include "utils.hpp"
//
#include "Render/basic_render.cpp"
//
//#include "Render/multi_threaded_render.cpp"
//
#include "Render/render_geometry.cpp"
//
#include "Render/render_text.cpp"
//
#include "Render/render_sprite.cpp"
//
#include "Render/animation.cpp"
//
#include "Render/render_phenomen.cpp"

#endif  // GAME_ENGINE_RENDER_HPP