

struct animation {
    spritesheet_t sprite_sheet;  // лист спрайтов

    u8 frame_begin;  // начало кадра в листе спрайтов
    u8 frame_size;   // количество кадров в анимации
    u8 frame_count;  // счетчик текущего кадра с 0

    efloat frame_duration;    // продолжительность кадра
    efloat frame_time_accum;  // время накопления продолжительности кадра

    animation() = default;

    animation(
        spritesheet_t sprite_sheet,
        u8 frame_begin,
        u8 frame_size,
        efloat frame_duration
    ) {
        this->sprite_sheet = sprite_sheet;
        this->frame_begin = frame_begin;
        this->frame_size = frame_size;
        this->frame_duration = frame_duration;
        frame_count = 0;
        frame_time_accum = 0;
    }

    void reset() {
        frame_count = 0;
        frame_time_accum = 0;
    }

    // вернет true, если прошел полный веток анимации
    bool frame_update(efloat delta_time) {
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

    template <typename func_t = Color(const Color &color)>
    void draw(Dot pos, efloat size, func_t &&func = standart_pixel_func)
        const {
        draw_spritesheet(
            pos, size, sprite_sheet, frame_begin + frame_count, func
        );
    }
};
