#ifndef GAME_ENGINE_COLOR
#define GAME_ENGINE_COLOR

#include "..\..\utils.h"

// 0xAARRGGBB
// [a, r, g, b]
// [0..255]
struct Color {
    u8 b;  // blue
    u8 g;  // green
    u8 r;  // red
    u8 a;  // alpha

#define calc_color(alpha, c) ((static_cast<unsigned int>(alpha) * (c)) / 255)

    Color() {
        b = g = r = a = 0;
    }

    Color(u8 red, u8 green, u8 blue, u8 alpha)
        : a(alpha),
          r(calc_color(alpha, red)),
          g(calc_color(alpha, green)),
          b(calc_color(alpha, blue)) {
    }

    Color(u8 red, u8 green, u8 blue) : a(0xff), r(red), g(green), b(blue) {
    }

    // 0xRRGGBB, alpha
    Color(unsigned int rgb, u8 alpha)
        : b(rgb & 0xff), g((rgb & 0xff00) >> 8), r((rgb & 0xff0000) >> 16) {
        r = calc_color(alpha, r);
        g = calc_color(alpha, g);
        b = calc_color(alpha, b);
        a = alpha;
    }

    // 0xAARRGGBB
    Color(unsigned int argb)
        : b(argb & 0xff),
          g((argb & 0xff00) >> 8),
          r((argb & 0xff0000) >> 16),
          a((argb & 0xff000000) >> 24) {
        r = calc_color(a, r);
        g = calc_color(a, g);
        b = calc_color(a, b);
    }

    // накладывает на этот цвет color
    [[nodiscard]] Color combine(const Color &color) const {
        Color result;
        u8 inv_alpha = 0xff - color.a;
        result.r = color.r + calc_color(inv_alpha, r);
        result.g = color.g + calc_color(inv_alpha, g);
        result.b = color.b + calc_color(inv_alpha, b);
        result.a = color.a + calc_color(inv_alpha, a);
        return result;
    }

#undef calc_color

    explicit operator unsigned int() const {
        return static_cast<unsigned int>(r << 16) + (g << 8) + b;
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

#define GREY Color(0xffC4C4C4)
#define YELLOW Color(0xffffff00)
#define PURPLE Color(0xffff00ff)

#endif  // GAME_ENGINE_COLOR