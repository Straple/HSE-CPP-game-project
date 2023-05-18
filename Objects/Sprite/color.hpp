#ifndef GAME_ENGINE_COLOR_HPP
#define GAME_ENGINE_COLOR_HPP

#include "../../utils.hpp"

// 0xAARRGGBB
// [a, r, g, b]
// [0..255]
struct Color {
    u8 b;  // blue
    u8 g;  // green
    u8 r;  // red
    u8 a;  // alpha

    Color() {
        b = g = r = 0;
        a = 255;
    }

    Color(u8 red, u8 green, u8 blue, u8 alpha)
        : r(red), g(green), b(blue), a(alpha) {
    }

    Color(u8 red, u8 green, u8 blue) : r(red), g(green), b(blue), a(0xff) {
    }

    // 0xRRGGBB, alpha
    Color(unsigned int rgb, u8 alpha)
        : b(rgb & 0xff),
          g((rgb & 0xff00) >> 8),
          r((rgb & 0xff0000) >> 16),
          a(alpha) {
    }

    // 0xAARRGGBB
    Color(unsigned int argb)
        : b(argb & 0xff),
          g((argb & 0xff00) >> 8),
          r((argb & 0xff0000) >> 16),
          a((argb & 0xff000000) >> 24) {
    }

    // накладывает на этот цвет color
    [[nodiscard]] Color combine(const Color &color) const {
        Color result;

        result.r = (color.r * color.a + r * (255 - color.a)) / 256;
        result.g = (color.g * color.a + g * (255 - color.a)) / 256;
        result.b = (color.b * color.a + b * (255 - color.a)) / 256;
        //result.a = color.a;
        return result;
    }

    explicit operator unsigned int() const {
        return (static_cast<unsigned int>(a) << 24) +
               (static_cast<unsigned int>(r) << 16) + (g << 8) + b;
    }
};

bool operator==(const Color &lhs, const Color &rhs) {
    return lhs.b == rhs.b && lhs.g == rhs.g && lhs.r == rhs.r && lhs.a == rhs.a;
}

inline bool is_draw(const Color &color) {
    return color.a != 0;
}

#define WHITE Color(0xffffffff)
#define BLACK Color(0xff000000)

#define RED Color(0xffff0000)
#define BLUE Color(0xff0000ff)
#define GREEN Color(0xff00ff00)

#define GREY Color(0xff4C4C4C)
#define YELLOW Color(0xffffff00)
#define PURPLE Color(0xffff00ff)

#endif  // GAME_ENGINE_COLOR_HPP
