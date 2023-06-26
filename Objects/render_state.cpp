#include "render_state.hpp"

// изменение размеров окна
void RenderState::resize(unsigned int new_width, unsigned int new_height) {
  m_width = new_width;
  m_height = new_height;

  // update render_memory
  {
    uint64_t size = static_cast<uint64_t>(m_width) * new_height;

    if (m_render_memory_len < size) { // не хватает памяти
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

Color *RenderState::data() { return m_render_memory; }

// вернет указатель на начало строки памяти экрана
[[nodiscard]] Color *RenderState::operator[](unsigned int row) const {
  ASSERT(row < height(), "row is bad: " + to_string(row));
  return m_render_memory + row * m_width;
}

[[nodiscard]] unsigned int RenderState::width() const { return m_width; }

[[nodiscard]] unsigned int RenderState::height() const { return m_height; }

[[nodiscard]] Color *RenderState::render_memory() const {
  return m_render_memory;
}

[[nodiscard]] BITMAPINFO &RenderState::bitmap_info() { return m_bitmap_info; }
