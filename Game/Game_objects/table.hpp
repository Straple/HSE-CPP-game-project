#ifndef GAME_TABLE_HPP
#define GAME_TABLE_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"

struct Table : AbstractPhysicalObject {
    ADD_BYTE_SERIALIZATION();

    // visible
    inline const static efloat size = 1;
    inline const static Dot delta_draw_pos = Dot(-10, 30);

    Table() = default;

    explicit Table(const Dot new_pos);

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

    void draw() const override;
};

static std::vector<Table> Tables;

#endif  // GAME_TABLE_HPP