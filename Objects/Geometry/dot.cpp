#include "dot.hpp"
#include <cmath>
#include "../../assert.hpp"

Dot::Dot(efloat x, efloat y)
    : x(x), y(y) {
}

Dot Dot::operator-() const {
    return {-x, -y};
}

Dot &Dot::operator+=(const Dot &p) {
    return *this = *this + p;
}

Dot &Dot::operator-=(const Dot &p) {
    return *this = *this - p;
}

Dot Dot::operator*(efloat k) const {
    return {x * k, y * k};
}

Dot &Dot::operator*=(efloat k) {
    return *this = *this * k;
}

Dot Dot::operator/(efloat k) const {
    return {x / k, y / k};
}

Dot &Dot::operator/=(efloat k) {
    return *this = *this / k;
}

efloat Dot::get_quare_len() const {
    return x * x + y * y;
}

efloat Dot::get_len() const {
    // hypot в ~4 раза медленней, но точнее
    // return hypot(x, y);
    return std::sqrt(x * x + y * y);
}

Dot Dot::normalize() const {
    if (std::abs(x) + std::abs(y) > EPS) {
        return *this / get_len();
    } else {
        return {};
    }
}

Dot operator+(const Dot &lhs, const Dot &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

Dot operator-(const Dot &lhs, const Dot &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

Dot operator*(efloat k, Dot p) {
    return p * k;
}

bool operator==(const Dot &lhs, const Dot &rhs) {
    return std::abs(lhs.x - rhs.x) < EPS && std::abs(lhs.y - rhs.y) < EPS;
    // return x == Rhs.x && y == Rhs.y;
}

bool operator!=(const Dot &lhs, const Dot &rhs) {
    return !(lhs == rhs);
}

// самая левая, потом самая нижняя
bool operator<(const Dot &lhs, const Dot &rhs) {
    return lhs.x == rhs.x ? lhs.y < rhs.y : lhs.x < rhs.x;
}

// самая правая, потом самая верхняя
bool operator>(const Dot &lhs, const Dot &rhs) {
    return lhs.x == rhs.x ? lhs.y > rhs.y : lhs.x > rhs.x;
}

// векторное/косое произведение
// Это площадь параллелограмма
efloat operator%(const Dot &lhs, const Dot &rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

// скалярное произведение
efloat operator*(const Dot &lhs, const Dot &rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

std::ostream &operator<<(std::ostream &output, const Dot &p) {
    return output << "Dot(" << p.x << ", " << p.y << ')';
}

std::istream &operator>>(std::istream &input, Dot &p) {
    char c;
    input >> c;  // D
    ASSERT(c == 'D', "failed read Dot");

    input >> c;  // o
    ASSERT(c == 'o', "failed read Dot");

    input >> c;  // t
    ASSERT(c == 't', "failed read Dot");

    input >> c;  // (
    ASSERT(c == '(', "failed read Dot");

    input >> p.x;

    input >> c;  // ,
    ASSERT(c == ',', "failed read Dot");

    input >> p.y;

    input >> c;  // )
    ASSERT(c == ')', "failed read Dot");

    return input;
}

// возвращает угол между векторами
efloat get_angle(const Dot &a, const Dot &b) {
    return atan2(a % b, a * b);
}

// возвращает неотрицательный угол между векторами
efloat get_good_angle(const Dot &a, const Dot &b) {
    efloat res = get_angle(a, b);
    if (res < 0) {
        res += 2 * PI;
    }
    return res;
}

// возвращает неотрицательный угол меньше 180 между векторами
efloat get_very_good_angle(const Dot &a, const Dot &b) {
    efloat res = get_good_angle(a, b);
    if (res > PI) {
        res = 2 * PI - res;
    }
    return res;
}
