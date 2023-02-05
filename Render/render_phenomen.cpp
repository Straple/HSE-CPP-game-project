
void draw_rain(u32 count_water, u8 alpha) {
	clear_screen(Color(0, 0, 255, alpha));

	std::uniform_int_distribution<s32> random_range_x(0, render_state.width);
	std::uniform_int_distribution<s32> random_range_y(0, render_state.height);

	for (u32 i = 0; i < count_water; i++) {

		s32 x = random_range_x(rnd);
		s32 y = random_range_y(rnd);

		draw_rect_in_pixels(x, y, x + 2, y + 30, Color(0, 0, 255, 24));
	}
}

struct Fire_machine {

	dot pos;
	point_t size;

	// fire settings
	point_t cooldown_add;
	point_t mult_size_dec;
	point_t mult_color_dec;
	point_t time_color_dec;

	point_t cooldown_add_accum = 0;

	Fire_machine(){}
	Fire_machine(const dot& pos, point_t size, point_t cooldown_add, point_t mult_size_dec, point_t mult_color_dec, point_t time_color_dec) {
		this->pos = pos;
		this->size = size;
		this->cooldown_add = cooldown_add;
		this->mult_size_dec = mult_size_dec;
		this->mult_color_dec = mult_color_dec;
		this->time_color_dec = time_color_dec;
	}

	struct rhombus {
		dot pos;
		point_t dpy;
		point_t time;
		bool is_smoke;

		serialization_traits_byte(rhombus);
	};

	std::vector<rhombus> Rhombus;

	void serialize(std::ostream& os) const {
		byte_serialization(os, pos);
		byte_serialization(os, size);

		byte_serialization(os, cooldown_add);
		byte_serialization(os, mult_size_dec);
		byte_serialization(os, mult_color_dec);
		byte_serialization(os, time_color_dec);

		byte_serialization(os, cooldown_add_accum);

		serialization_traits<std::vector<rhombus>>::serialize(os, Rhombus);

	}
	void deserialize(std::istream& is) {
		pos = byte_deserialization<dot>(is);
		size = byte_deserialization<point_t>(is);

		cooldown_add = byte_deserialization<point_t>(is);
		mult_size_dec = byte_deserialization<point_t>(is);
		mult_color_dec = byte_deserialization<point_t>(is);
		time_color_dec = byte_deserialization<point_t>(is);

		cooldown_add_accum = byte_deserialization<point_t>(is);

		Rhombus = serialization_traits<std::vector<rhombus>>::deserialize(is);
	}


	point_t get_size(const rhombus& item) const {
		point_t res = size - item.time * mult_size_dec;
		if (item.is_smoke) {
			res *= 1.5;
		}
		return res;
	}

	s32 get_u8(const rhombus& item) const {
		return 0xff - item.time / time_color_dec * mult_color_dec;
	}
	Color get_color(const rhombus& item) const {

		if (item.is_smoke) {
			return Color(0xff - get_u8(item), 0xff - get_u8(item), 0xff - get_u8(item), 196);
		}
		else {
			return Color(0xff, get_u8(item), 0);
		}
	}

	void simulate(point_t delta_time) {

		cooldown_add_accum += delta_time;

		for (s32 i = 0; i < Rhombus.size(); i++) {
			simulate_move(Rhombus[i].pos.y, Rhombus[i].dpy, 50, delta_time);

			Rhombus[i].time += delta_time;

			if (get_size(Rhombus[i]) < 1e-2 || get_u8(Rhombus[i]) <= 0) {
				Rhombus.erase(Rhombus.begin() + i);
				i--;
			}
		}

		auto add = [&]() -> void {
			std::uniform_int_distribution<s64> rnd_range(-2e5, 2e5);
			dot p = dot(rnd_range(rnd) / 5e4 + pos.x, pos.y);
			Rhombus.push_back({ p, 0, 0, false });

			if (randomness(70)) {
				dot p = dot(rnd_range(rnd) / 5e4 + pos.x, pos.y);
				Rhombus.insert(Rhombus.begin(), { p, 0, 0, true });
			}
		};

		if (cooldown_add_accum >= cooldown_add) {
			cooldown_add_accum = 0;

			add();

			if (randomness(10)) {
				add();
			}
		}
	}

	void draw() const {
		for (auto& item : Rhombus) {
			dot p = item.pos;
			static_pos_update(p, !eng_state.camera_mode());
			draw_rhombus(p, dot(1, 1) * get_size(item), get_color(item));
		}
	}
};
