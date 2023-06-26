#ifndef GAME_NUN_STATUE_HPP
#define GAME_NUN_STATUE_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"

struct NunStatue : AbstractPhysicalObject {
  ADD_BYTE_SERIALIZATION();

  // visible
  inline const static efloat size = 0.7;
  inline const static Dot delta_draw_pos = Dot(-64, 90) * size;

  NunStatue() = default;

  explicit NunStatue(const Dot &position);

  [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

  void draw() override;

  bool trigger_in_draw_sprite(const Dot &check_pos);
};

#endif // GAME_NUN_STATUE_HPP