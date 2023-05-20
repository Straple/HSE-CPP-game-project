#ifndef GAME_EFFECT_HPP
#define GAME_EFFECT_HPP

#include "render.hpp"

struct effect {
    efloat size = 0.5;
    Dot pos;
    animation anim;

    effect() = default;

    effect(const Dot &pos, const animation &anim)
        : pos(pos), anim(anim) {
    }

    // вернет правду, если его анимация закончилась и нужно удалить объект
    bool simulate(efloat delta_time) {
        return anim.frame_update(delta_time);
    }

    void draw() const {
        anim.draw(pos, size);
    }
};

std::vector<effect> Effects;

void add_hit_effect(const Dot &pos) {
    Effects.emplace_back(
        pos + Dot(-12, 12) * 0.5, animation(SS_HIT_EFFECT, 0, 2, 0.1)
    );
}

void add_death_effect(const Dot &pos) {
    Effects.emplace_back(
        pos + Dot(-16, 16) * 0.5, animation(SS_DEATH_EFFECT, 0, 10, 0.1)
    );
}

#endif  // GAME_EFFECT_HPP