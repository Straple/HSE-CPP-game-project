#ifndef GAME_EFFECT_HPP
#define GAME_EFFECT_HPP

#include "../../render.hpp"
#include "abstract_object.hpp"

struct Effect : AbstractObject {
  ADD_BYTE_SERIALIZATION();

  Dot delta_draw_pos;
  efloat size = 0.5;
  animation anim;

  Effect() = default;

  // pos лежит в AbstractObject
  Effect(const Dot &pos, const animation &anim);

  // вернет правду, если его анимация закончилась и нужно удалить объект
  bool simulate(efloat delta_time);

  void draw();
};

void add_hit_effect(const Dot &pos);

void add_death_effect(const Dot &pos);

void add_flower_effect(const Dot &pos);

void add_flower_dome_effect(const Dot &pos);

#endif // GAME_EFFECT_HPP