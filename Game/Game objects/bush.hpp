#ifndef GAME_BUSH_HPP
#define GAME_BUSH_HPP

#include "abstract_game_object.hpp"
#include "render.hpp"

struct Bush : abstract_game_object {
    ADD_BYTE_SERIALIZATION();

    Bush() = default;

    explicit Bush(const Dot &new_pos) {
        size = 1;
        delta_draw_pos = Dot(-16, 20) * size;
        collision_radius = 10;
        pos = new_pos;
    }

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override {
        return std::make_unique<CollisionCircle>(pos, collision_radius);
    }

    void draw() const override {
        draw_sprite(pos + delta_draw_pos + Dot(0, -13) * size, size, SP_LARGE_SHADOW);
        draw_sprite(pos + delta_draw_pos, size, SP_BUSH);

        draw_collision(*this);
    }
};

std::vector<Bush> Bushes = {
    //Bush(Dot(49, -106)),
};

#endif  // GAME_BUSH_HPP