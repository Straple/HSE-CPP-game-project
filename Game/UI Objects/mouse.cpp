
struct Mouse {

	sprite_t sprite;
	sprite_t focus_sprite;

	point_t size;

	dot pos;
	dot focus_pos; // позиция точки, когда мы нажали на мышку и удерживали

	bool focus = false;
	bool is_down = false;

	Mouse(sprite_t sprite, sprite_t focus_sprite, point_t size) {
		this->sprite = sprite;
		this->focus_sprite = focus_sprite;
		this->size = size;
	}

	void simulate(const Input& input) {
		if (pressed(BUTTON_MOUSE_L)) {
			focus_pos = pos;
		}
		is_down = is_down(BUTTON_MOUSE_L);
		focus = false;
	}

	void draw() {
		if (is_down) {
			dot pos0(std::min(focus_pos.x, pos.x), std::max(focus_pos.y, pos.y));
			dot pos1(std::max(focus_pos.x, pos.x), std::min(focus_pos.y, pos.y));
			draw_rect2(pos0, pos1, Color(0xffffff, 64));

			/*draw_object(Line(focus_pos, pos).get_a(), dot(), 0.3, 0xffffffff);
			draw_object(Line(focus_pos, pos).get_b(), dot(0, 10), 0.3, 0xffffffff);
			draw_object(Line(focus_pos, pos).get_c(), dot(0, 20), 0.3, 0xffffffff);

			draw_line(Line(focus_pos, pos), 0.3, 0xffffffff);

			draw_circle(Circle(pos, 20), Color(0xff0000, 60));*/
		}

		if (focus) {
			draw_sprite_static(pos - dot(16, 0) * size, size, focus_sprite);
		}
		else {
			draw_sprite_static(pos, size, sprite);
		}
	}
};
