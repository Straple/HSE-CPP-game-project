
void draw_rect(dot pos, dot half_size, const Color& color) {

	// high accuracy

	// change to pixels
	s32 x0 = static_cast<s32>(((pos.x - half_size.x) + arena_half_size.x) * scale_factor);
	s32 y0 = static_cast<s32>(((pos.y - half_size.y) + arena_half_size.y) * scale_factor);

	s32 x1 = static_cast<s32>(((pos.x + half_size.x) + arena_half_size.x) * scale_factor);
	s32 y1 = static_cast<s32>(((pos.y + half_size.y) + arena_half_size.y) * scale_factor);

	draw_rect_in_pixels(x0, y0, x1, y1, color);
}

void draw_rect2(dot top_left, dot bottom_right, const Color& color) {

	top_left += arena_half_size;
	bottom_right += arena_half_size;

	// масштабирование
	top_left *= scale_factor;
	bottom_right *= scale_factor;

	// change to pixels
	s32 x0 = static_cast<s32>(top_left.x);
	s32 y0 = static_cast<s32>(bottom_right.y);

	s32 x1 = static_cast<s32>(bottom_right.x);
	s32 y1 = static_cast<s32>(top_left.y);

	draw_rect_in_pixels(x0, y0, x1, y1, color);
}

void draw_circle(const Circle& circle, const Color& color) {

	s32 top_y = clamp<s32>(0, (circle.pos.y + arena_half_size.y - circle.radius) * scale_factor, render_state.height);
	s32 bottom_y = clamp<s32>(0, (circle.pos.y + arena_half_size.y + circle.radius) * scale_factor, render_state.height - 1);

	for (s32 y = top_y; y <= bottom_y; y++) {

		point_t h = std::min(circle.radius, abs(y / scale_factor - arena_half_size.y - circle.pos.y));

		point_t delta = sqrt(quare(circle.radius) - quare(h));

		s32 left_x = clamp<s32>(0, (circle.pos.x + arena_half_size.x - delta) * scale_factor, render_state.width);
		s32 right_x = clamp<s32>(-1, (circle.pos.x + arena_half_size.x + delta) * scale_factor, render_state.width - 1);

		draw_pixels(left_x, y, right_x + 1, y + 1, color);
	}
}

void draw_line(const Line& line, point_t half_size, const Color& color) {

	dot half_size_pt(half_size, half_size);

	if (abs(line.get_b()) >= 0.5) {
		for (u32 x = 0; x < render_state.width; x++) {
			point_t pos_x = x / scale_factor - arena_half_size.x;
			// a*x + b*y + c = 0
			// b*y = -(c + a*x)
			// y = -(c + a*x)/b
			point_t pos_y = -(line.get_c() + line.get_a() * pos_x) / line.get_b();
			draw_rect(dot(pos_x, pos_y), half_size_pt, color);
		}
	}
	else {
		for (u32 y = 0; y < render_state.height; y++) {
			point_t pos_y = y / scale_factor - arena_half_size.y;
			// a*x + b*y + c = 0
			// a*x = -(c + b*y)
			// x = -(c + b*y)/a
			point_t pos_x = -(line.get_c() + line.get_b() * pos_y) / line.get_a();
			draw_rect(dot(pos_x, pos_y), half_size_pt, color);
		}
	}
}

void draw_rhombus(dot pos, dot half_size, const Color& color) {

	s32 top_y = clamp<s32>(0, (pos.y + arena_half_size.y - half_size.y) * scale_factor, render_state.height);
	s32 bottom_y = clamp<s32>(0, (pos.y + arena_half_size.y + half_size.y) * scale_factor, render_state.height - 1);

	for (s32 y = top_y; y <= bottom_y; y++) {

		point_t cur_h = abs(y / scale_factor - arena_half_size.y - pos.y);
		point_t delta = (half_size.y - cur_h) * half_size.x / half_size.y;

		s32 left_x = clamp<s32>(0, (pos.x + arena_half_size.x - delta) * scale_factor, render_state.width);
		s32 right_x = clamp<s32>(-1, (pos.x + arena_half_size.x + delta) * scale_factor, render_state.width - 1);

		draw_pixels(left_x, y, right_x + 1, y + 1, color);
	}
}
