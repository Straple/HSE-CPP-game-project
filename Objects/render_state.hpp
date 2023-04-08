#ifndef GAME_ENGINE_RENDER_STATE_HPP
#define GAME_ENGINE_RENDER_STATE_HPP

#include <windows.h>
// windows.h defined min and max macros
// this is bad
#undef min
#undef max

#include "../utils.hpp"
#include "Sprite/color.hpp"

const int chunk_len = 32;

struct render_chunk {
    bool state = true;
    Color data[chunk_len];

    // true => color of all chunk is data[0]
    // false => colors of chunk is data[0], data[1], data[2], ...

    void flush() {
        if (state) {
            state = false;
            for (int k = 1; k < chunk_len; k++) {
                data[k] = data[0];
            }
        }
    }
};

class Render_state {
    unsigned int m_height = 0, m_width = 0;

    Color *m_render_memory = nullptr;  // память для рендера

    render_chunk **m_data = nullptr;

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
            m_data = new render_chunk *[m_height];
            for (int i = 0; i < m_height; i++) {
                m_data[i] = new render_chunk[m_width / chunk_len + 1];
            }
            /*m_data.assign(
                m_height, std::vector<render_chunk>(m_width / chunk_len + 1)
            );*/

            u64 size = static_cast<u64>(new_width) * new_height;

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

    // chunks -> render_memory
    void flush() {
        for (int i = 0; i < height(); i++) {
            for (int j = 0; j < width() / chunk_len + 1; j++) {
                m_data[i][j].flush();
            }
        }

        int k = 0;
        for (int i = 0; i < height(); i++) {
            int id = 0;
            int chunk = 0;
            for (int j = 0; j < width(); j++) {
                render_memory()[k] = m_data[i][chunk].data[id];

                k++;
                id++;
                if (id == chunk_len) {
                    chunk++;
                    id = 0;
                }
            }
        }
    }

    /*[[nodiscard]] std::vector<render_chunk> &operator[](unsigned int row) {
        ASSERT(row < height(), "row is bad");
        return m_data[row];
    }*/

    // вернет указатель на начало строки памяти экрана
    /*[[nodiscard]] Color *operator[](unsigned int row) const {
        ASSERT(row < height(), "row is bad");
        return m_render_memory + row * m_width;
    }*/

    [[nodiscard]] unsigned int width() const {
        return m_width;
    }

    [[nodiscard]] unsigned int height() const {
        return m_height;
    }

    [[nodiscard]] render_chunk** &data() {
        return m_data;
    }

    [[nodiscard]] Color *render_memory() const {
        return m_render_memory;
    }

    [[nodiscard]] BITMAPINFO &bitmap_info() {
        return m_bitmap_info;
    }
};

#endif  // GAME_ENGINE_RENDER_STATE_HPP