#include "line.hpp"

efloat Line::get_a() const { return a; }

[[nodiscard]] efloat Line::get_b() const { return b; }

[[nodiscard]] efloat Line::get_c() const { return c; }

Line::Line(const Dot &begin, const Dot &end) {
  a = begin.y - end.y;
  b = end.x - begin.x;
  // normalize
  {
    Dot ab = Dot(a, b).normalize();
    a = ab.x;
    b = ab.y;
  }
  c = -a * begin.x - b * begin.y;
}

Line::Line(efloat A, efloat B, efloat C) : a(A), b(B), c(C) {
  // normalize
  {
    efloat d = Dot(a, b).get_len();
    if (std::abs(d) > EPS) {
      a /= d;
      b /= d;
      c /= d;
    } else {
      a = b = c = 0;
    }
  }
}

// возвращает перпендикуляр из точки
[[nodiscard]] Line Line::get_perp(const Dot &point) const {
  efloat A = -b, B = a;
  efloat C = -A * point.x - B * point.y;
  return {A, B, C};
}

// возвращает параллельную прямую на расстоянии dist
// если оно будет отрицательно, то вернет с другой стороны
[[nodiscard]] Line Line::get_parallel(efloat dist) const {
  return {a, b, c + dist /* * sqrt(a * a + b * b)*/};
}

// возвращает нормализованный вектор
[[nodiscard]] Dot Line::get_vector() const {
  return Dot(-b, a) /*.normalize()*/;
}

// возвращает точку пересечения двух прямых
[[nodiscard]] Dot Line::intersect(const Line &Rhs) const {
  efloat x, y;
  // ax + by + c = 0
  // by = -c - ax
  // y  = (-ax - c) / b

  if (std::abs(Rhs.b) > EPS) {
    x = (b * Rhs.c / Rhs.b - c) / (a - b * Rhs.a / Rhs.b);
    y = (-x * Rhs.a - Rhs.c) / Rhs.b;
  } else {
    x = -Rhs.c / Rhs.a;
    y = (-x * a - c) / b;
  }
  return {x, y};
}

// возвращает точку пересечения перпендикуляра
[[nodiscard]] Dot Line::perp_intersect(const Dot &point) const {
  return intersect(get_perp(point));
}

// отражает точку от прямой
[[nodiscard]] Dot Line::reflection(const Dot &p) const {
  return p + (perp_intersect(p) - p) * 2;
}

// возвращает длину перпендикуляра
[[nodiscard]] efloat Line::dist(const Dot &point) const {
  return std::abs(a * point.x + b * point.y +
                  c) /* / std::sqrt(a * a + b * b)*/;
}

// возвращает расстояние между ПАРАЛЛЕЛЬНЫМИ прямыми
[[nodiscard]] efloat Line::dist(const Line &parallel) const {
  return std::abs(c - parallel.c) /* / sqrt(a * a + b * b)*/;
}

// параллельны ли прямые
[[nodiscard]] bool Line::is_parallel(const Line &rhs) const {
  return std::abs(a * rhs.b - b * rhs.a) < EPS;
}

// перпендикулярны ли прямые
[[nodiscard]] bool Line::is_perp(const Line &rhs) const {
  return std::abs(a * rhs.a + b * rhs.b) < EPS;
}

// лежит ли точка на прямой
[[nodiscard]] bool Line::ison(const Dot &point) const {
  return a * point.x + b * point.y + c == 0;
}

bool Line::operator==(const Line &Rhs) const {
  // 1 1  1
  // 1 0 -1
  // 0 1 -1
  // 0 0  1
  efloat sign;
  if ((a < 0) ^ (Rhs.a < 0)) {
    sign = -1;
  } else {
    sign = 1;
  }

  return std::abs(a - sign * Rhs.a) < EPS && std::abs(b - sign * Rhs.b) < EPS &&
         std::abs(c - sign * Rhs.c) < EPS;
}

std::ostream &operator<<(std::ostream &output, const Line &l) {
  return output << "Line(" << l.get_a() << ", " << l.get_b() << ", "
                << l.get_c() << ')';
}
