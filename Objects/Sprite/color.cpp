#include "color.hpp"

Color::Color() {
    b = g = r = 0;
    a = 255;
}

Color::Color(u8 red, u8 green, u8 blue, u8 alpha)
    : r(red), g(green), b(blue), a(alpha) {
}

Color::Color(u8 red, u8 green, u8 blue) : r(red), g(green), b(blue), a(0xff) {
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

Color::operator uint32_t() const {
    return (static_cast<uint32_t>(a) << 24) + (static_cast<uint32_t>(r) << 16) +
           (g << 8) + b;
}

bool operator==(const Color &lhs, const Color &rhs) {
    return lhs.b == rhs.b && lhs.g == rhs.g && lhs.r == rhs.r && lhs.a == rhs.a;
}

bool is_draw(const Color &color) {
    return color.a != 0;
}

// output 0xAARRGGBB
std::ostream &operator<<(std::ostream &output, const Color &color) {
    uint32_t val = static_cast<uint32_t>(color);
    std::string str;
    auto to_16 = [](uint32_t x) {
        if (x < 10) {
            return '0' + x;
        } else if (x < 16) {
            return 'a' + x - 10;
        } else {
            ASSERT(false, "bad x");
        }
    };
    for (int symbol = 0; symbol < 8; symbol++) {
        str += to_16(val % 16);
        val /= 16;
    }
    str += "x0";
    std::reverse(str.begin(), str.end());
    return output << str;
}

std::istream &operator>>(std::istream &input, Color &color) {
    std::string str;
    input >> str;
    ASSERT(str.size() == 10 && str[0] == '0' && str[1] == 'x', "bad str");

    auto to_10 = [](char x) {
        if ('0' <= x && x <= '9') {
            return x - '0';
        } else if ('a' <= x && x <= 'f') {
            return x - 'a' + 10;
        } else {
            ASSERT(false, "bad symbol");
        }
    };

    str.erase(str.begin());  // remove '0'
    str.erase(str.begin());  // remove 'x'

    uint32_t val = 0;
    for (char c : str) {
        val *= 16;
        val += to_10(c);
    }
    color = static_cast<Color>(val);
    return input;
}