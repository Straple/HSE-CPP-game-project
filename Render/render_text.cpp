
#include "letters.cpp"

// рисует символ
void draw_symbol(char symbol, dot pos, point_t size, const Color& color) {

	if (symbol != ' ') {
		point_t original_x = pos.x;
		dot half_size = dot(1, 1) * (size * 0.5);

		if (symbol == 'q' || symbol == 'p' || symbol == 'y' || symbol == 'g') {
			pos.y -= 2 * size;
		}

		auto memory = get_symbol(symbol);

		for (u32 i = 0; i < 7; i++) {
			for (auto row = memory[i]; *row; row++) {
				if (*row == '0') {
					draw_rect(pos, half_size, color);
				}
				pos.x += size;
			}
			pos.y -= size;
			pos.x = original_x;
		}
	}
}

// рисует текст
void draw_text(text_t text, dot pos, point_t size, const Color& color) {

	point_t original_x = pos.x;

	while (*text) {
		if (*text == '\n') {
			pos.y -= size * 9;
			pos.x = original_x;
		}
		else {
			draw_symbol(*text, pos, size, color);
			pos.x += size * symbol_len(*text);
		}
		
		text++;
	}
}

// рисует выравненный текст по центру
void draw_text_align(text_t text, dot pos, point_t size, const Color& color) {

	pos.x -= text_len(text) * size * 0.5;
	draw_text(text, pos, size, color);
}

// рисует число
template<typename T>
void draw_object(const T& object, dot pos, point_t size, const Color& color) {

	draw_text(to_string(object).c_str(), pos, size, color);
}
