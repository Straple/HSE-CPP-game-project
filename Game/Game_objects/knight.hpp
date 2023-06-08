#ifndef GAME_KNIGHT_HPP
#define GAME_KNIGHT_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"

struct Knight : AbstractPhysicalObject {
    ADD_BYTE_SERIALIZATION();

    // visible
    inline const static efloat size = 1.5;
    inline const static Dot delta_draw_pos = Dot(-16, 28) * size;

    Knight() = default;

    explicit Knight(const Dot &position);

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

    void draw() override;

    bool trigger_in_draw_sprite(const Dot &check_pos);
};

#endif  // GAME_KNIGHT_HPP