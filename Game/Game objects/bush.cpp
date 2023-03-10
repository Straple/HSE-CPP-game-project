#include "abstract_game_object.h"

struct Bush : abstract_game_object {
    Bush() = default;

    explicit Bush(const Dot &new_pos) {
        size = 0.5;
        delta_draw_pos = Dot(-15, 20) * size;
        collision_radius = 5;
        pos = new_pos - delta_draw_pos;
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }

    void draw() const override {
        draw_sprite(
            pos + delta_draw_pos + Dot(0, -13) * size, size, SP_LARGE_SHADOW
        );
        draw_sprite(pos + delta_draw_pos, size, SP_BUSH);

        draw_collision_obj(*this);

        draw_rect(pos - camera.pos, Dot(1, 1) * 0.3, RED);
    }
};
