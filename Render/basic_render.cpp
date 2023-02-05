
// рисует в пикселях
void draw_pixels(u16 x0, u16 y0, u16 x1, u16 y1, const Color& color) {

	if (x0 < x1 && y0 < y1) {

		Color* row = render_state.render_memory + static_cast<u64>(y0) * render_state.width;

		if (color.a == 0xff) {

			for (u32 y = y0; y < y1; y++) {
				fill(reinterpret_cast<u32*>(row + x0), color, x1 - x0);

				row += render_state.width;
			}
		}
		else {
			for (u32 y = y0; y < y1; y++) {

				u32 x = x0;

				while (x < x1) {
					u32 k = x + 1;

					while (k < x1 && row[k] == row[x]) {
						k++;
					}

					fill(reinterpret_cast<u32*>(row + x), row[x].combine(color), k - x);
					x = k;
				}
				row += render_state.width;
			}
		}
	}
}

// рисует прямоугольник в пикселях с обработкой границ
void draw_rect_in_pixels(s32 x0, s32 y0, s32 x1, s32 y1, const Color& color) {
	x0 = clamp<s32>(0, x0, render_state.width);
	x1 = clamp<s32>(0, x1, render_state.width);

	y0 = clamp<s32>(0, y0, render_state.height);
	y1 = clamp<s32>(0, y1, render_state.height);

	draw_pixels(x0, y0, x1, y1, color);
}

// зарисовывает весь экран этим цветом
void clear_screen(const Color& color) {
	draw_pixels(0, 0, render_state.width, render_state.height, color);
}
