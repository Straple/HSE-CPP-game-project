#ifndef BULLETPROOF_BOMBER_HPP
#define BULLETPROOF_BOMBER_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"
#include "mob.hpp"

struct Bomber : Mob {
  ADD_BYTE_SERIALIZATION();

  // visible
  inline const static efloat size = 0.8;
  inline const static Dot delta_draw_pos = Dot(-30, 38) * size;
  inline const static uint8_t draw_alpha = 210;
  inline const static efloat frame_duration = 1.0 / 7;
  inline const static animation animation_idle =
                                    animation(SS_BOMBER, 0, 10, frame_duration),
                                animation_boom = animation(SS_BOOM, 0, 18,
                                                           frame_duration / 2);
  // physics
  inline const static efloat collision_radius = 6;
  inline const static efloat ddp_speed = 400;
  // others
  inline const static efloat jump_radius = 14;
  inline const static efloat boom_radius = 40;
  inline const static int boom_damage = 2;

  int hp = 2;

  animation anim = animation_idle;

  bool is_booming = false;

  Bomber(const Dot &position = Dot());

  [[nodiscard]] bool is_invulnerable() const;

  [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

  [[nodiscard]] std::unique_ptr<Collision> get_hitbox() const;

  void simulate(const efloat delta_time,
                const std::set<grid_pos_t> &visitable_grid_dots);

  void do_boom();

  void draw() override;
};

#endif // BULLETPROOF_BOMBER_HPP