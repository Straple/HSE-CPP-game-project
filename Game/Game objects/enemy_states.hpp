#ifndef GAME_ENEMY_STATES_HPP
#define GAME_ENEMY_STATES_HPP

#include "utils.hpp"

struct enemy_state_for_trivial_enemy {
    int damage;
    efloat jump_radius;  // радиус возможного прыжка до врага

    efloat ddp_speed;

    efloat paralyzed_cooldown;
    efloat attack_cooldown;
};

#endif  // GAME_ENEMY_STATES_HPP