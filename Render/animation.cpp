
struct animation {
    efloat frame_duration{};  // продолжительность кадра
    efloat frame_time_accum = 0;  // время накопления продолжительности кадра

    spritesheet_t sprite_sheet = spritesheet_t::SS_COUNT;  // лист спрайтов
    u8 frame_begin{};  // начало кадра в листе спрайтов
    u8 frame_count{};  // количество кадров в анимации
    u8 frame_cur_count = 0;  // счетчик текущего кадра с 0

    animation() = default;

    animation(
        spritesheet_t sprite_sheet,
        u8 frame_begin,
        u8 frame_count,
        efloat frame_duration
    )
        : sprite_sheet(sprite_sheet),
          frame_begin(frame_begin),
          frame_count(frame_count),
          frame_duration(frame_duration) {
    }

    void reset() {
        frame_cur_count = 0;
        frame_time_accum = 0;
    }

    // вернет true, если прошел полный веток анимации
    bool frame_update(efloat delta_time) {
        frame_time_accum += delta_time;
        if (frame_time_accum > frame_duration) {
            frame_time_accum = 0;
            frame_cur_count++;
            if (frame_cur_count >= frame_count) {
                frame_cur_count = 0;
                return true;
            }
        }
        return false;
    }

    template <typename func_t = dummy_color_func>
    void draw(Dot pos, efloat size, func_t &&func = dummy_color_func()) const {
        draw_spritesheet(
            pos, size, sprite_sheet, frame_begin + frame_cur_count, func
        );
    }
};
