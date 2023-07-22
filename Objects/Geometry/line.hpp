#ifndef GAME_ENGINE_GEOMETRY_LINE_HPP
#define GAME_ENGINE_GEOMETRY_LINE_HPP

#include "dot.hpp"

// (a, b, c)
// ax * by + c = 0
class Line {
    efloat a = 0, b = 0, c = 0;
    // a, b нормализованы для оптимизации
    // во избежание ошибок a, b, c закрыты
    // закомментированный код показывает, что нужно было бы сделать, если бы a, b
    // были не нормализованы

public:
    [[nodiscard]] efloat get_a() const;

    [[nodiscard]] efloat get_b() const;

    [[nodiscard]] efloat get_c() const;

    Line() = default;

    Line(const Dot &begin, const Dot &end);

    Line(efloat A, efloat B, efloat C);

    // возвращает перпендикуляр из точки
    [[nodiscard]] Line get_perp(const Dot &point) const;

    // возвращает параллельную прямую на расстоянии dist
    // если оно будет отрицательно, то вернет с другой стороны
    [[nodiscard]] Line get_parallel(efloat dist) const;

    // возвращает нормализованный вектор
    [[nodiscard]] Dot get_vector() const;

    // возвращает точку пересечения двух прямых
    [[nodiscard]] Dot intersect(const Line &Rhs) const;

    // возвращает точку пересечения перпендикуляра
    [[nodiscard]] Dot perp_intersect(const Dot &point) const;

    // отражает точку от прямой
    [[nodiscard]] Dot reflection(const Dot &p) const;

    // возвращает длину перпендикуляра
    [[nodiscard]] efloat dist(const Dot &point) const;

    // возвращает расстояние между ПАРАЛЛЕЛЬНЫМИ прямыми
    [[nodiscard]] efloat dist(const Line &parallel) const;

    // параллельны ли прямые
    [[nodiscard]] bool is_parallel(const Line &rhs) const;

    // перпендикулярны ли прямые
    [[nodiscard]] bool is_perp(const Line &rhs) const;

    // лежит ли точка на прямой
    [[nodiscard]] bool ison(const Dot &point) const;

    bool operator==(const Line &Rhs) const;
};

std::ostream &operator<<(std::ostream &output, const Line &l);

#endif  // GAME_ENGINE_GEOMETRY_LINE_HPP