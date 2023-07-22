#include "render_sprite.hpp"

bool collision_in_draw_sprite(Dot pos, efloat size, sprite_t sprite, Dot collision_pos) {
    static_pos_update(pos);

    const auto &pixels = get_sprite(sprite);

    static const efloat epsilon = 1e-3;

    for (int i = 0; i < pixels.height(); i++) {
        for (int j = 0; j < pixels.width(); j++) {
            if (is_draw(pixels[i][j]) &&
                is_between(pos.x + size * j - epsilon, collision_pos.x, pos.x + size * (j + 1) + epsilon) &&
                is_between(pos.y - size * (i + 1) - epsilon, collision_pos.y, pos.y - size * i + epsilon)) {
                return true;
            }
        }
    }
    return false;
}