#pragma once

#include "../../utils.h"

// 0xAARRGGBB
// [a, r, g, b]
// [0..255]
struct Color {

    u8 b; // blue
    u8 g; // green
    u8 r; // red
    u8 a; // alpha

#define calc_color(alpha, c) ((static_cast<u32>(alpha) * c) / 255)


    Color() {
        r = g = b = a = 0;
    }
    Color(u8 red, u8 green, u8 blue, u8 alpha = 0xff) {
        r = calc_color(alpha, red);
        g = calc_color(alpha, green);
        b = calc_color(alpha, blue);
        a = alpha;
    }
    // 0xRRGGBB, alpha
    Color(u32 rgb, u8 alpha) {
        *reinterpret_cast<u32*>(this) = rgb;

        r = calc_color(alpha, r);
        g = calc_color(alpha, g);
        b = calc_color(alpha, b);
        a = alpha;
    }
    Color(u32 argb) {
        *reinterpret_cast<u32*>(this) = argb;

        r = calc_color(a, r);
        g = calc_color(a, g);
        b = calc_color(a, b);
    }

    // накладывает на этот цвет color
    Color combine(const Color& color) const {

        u8 inv_alpha = 0xff - color.a;

        Color result;
        result.a = 0xff - calc_color(inv_alpha, 0xff - a);
        result.r = color.r + calc_color(inv_alpha, r);
        result.g = color.g + calc_color(inv_alpha, g);
        result.b = color.b + calc_color(inv_alpha, b);

        return result;
    }

#undef calc_color

    operator u32() const {
        return static_cast<u32>(r << 16) + (g << 8) + b;
    }
};

bool is_draw(const Color& color) {
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
