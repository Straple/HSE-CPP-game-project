#ifndef GAME_LOG_HPP
#define GAME_LOG_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"

struct Log : AbstractPhysicalObject {
    ADD_BYTE_SERIALIZATION();

    // visible
    inline const static efloat size = 0.3;
    inline const static Dot delta_draw_pos = Dot(-30, 18) * size;
    // physics
    inline const static efloat collision_radius = 4;

    Log() = default;

    explicit Log(const Dot &position);

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

    void draw() const override;

    void simulate(efloat delta_time);
};


#endif  // GAME_LOG_HPP