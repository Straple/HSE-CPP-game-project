
void draw_rain(unsigned int count_water, u8 alpha) {
    clear_screen(Color(0, 0, 255, alpha));

    std::uniform_int_distribution<int> random_range_x(0, global_variables::render_state.width());
    std::uniform_int_distribution<int> random_range_y(0, global_variables::render_state.height());

    for (unsigned int i = 0; i < count_water; i++) {
        int x = random_range_x(rnd);
        int y = random_range_y(rnd);

        draw_rect_in_pixels(x, y, x + 2, y + 30, Color(0, 0, 255, 24));
    }
}

struct Fire_machine {
    Dot pos;
    efloat size;

    // fire settings
    efloat cooldown_add;
    efloat mult_size_dec;
    efloat mult_color_dec;
    efloat time_color_dec;

    efloat cooldown_add_accum = 0;

    Fire_machine() {
    }

    Fire_machine(
        const Dot &pos,
        efloat size,
        efloat cooldown_add,
        efloat mult_size_dec,
        efloat mult_color_dec,
        efloat time_color_dec
    ) {
        this->pos = pos;
        this->size = size;
        this->cooldown_add = cooldown_add;
        this->mult_size_dec = mult_size_dec;
        this->mult_color_dec = mult_color_dec;
        this->time_color_dec = time_color_dec;
    }

    struct rhombus {
        Dot pos;
        efloat dpy;
        efloat time;
        bool is_smoke;
    };

    std::vector<rhombus> Rhombus;

    efloat get_size(const rhombus &item) const {
        efloat res = size - item.time * mult_size_dec;
        if (item.is_smoke) {
            res *= 1.5;
        }
        return res;
    }

    int get_u8(const rhombus &item) const {
        return 0xff - item.time / time_color_dec * mult_color_dec;
    }

    Color get_color(const rhombus &item) const {
        if (item.is_smoke) {
            return Color(
                0xff - get_u8(item), 0xff - get_u8(item), 0xff - get_u8(item),
                196
            );
        } else {
            return Color(0xff, get_u8(item), 0);
        }
    }

    void simulate(efloat delta_time) {
        cooldown_add_accum += delta_time;

        for (int i = 0; i < Rhombus.size(); i++) {
            simulate_move(Rhombus[i].pos.y, Rhombus[i].dpy, 50, delta_time);

            Rhombus[i].time += delta_time;

            if (get_size(Rhombus[i]) < 1e-2 || get_u8(Rhombus[i]) <= 0) {
                Rhombus.erase(Rhombus.begin() + i);
                i--;
            }
        }

        auto add = [&]() -> void {
            std::uniform_int_distribution<s64> rnd_range(-2e5, 2e5);
            Dot p = Dot(rnd_range(rnd) / 5e4 + pos.x, pos.y);
            Rhombus.push_back({p, 0, 0, false});

            if (randomness(70)) {
                Dot p = Dot(rnd_range(rnd) / 5e4 + pos.x, pos.y);
                Rhombus.insert(Rhombus.begin(), {p, 0, 0, true});
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
        for (auto &item : Rhombus) {
            Dot p = item.pos;
            static_pos_update(p, global_variables::camera_is_static);
            draw_rhombus(p, Dot(1, 1) * get_size(item), get_color(item));
        }
    }
};
