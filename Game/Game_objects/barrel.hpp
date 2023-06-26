#ifndef GAME_BARREL_HPP
#define GAME_BARREL_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"

struct Barrel : AbstractPhysicalObject {
  ADD_BYTE_SERIALIZATION();

  // visible
  inline const static efloat size = 1;
  inline const static Dot delta_draw_pos = Dot(-16, 20) * size;

  Barrel() = default;

  explicit Barrel(const Dot &position);

  [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

  void draw() override;

  bool trigger_in_draw_sprite(const Dot &check_pos);
};

#endif // GAME_BARREL_HPP