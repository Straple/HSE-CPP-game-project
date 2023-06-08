#include "effect.hpp"
//
#include "../../sprites.hpp"
#include "game_variables.hpp"

// pos лежит в AbstractObject
Effect::Effect(const Dot &pos, const animation &anim) : AbstractObject(pos), anim(anim) {
}

// вернет правду, если его анимация закончилась и нужно удалить объект
bool Effect::simulate(efloat delta_time) {
    return anim.frame_update(delta_time);
}

void Effect::draw() {
    anim.draw(pos + delta_draw_pos, size);
}

void add_hit_effect(const Dot &pos) {
    Effect effect(pos, animation(SS_HIT_EFFECT, 0, 2, 0.1));
    effect.delta_draw_pos = Dot(-12, 12) * 0.5;
    game_variables::Effects.push_back(effect);
}

void add_death_effect(const Dot &pos) {
    Effect effect(pos, animation(SS_DEATH_EFFECT, 0, 10, 0.1));
    effect.delta_draw_pos = Dot(-16, 16) * 0.5;
    game_variables::Effects.push_back(effect);
}

void add_flower_effect(const Dot &pos) {
    uint32_t frame_shift = get_random_engine()() % 4;
    Effect effect(pos, animation(SS_FLOWER_EFFECT, frame_shift, 52 - frame_shift, 0.2));
    effect.delta_draw_pos = Dot(-16, 22) * 0.5;
    game_variables::Effects.push_back(effect);
}

void add_flower_dome_effect(const Dot &pos) {
    add_flower_effect(pos);
    add_flower_effect(pos + Dot(10, -10));
    add_flower_effect(pos + Dot(20, -20));
    add_flower_effect(pos + Dot(25, -5));
    add_flower_effect(pos + Dot(-5, -15));
    add_flower_effect(pos + Dot(-12, -25));
    add_flower_effect(pos + Dot(-15, 0));
    add_flower_effect(pos + Dot(-22, -10));
    add_flower_effect(pos + Dot(-5, 10));
    add_flower_effect(pos + Dot(10, 12));
    add_flower_effect(pos + Dot(-7, 18));
    add_flower_effect(pos + Dot(10, -23));
    add_flower_effect(pos + Dot(-22, 11));
    add_flower_effect(pos + Dot(13, 2));
    add_flower_effect(pos + Dot(22, 7));
}
