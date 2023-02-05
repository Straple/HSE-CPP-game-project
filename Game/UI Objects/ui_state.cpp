
struct ui_state {
	
	Dot top_left, bottom_right;

	ui_state(Dot top_left, Dot bottom_right) {
		ASSERT(top_left.x <= bottom_right.x && bottom_right.y <= top_left.y, "bad points");
		this->top_left = top_left;
		this->bottom_right = bottom_right;
	}


	template<typename T>
	void draw(const T& value, const T& max, Color background, Color fill_color) {
		draw_rect2(top_left + Dot(-0.5, 0.5), bottom_right - Dot(-0.5, 0.5), background);

		efloat x = top_left.x + (efloat(value) / max) * (bottom_right.x - top_left.x);
		draw_rect2(top_left, Dot(x, bottom_right.y), fill_color);

		if (debug_mode) {
			draw_rect(top_left, Dot(0.5, 0.5), BLUE);
			draw_rect(bottom_right, Dot(0.5, 0.5), GREEN);
		}
	}
};
