
struct ui_state {
	
	dot top_left, bottom_right;

	ui_state(dot top_left, dot bottom_right) {
		ASSERT(top_left.x <= bottom_right.x && bottom_right.y <= top_left.y, "bad points");
		this->top_left = top_left;
		this->bottom_right = bottom_right;
	}


	template<typename T>
	void draw(const T& value, const T& max, Color background, Color fill_color) {
		draw_rect2(top_left + dot(-0.5, 0.5), bottom_right - dot(-0.5, 0.5), background);

		point_t x = top_left.x + (point_t(value) / max) * (bottom_right.x - top_left.x);
		draw_rect2(top_left, dot(x, bottom_right.y), fill_color);

		if (eng_state.debug_mode()) {
			draw_rect(top_left, dot(0.5, 0.5), BLUE);
			draw_rect(bottom_right, dot(0.5, 0.5), GREEN);
		}
	}
};
