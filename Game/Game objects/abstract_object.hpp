#ifndef GAME_POSITION_HPP
#define GAME_POSITION_HPP

#include "../../objects.hpp"

struct AbstractObject {
    Dot pos;

    AbstractObject() = default;

    AbstractObject(Dot pos) : pos(pos) {
    }

    virtual void draw() const = 0;
};

#endif  // GAME_POSITION_HPP