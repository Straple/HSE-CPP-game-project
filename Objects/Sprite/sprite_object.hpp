#ifndef GAME_ENGINE_SPRITE_OBJECT_HPP
#define GAME_ENGINE_SPRITE_OBJECT_HPP

#include "../matrix.hpp"
#include "color.hpp"
#include "png.h"

using Sprite = Matrix<Color>;

Sprite read_sprite_from_png(const std::string &path);

// надстройка над Sprite, позволяющая хранить покадровую анимацию
class Spritesheet {
    std::vector<Sprite> frames;

public:
    Spritesheet() = default;

    // распарсить спрайт на кадры
    Spritesheet(const Sprite &sprite, int frame_len_x);

    const Sprite &operator[](unsigned int i) const;
};

#endif  // GAME_ENGINE_SPRITE_OBJECT_HPP