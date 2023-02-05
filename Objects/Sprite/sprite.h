#pragma once

#include "color.h"
#include "../matrix.h"
#include "png.h"

struct sprite {

    matrix<Color> picture;

    sprite() {}

    // считывание спрайта из png файла
    sprite(std::string path) {

        if (path.substr(path.size() - 3) == "png") {
            std::vector<unsigned char> img;
            u32 width, height;
            lodepng::decode(img, width, height, path, LCT_RGBA);

            picture = matrix<Color>(height, width);

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
        else {

            std::ifstream input(path);
            u32 height, width;
            input >> height >> width;
            picture = matrix<Color>(height, width);

            for (u32 i = 0; i < height; i++) {
                for (u32 j = 0; j < width; j++) {
                    s64 pixel;
                    input >> pixel;
                    if (pixel == -1) {
                        picture[i][j].a = 0;
                    }
                    else {
                        picture[i][j] = Color(pixel, 0xff);
                    }
                }
            }
        }
    }
};
