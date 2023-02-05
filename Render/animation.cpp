

struct animation {
	
	sprite_t sprite_sheet; // лист спрайтов

	u8 frame_begin; // начало кадра в листе спрайтов
	u8 frame_size; // количество кадров в анимации
	u8 frame_count; // счетчик текущего кадра с 0
	u8 len_x; // длина спрайта одного кадра по x

	point_t frame_duration; // продолжительность кадра
	point_t frame_time_accum; // время накопления до продолжительности кадра

	animation() {}

	animation(sprite_t sprite_sheet, u8 frame_begin, u8 frame_size, point_t frame_duration, u8 len_x) {
		this->sprite_sheet = sprite_sheet;
		this->frame_begin = frame_begin;
		this->frame_size = frame_size;
		this->frame_duration = frame_duration;
		this->len_x = len_x;
		frame_count = 0;
		frame_time_accum = 0;
	}

	void reset() {
		frame_count = 0;
		frame_time_accum = 0;
	}

	// вернет true, если прошел полный веток анимации
	bool frame_update(point_t delta_time) {
		frame_time_accum += delta_time;
		if (frame_time_accum > frame_duration) {
			frame_time_accum = 0;
			frame_count++;
			if (frame_count >= frame_size) {
				frame_count = 0;
				return true;
			}
		}
		return false;
	}

	template<typename func_t = Color(const Color& color)>
	void draw(dot pos, point_t size, func_t&& func = standart_pixel_func) const {

		draw_spritesheet(pos, size, sprite_sheet, len_x, frame_begin + frame_count, func);
	}
};
