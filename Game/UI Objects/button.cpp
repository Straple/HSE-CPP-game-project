// текст с колизией
struct button { // кнопка
	text_t text;
	dot pos; // центр текста по x
	point_t size;
	collision_box coll;
	Color color, // usual color
		focus_color; // color when the button doesn't focus
	bool is_align, is_focus;

	button() {}

	button(text_t _text, dot _pos, point_t _size, Color _color, Color _focus_color, bool align = false) {
		is_align = align;
		is_focus = false;
		text = _text;
		pos.x = _pos.x + 0.5f * _size;
		pos.y = _pos.y - 0.5f * _size;
		size = _size;
		color = _color;
		focus_color = _focus_color;

		// !создать коллизию!
		{
			int len = text_len(_text);

			coll.p0 = dot(_pos.x + (align ? -(size * len) * 0.5 : 0), _pos.y);
			coll.p1.x = _pos.x + size * len - size + (align ? -(size * len) * 0.5 : 0);
			coll.p1.y = _pos.y - size * 6 - size;
		}
	}

	void draw() {
		if (is_align) {
			draw_text_align(text, pos, size, (is_focus ? focus_color : color));
		}
		else {
			draw_text(text, pos, size, (is_focus ? focus_color : color));
		}

		if (eng_state.debug_mode()) {
			draw_rect(coll.p0, dot(0.5, 0.5), 0xff00ff00);
			draw_rect(coll.p1, dot(0.5, 0.5), 0xff00ff00);
		}
	}

	// обновит состояние фокуса мыши
	void simulate(Mouse* mouse) {
		if (coll.trigger(mouse->pos)) {
			mouse->focus = true;
			is_focus = true;
		}
		else {
			is_focus = false;
		}
	}
};
