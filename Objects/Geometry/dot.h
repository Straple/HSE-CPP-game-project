#pragma once

#include "../../utils.h"

// (x, y)
struct dot {
    point_t x, y;

    dot() {
        x = y = 0;
    }
    dot(point_t x, point_t y) {
        this->x = x;
        this->y = y;
    }

    dot operator - () const {
        return dot(-x, -y);
    }

    dot operator + (const dot& p) const {
        return dot(x + p.x, y + p.y);
    }
    dot& operator += (const dot& p) {
        return *this = *this + p;
    }

    dot operator - (const dot& p) const {
        return dot(x - p.x, y - p.y);
    }
    dot& operator -= (const dot& p) {
        return *this = *this - p;
    }

    dot operator * (point_t k) const {
        return dot(x * k, y * k);
    }
    dot& operator *= (point_t k) {
        return *this = *this * k;
    }

    dot operator / (point_t k) const {
        return dot(x / k, y / k);
    }
    dot& operator /= (point_t k) {
        return *this = *this / k;
    }

    // векторное/косое произведение
    // Это площадь параллелограмма
    point_t operator % (const dot& p) const {
        return x * p.y - y * p.x;
    }
    // скалярное произведение
    point_t operator * (const dot& p) const {
        return x * p.x + y * p.y;
    }

    point_t getQuareLen() const {
        return x * x + y * y;
    }
    point_t getLen() const {
        return hypot(x, y);
    }

    bool operator == (const dot& Rhs) const {
        return x == Rhs.x && y == Rhs.y;
    }
    bool operator != (const dot& Rhs) const {
        return !(*this == Rhs);
    }

    // самая левая, потом самая нижняя
    bool operator < (const dot& Rhs) const {
        return x == Rhs.x ? y < Rhs.y : x < Rhs.x;
    }
    // самая правая, потом самая верхняя
    bool operator > (const dot& Rhs) const {
        return x == Rhs.x ? y > Rhs.y : x > Rhs.x;
    }

    dot normalize() const {
        if (getLen() != 0) {
            return *this / getLen();
        }
        else {
            return dot();
        }
    }
};
std::ostream& operator << (std::ostream& output, const dot& p) {
    return output << '(' << p.x << ", " << p.y << ")";
}
