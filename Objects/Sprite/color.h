#ifndef GAME_ENGINE_COLOR
#define GAME_ENGINE_COLOR

#include "utils.h"

// 0xAARRGGBB
// [a, r, g, b]
// [0..255]
struct Color {
    u8 b = 0;  // blue
    u8 g = 0;  // green
    u8 r = 0;  // red
    u8 a = 0;  // alpha

#define calc_color(alpha, c) ((static_cast<unsigned int>(alpha) * c) / 255)

    Color() = default;

    Color(u8 red, u8 green, u8 blue, u8 alpha = 0xff)
        : a(alpha),
          r(calc_color(alpha, red)),
          g(calc_color(alpha, green)),
          b(calc_color(alpha, blue)) {
    }

    // 0xRRGGBB, alpha
    Color(unsigned int rgb, u8 alpha) {
        *reinterpret_cast<unsigned int *>(this) = rgb;
        r = calc_color(alpha, r);
        g = calc_color(alpha, g);
        b = calc_color(alpha, b);
        a = alpha;
    }

    // 0xAARRGGBB
    Color(unsigned int argb) {
        *reinterpret_cast<unsigned int *>(this) = argb;
        r = calc_color(a, r);
        g = calc_color(a, g);
        b = calc_color(a, b);
    }

    // накладывает на этот цвет color
    [[nodiscard]] Color combine(const Color &color) const {
        u8 inv_alpha = 0xff - color.a;

        Color result;
        result.a = 0xff - calc_color(inv_alpha, 0xff - a);
        result.r = color.r + calc_color(inv_alpha, r);
        result.g = color.g + calc_color(inv_alpha, g);
        result.b = color.b + calc_color(inv_alpha, b);

        return result;
    }

#undef calc_color

    explicit operator unsigned int() const {
        return static_cast<unsigned int>(r << 16) + (g << 8) + b;
    }
};

bool operator==(const Color &lhs, const Color &rhs) {
    // very fast
    return lhs.b == rhs.b && lhs.g == rhs.g && lhs.r == rhs.r && lhs.a == rhs.a;
}

bool is_draw(const Color &color) {
    return color.a != 0;
}

#define WHITE Color(0xffffffff)
#define BLACK Color(0xff000000)

#define RED Color(0xffff0000)
#define BLUE Color(0xff0000ff)
#define GREEN Color(0xff00ff00)

#define GREY Color(0xffC4C4C4)
#define YELLOW Color(0xffffff00)
#define PURPLE Color(0xffff00ff)

#endif  // GAME_ENGINE_COLOR
