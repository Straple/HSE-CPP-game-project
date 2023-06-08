

#include "render_geometry.hpp"
#include "../global_variables.hpp"
#include "basic_render.hpp"

void draw_rect(Dot pos, Dot half_size, const Color &color) {
    // high accuracy

    // change to pixels
    int x0 = static_cast<int>(
        ((pos.x - half_size.x) + global_variables::arena_half_size.x) * global_variables::scale_factor
    );
    int y0 = static_cast<int>(
        ((pos.y - half_size.y) + global_variables::arena_half_size.y) * global_variables::scale_factor
    );

    int x1 = static_cast<int>(
        ((pos.x + half_size.x) + global_variables::arena_half_size.x) * global_variables::scale_factor
    );
    int y1 = static_cast<int>(
        ((pos.y + half_size.y) + global_variables::arena_half_size.y) * global_variables::scale_factor
    );

    draw_rect_in_pixels(x0, y0, x1, y1, color);
}

void draw_rect2(Dot top_left, Dot bottom_right, const Color &color) {
    top_left += global_variables::arena_half_size;
    bottom_right += global_variables::arena_half_size;

    // масштабирование
    top_left *= global_variables::scale_factor;
    bottom_right *= global_variables::scale_factor;

    // change to pixels
    int x0 = static_cast<int>(top_left.x);
    int y0 = static_cast<int>(bottom_right.y);

    int x1 = static_cast<int>(bottom_right.x);
    int y1 = static_cast<int>(top_left.y);

    draw_rect_in_pixels(x0, y0, x1, y1, color);
}

void draw_circle(const Circle &circle, const Color &color) {
    int top_y = clamp<int>(
        0,
        static_cast<int>(
            (circle.pos.y + global_variables::arena_half_size.y - circle.radius) * global_variables::scale_factor
        ),
        static_cast<int>(global_variables::render_state.height())
    );
    int bottom_y = clamp<int>(
        0,
        static_cast<int>(
            (circle.pos.y + global_variables::arena_half_size.y + circle.radius) * global_variables::scale_factor
        ),
        static_cast<int>(global_variables::render_state.height()) - 1
    );

    for (int y = top_y; y <= bottom_y; y++) {
        efloat h = std::min(
            circle.radius,
            std::abs(y / global_variables::scale_factor - global_variables::arena_half_size.y - circle.pos.y)
        );

        efloat delta = sqrt(quare(circle.radius) - quare(h));

        int left_x = clamp<int>(
            0,
            static_cast<int>(
                (circle.pos.x + global_variables::arena_half_size.x - delta) * global_variables::scale_factor
            ),
            static_cast<int>(global_variables::render_state.width())
        );
        int right_x = clamp<int>(
            -1,
            static_cast<int>(
                (circle.pos.x + global_variables::arena_half_size.x + delta) * global_variables::scale_factor
            ),
            static_cast<int>(global_variables::render_state.width()) - 1
        );

        draw_pixels(left_x, y, right_x + 1, y + 1, color);
    }
}

void draw_line(const Line &line, efloat half_size, const Color &color) {
    Dot half_size_pt(half_size, half_size);

    if (std::abs(line.get_b()) >= 0.5) {
        for (unsigned int x = 0; x < global_variables::render_state.width(); x++) {
            efloat pos_x = x / global_variables::scale_factor - global_variables::arena_half_size.x;
            // a*x + b*y + c = 0
            // b*y = -(c + a*x)
            // y = -(c + a*x)/b
            efloat pos_y = -(line.get_c() + line.get_a() * pos_x) / line.get_b();
            draw_rect(Dot(pos_x, pos_y), half_size_pt, color);
        }
    } else {
        for (unsigned int y = 0; y < global_variables::render_state.height(); y++) {
            efloat pos_y = y / global_variables::scale_factor - global_variables::arena_half_size.y;
            // a*x + b*y + c = 0
            // a*x = -(c + b*y)
            // x = -(c + b*y)/a
            efloat pos_x = -(line.get_c() + line.get_b() * pos_y) / line.get_a();
            draw_rect(Dot(pos_x, pos_y), half_size_pt, color);
        }
    }
}

void draw_rhombus(Dot pos, Dot half_size, const Color &color) {
    int top_y = clamp<int>(
        0,
        static_cast<int>((pos.y + global_variables::arena_half_size.y - half_size.y) * global_variables::scale_factor),
        static_cast<int>(global_variables::render_state.height())
    );
    int bottom_y = clamp<int>(
        0,
        static_cast<int>((pos.y + global_variables::arena_half_size.y + half_size.y) * global_variables::scale_factor),
        static_cast<int>(global_variables::render_state.height()) - 1
    );

    for (int y = top_y; y <= bottom_y; y++) {
        efloat cur_h = std::abs(y / global_variables::scale_factor - global_variables::arena_half_size.y - pos.y);
        efloat delta = (half_size.y - cur_h) * half_size.x / half_size.y;

        int left_x = clamp<int>(
            0, static_cast<int>((pos.x + global_variables::arena_half_size.x - delta) * global_variables::scale_factor),
            static_cast<int>(global_variables::render_state.width())
        );
        int right_x = clamp<int>(
            -1,
            static_cast<int>((pos.x + global_variables::arena_half_size.x + delta) * global_variables::scale_factor),
            static_cast<int>(global_variables::render_state.width()) - 1
        );

        draw_pixels(left_x, y, right_x + 1, y + 1, color);
    }
}
