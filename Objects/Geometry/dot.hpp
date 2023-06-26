#ifndef GAME_ENGINE_GEOMETRY_DOT_HPP
#define GAME_ENGINE_GEOMETRY_DOT_HPP

#include "geometry_utils.hpp"
#include <iostream>

// (x, y)
struct Dot {
  efloat x = 0, y = 0;

  Dot() = default;

  Dot(efloat x, efloat y);

  [[nodiscard]] Dot operator-() const;

  Dot &operator+=(const Dot &p);

  Dot &operator-=(const Dot &p);

  [[nodiscard]] Dot operator*(efloat k) const;

  Dot &operator*=(efloat k);

  [[nodiscard]] Dot operator/(efloat k) const;

  Dot &operator/=(efloat k);

  [[nodiscard]] efloat get_quare_len() const;

  // VERY SLOW!!!
  // зато очень точно
  [[nodiscard]] efloat get_len() const;

  [[nodiscard]] Dot normalize() const;
};

Dot operator+(const Dot &lhs, const Dot &rhs);

Dot operator-(const Dot &lhs, const Dot &rhs);

Dot operator*(efloat k, Dot p);

bool operator==(const Dot &lhs, const Dot &rhs);

bool operator!=(const Dot &lhs, const Dot &rhs);

// самая левая, потом самая нижняя
bool operator<(const Dot &lhs, const Dot &rhs);

// самая правая, потом самая верхняя
bool operator>(const Dot &lhs, const Dot &rhs);

// векторное/косое произведение
// Это площадь параллелограмма
efloat operator%(const Dot &lhs, const Dot &rhs);

// скалярное произведение
efloat operator*(const Dot &lhs, const Dot &rhs);

std::ostream &operator<<(std::ostream &output, const Dot &p);

std::istream &operator>>(std::istream &input, Dot &p);

// возвращает угол между векторами
efloat get_angle(const Dot &a, const Dot &b);

// возвращает неотрицательный угол между векторами
efloat get_good_angle(const Dot &a, const Dot &b);

// возвращает неотрицательный угол меньше 180 между векторами
efloat get_very_good_angle(const Dot &a, const Dot &b);

#endif // GAME_ENGINE_GEOMETRY_DOT_HPP