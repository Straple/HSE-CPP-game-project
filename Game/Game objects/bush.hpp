#ifndef GAME_BUSH_HPP
#define GAME_BUSH_HPP

#include "../../render.hpp"
#include "abstract_game_object.hpp"

struct Bush : AbstractGameObject {
    ADD_BYTE_SERIALIZATION();

    Bush() = default;

    explicit Bush(const Dot &position) {
        size = 1;
        delta_draw_pos = Dot(-16, 20) * size;
        collision_radius = 10;
        pos = position;
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
    // Bush(Dot(49, -106)),
};

#endif  // GAME_BUSH_HPP