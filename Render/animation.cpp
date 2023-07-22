
#include "animation.hpp"

animation::animation(spritesheet_t sprite_sheet, uint8_t frame_begin, uint8_t frame_count, efloat frame_duration)
    : sprite_sheet(sprite_sheet), frame_begin(frame_begin), frame_count(frame_count), frame_duration(frame_duration) {
}

void animation::reset() {
    frame_cur_count = 0;
    frame_time_accum = 0;
}

// вернет true, если прошел полный веток анимации
bool animation::frame_update(efloat delta_time) {
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
