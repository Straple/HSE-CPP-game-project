#include "color.hpp"

Color::Color() {
    b = g = r = 0;
    a = 255;
}

Color::Color(u8 red, u8 green, u8 blue, u8 alpha)
    : r(red), g(green), b(blue), a(alpha) {
}

Color::Color(u8 red, u8 green, u8 blue)
    : r(red), g(green), b(blue), a(0xff) {
}

// 0xRRGGBB, alpha
Color::Color(unsigned int rgb, u8 alpha)
    : b(rgb & 0xff),
      g((rgb & 0xff00) >> 8),
      r((rgb & 0xff0000) >> 16),
      a(alpha) {
}

// 0xAARRGGBB
Color::Color(unsigned int argb)
    : b(argb & 0xff),
      g((argb & 0xff00) >> 8),
      r((argb & 0xff0000) >> 16),
      a((argb & 0xff000000) >> 24) {
}

// накладывает на этот цвет color
[[nodiscard]] Color Color::combine(const Color &color) const {
    Color result;

    result.r = (color.r * color.a + r * (255 - color.a)) >> 8;
    result.g = (color.g * color.a + g * (255 - color.a)) >> 8;
    result.b = (color.b * color.a + b * (255 - color.a)) >> 8;
    // result.a = color.a;
    return result;
}

Color::operator unsigned int() const {
    return (static_cast<unsigned int>(a) << 24) +
           (static_cast<unsigned int>(r) << 16) +
           (g << 8) +
           b;
}

bool operator==(const Color &lhs, const Color &rhs) {
    return lhs.b == rhs.b && lhs.g == rhs.g && lhs.r == rhs.r && lhs.a == rhs.a;
}

bool is_draw(const Color &color) {
    return color.a != 0;
}
