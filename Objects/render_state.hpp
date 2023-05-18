#ifndef GAME_ENGINE_RENDER_STATE_HPP
#define GAME_ENGINE_RENDER_STATE_HPP

#include <windows.h>
// windows.h defined min and max macros
// this is bad
#undef min
#undef max

#include "../utils.hpp"
#include "Sprite/color.hpp"

class Render_state {
    unsigned int m_height = 0, m_width = 0;

    Color *m_render_memory = nullptr;  // память для рендера

    unsigned int m_render_memory_len = 0;

    BITMAPINFO m_bitmap_info{};

public:
    Render_state() = default;
    Render_state(const Render_state &copy_object) = delete;
    Render_state(Render_state &&move_object) = delete;

    Render_state &operator=(const Render_state &source) = delete;
    Render_state &operator=(Render_state &&source) = delete;

    // изменение размеров окна
    void resize(unsigned int new_width, unsigned int new_height) {
        m_width = new_width;
        m_height = new_height;

        // update render_memory
        {
            u64 size = static_cast<u64>(m_width) * new_height;

            if (m_render_memory_len < size) {  // не хватает памяти
                delete[] m_render_memory;
                m_render_memory = new Color[size];
                m_render_memory_len = size;
            }
        }

        // update bitmap_info
        {
            auto &header = m_bitmap_info.bmiHeader;
            header.biSize = sizeof(header);
            header.biWidth = static_cast<LONG>(new_width);
            header.biHeight = static_cast<LONG>(new_height);
            header.biPlanes = 1;
            header.biBitCount = 32;
            header.biCompression = BI_RGB;
        }
    }

    // вернет указатель на начало строки памяти экрана
    [[nodiscard]] Color *operator[](unsigned int row) const {
        ASSERT(row < height(), "row is bad: " + to_string(row));
        return m_render_memory + row * m_width;
    }

    [[nodiscard]] unsigned int width() const {
        return m_width;
    }

    [[nodiscard]] unsigned int height() const {
        return m_height;
    }

    [[nodiscard]] Color *render_memory() const {
        return m_render_memory;
    }

    [[nodiscard]] BITMAPINFO &bitmap_info() {
        return m_bitmap_info;
    }
};

#endif  // GAME_ENGINE_RENDER_STATE_HPP