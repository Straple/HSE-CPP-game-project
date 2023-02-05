#pragma once

#include <windows.h>
#include "../utils.h"

struct Render_state {
    u32 height, width;

    Color* render_memory; // память для рендера
    u32 render_memory_len = 0;

    BITMAPINFO bitmap_info;

    // изменение размеров окна
    void resize(u32 w, u32 h) {
        width = w;
        height = h;

        // update render_memory
        {
            u64 size = static_cast<u64>(w) * h;

            if (render_memory_len < size) { // не хватает памяти
                delete[] render_memory;
                render_memory = new Color[size];
                render_memory_len = size;
            }
        }
        
        // update bitmap_info
        {
            auto& header = bitmap_info.bmiHeader;
            header.biSize = sizeof(header);
            header.biWidth = w;
            header.biHeight = h;
            header.biPlanes = 1;
            header.biBitCount = 32;
            header.biCompression = BI_RGB;
        }
    }

    Render_state& operator = (const Render_state& source) = delete;
    Render_state& operator = (Render_state&& source) = delete;
};
