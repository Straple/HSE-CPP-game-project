#ifndef GAME_ENGINE_RENDER_STATE_HPP
#define GAME_ENGINE_RENDER_STATE_HPP

#include "../utils.hpp"
#include "Sprite/color.hpp"
//-------------------------------------
#include <windows.h>
// windows.h defined min and max macros
// this is bad
#undef min
#undef max

class RenderState {
    unsigned int m_height = 0, m_width = 0;

    Color *m_render_memory = nullptr;  // память для рендера

    unsigned int m_render_memory_len = 0;

    BITMAPINFO m_bitmap_info{};

public:
    RenderState() = default;
    RenderState(const RenderState &copy_object) = delete;
    RenderState(RenderState &&move_object) = delete;

    RenderState &operator=(const RenderState &source) = delete;
    RenderState &operator=(RenderState &&source) = delete;

    // изменение размеров окна
    void resize(unsigned int new_width, unsigned int new_height);

    Color *data();

    // вернет указатель на начало строки памяти экрана
    [[nodiscard]] Color *operator[](unsigned int row) const;

    [[nodiscard]] unsigned int width() const;

    [[nodiscard]] unsigned int height() const;

    [[nodiscard]] Color *render_memory() const;

    [[nodiscard]] BITMAPINFO &bitmap_info();
};

#endif  // GAME_ENGINE_RENDER_STATE_HPP