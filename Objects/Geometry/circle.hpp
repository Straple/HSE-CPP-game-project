#ifndef GAME_ENGINE_GEOMETRY_CIRCLE_HPP
#define GAME_ENGINE_GEOMETRY_CIRCLE_HPP

#include "dot.hpp"

// (pos, radius)
struct Circle {
  Dot pos;
  efloat radius = 0;

  Circle() = default;

  Circle(const Dot &new_pos, efloat new_radius);

  // возвращает точку на окружности
  // угол против часовой стрелки
  [[nodiscard]] Dot get_dot(efloat angle) const;

  // возвращает рандомную точку на окружности
  [[nodiscard]] Dot get_random_dot() const;

  // проверка принадлежности точки окружности
  [[nodiscard]] bool ison(const Dot &point) const;

  // проверка принадлежности точки кругу
  [[nodiscard]] bool isin(const Dot &point) const;
};

std::ostream &operator<<(std::ostream &output, const Circle &c);

#endif // GAME_ENGINE_GEOMETRY_CIRCLE_HPP