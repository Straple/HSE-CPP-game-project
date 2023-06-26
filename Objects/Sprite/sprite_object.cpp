#include "sprite_object.hpp"

Sprite read_sprite_from_png(const std::string &path) {
    std::vector<unsigned char> img;
    unsigned int width, height;
    unsigned int return_code =
        lodepng::decode(img, width, height, path, LCT_RGBA);

    ASSERT(
        return_code == 0, "failed read_sprite_from_png\npath: " + path +
                              "\nerror_code: " + lodepng_error_text(return_code)
    );

    Sprite pixels(height, width);

    for (uint64_t y = 0; y < height; y++) {
        for (uint64_t x = 0; x < width; x++) {
            pixels[y][x] = Color(
                img[y * width * 4 + x * 4 + 0], img[y * width * 4 + x * 4 + 1],
                img[y * width * 4 + x * 4 + 2], img[y * width * 4 + x * 4 + 3]
            );
        }
    }
    return pixels;
}

// распарсить спрайт на кадры
Spritesheet::Spritesheet(const Sprite &sprite, int frame_len_x) {
    ASSERT(
        frame_len_x > 0 && frame_len_x <= sprite.width() &&
            sprite.width() % frame_len_x == 0,
        "bad frame_len_x"
    );

    for (int j = 0; j < sprite.width(); j += frame_len_x) {
        frames.emplace_back(sprite.height(), frame_len_x);
        for (int i = 0; i < sprite.height(); i++) {
            for (int k = j; k < j + frame_len_x; k++) {
                frames.back()[i][k - j] = sprite[i][k];
            }
        }
    }
}

const Sprite &Spritesheet::operator[](unsigned int i) const {
    ASSERT(i < frames.size(), "out of frames");
    return frames[i];
}
