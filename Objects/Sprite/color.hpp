#ifndef GAME_ENGINE_COLOR_HPP
#define GAME_ENGINE_COLOR_HPP

#include "../../utils.hpp"

// 0xAARRGGBB
// [a, r, g, b]
// [0..255]
struct Color {
    using u8 = uint8_t;

    u8 b;  // blue
    u8 g;  // green
    u8 r;  // red
    u8 a;  // alpha

    Color();

    Color(u8 red, u8 green, u8 blue, u8 alpha);

    Color(u8 red, u8 green, u8 blue);

    // 0xRRGGBB, alpha
    Color(unsigned int rgb, u8 alpha);

    // 0xAARRGGBB
    explicit Color(unsigned int argb);

    // накладывает на этот цвет color
    [[nodiscard]] Color combine(const Color &color) const;

    explicit operator uint32_t() const;
};

bool operator==(const Color &lhs, const Color &rhs);

bool is_draw(const Color &color);

std::ostream& operator << (std::ostream& output, const Color& color);

std::istream& operator >> (std::istream& input, Color& color);

#define WHITE Color(0xffffffff)
#define BLACK Color(0xff000000)

#define RED Color(0xffff0000)
#define BLUE Color(0xff0000ff)
#define GREEN Color(0xff00ff00)

#define GREY Color(0xff4C4C4C)
#define YELLOW Color(0xffffff00)
#define PURPLE Color(0xffff00ff)

#endif  // GAME_ENGINE_COLOR_HPP
