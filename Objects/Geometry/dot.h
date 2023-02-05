#ifndef GAME_ENGINE_GEOMETRY_DOT
#define GAME_ENGINE_GEOMETRY_DOT

#include "geometry_utils.h"

// (x, y)
struct Dot {
    efloat x = 0, y = 0;

    Dot() = default;

    Dot(efloat new_x, efloat new_y) : x(new_x), y(new_y) {
    }

    [[nodiscard]] Dot operator-() const {
        return {-x, -y};
    }

    friend Dot operator+(const Dot &lhs, const Dot &rhs) {
        return {lhs.x + rhs.x, lhs.y + rhs.y};
    }

    friend Dot operator-(const Dot &lhs, const Dot &rhs) {
        return {lhs.x - rhs.x, lhs.y - rhs.y};
    }

    Dot &operator+=(const Dot &p) {
        return *this = *this + p;
    }

    Dot &operator-=(const Dot &p) {
        return *this = *this - p;
    }

    [[nodiscard]] Dot operator*(efloat k) const {
        return {x * k, y * k};
    }

    friend Dot operator*(efloat k, Dot p) {
        return p * k;
    }

    Dot &operator*=(efloat k) {
        return *this = *this * k;
    }

    [[nodiscard]] Dot operator/(efloat k) const {
        return {x / k, y / k};
    }

    Dot &operator/=(efloat k) {
        return *this = *this / k;
    }

    [[nodiscard]] efloat get_quare_len() const {
        return x * x + y * y;
    }

    // VERY SLOW!!!
    // зато очень точно
    [[nodiscard]] efloat get_len() const {
        return hypot(x, y);
    }

    [[nodiscard]] Dot normalize() const {
        if (std::abs(x) + std::abs(y) > EPS) {
            return *this / get_len();
        } else {
            return {};
        }
    }
};

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

#endif  // GAME_ENGINE_GEOMETRY_DOT