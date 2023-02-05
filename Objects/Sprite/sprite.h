#ifndef GAME_ENGINE_SPRITE
#define GAME_ENGINE_SPRITE

#include "color.h"
#include "matrix.h"
#include "png.h"

struct sprite {
    Matrix<Color> picture;

    sprite() = default;

    // считывание спрайта из png файла
    explicit sprite(const std::string &path) {
        std::vector<unsigned char> img;
        unsigned int width, height;
        lodepng::decode(img, width, height, path, LCT_RGBA);

        picture = Matrix<Color>(height, width);

        for (u64 y = 0; y < height; y++) {
            for (u64 x = 0; x < width; x++) {
                picture[y][x] = Color(
                    img[y * width * 4 + x * 4 + 0],
                    img[y * width * 4 + x * 4 + 1],
                    img[y * width * 4 + x * 4 + 2],
                    img[y * width * 4 + x * 4 + 3]
                );
            }
        }
    }
};

#endif  // GAME_ENGINE_SPRITE