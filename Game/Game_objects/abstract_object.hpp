#ifndef GAME_POSITION_HPP
#define GAME_POSITION_HPP

#include "../../Serialize/serialize.hpp"
#include "../../objects.hpp"

struct AbstractObject {
    Dot pos;

    AbstractObject() = default;

    AbstractObject(Dot pos);

    virtual void draw() = 0;
};

#endif  // GAME_POSITION_HPP