
// int hp, int exp_cnt, int damage,
// efloat collision_radius, efloat locator_radius, efloat persec_radius,
// efloat size

struct game_object_state {
    int hp;
    int exp_cnt;

    efloat size;

    game_object_state(int hp, int exp_cnt, efloat size) {
        this->hp = hp;
        this->exp_cnt = exp_cnt;
        this->size = size;
    }
};

struct enemy_state_for_trivial_enemy {
    int damage;

    efloat locator_radius; // радиус поиска цели
    efloat pursuit_radius; // радиус преследования цели
    efloat jump_radius; // радиус возможного прыжка до врага

    efloat ddp_speed;

    efloat paralyzed_cooldown;
    efloat attack_cooldown;

    efloat walk_time;
    efloat walk_sum_time;
};

struct enemy_state {
    int damage;

    efloat collision_radius;
    efloat locator_radius;
    efloat persec_radius;
    efloat jump_radius;

    efloat ddp_speed;

    efloat paralyzed_cooldown;
    efloat attack_cooldown;

    efloat walk_time;
    efloat walk_sum_time;

    enemy_state(
        int damage,

        efloat collision_radius,
        efloat locator_radius,
        efloat persec_radius,
        efloat jump_radius,  // dist jump

        efloat ddp_speed,

        efloat paralyzed_cooldown,
        efloat attack_cooldown,

        efloat walk_time,
        efloat walk_sum_time
    ) {
        this->damage = damage;

        this->collision_radius = collision_radius;
        this->locator_radius = locator_radius;
        this->persec_radius = persec_radius;
        this->jump_radius = jump_radius;

        this->ddp_speed = ddp_speed;

        this->paralyzed_cooldown = paralyzed_cooldown;
        this->attack_cooldown = attack_cooldown;

        this->walk_time = walk_time;
        this->walk_sum_time = walk_sum_time;
    }
};

// функции внутри game objects
// simulate - обновить объект
// draw - рисовать объект
// get_collision - выдать коллизию объекта (если есть)

#include "button.hpp"
#include "cursor.hpp"

#include "bullet.hpp"
#include "bush.hpp"
#include "effect.hpp"
#include "player.cpp"
#include "weapon.hpp"

// don't shuffle
#include "log.hpp"
// don't shuffle
#include "tree.hpp"
// don't shuffle
#include "fireplace.hpp"

// enemies

#include "bat.hpp"
#include "slime.cpp"