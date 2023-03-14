
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

struct enemy_state {
    int damage;

    efloat collision_radius;
    efloat locator_radius;
    efloat persec_radius;
    efloat jump_radius;  // dist jump

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


#include "bush.cpp"
#include "effect.cpp"
#include "player.cpp"


#include "bullet.cpp"
#include "weapon.cpp"
#include "rain.cpp"

// don't shuffle
#include "log.cpp"
// don't shuffle
#include "tree.cpp"
// don't shuffle
#include "fireplace.cpp"

// enemies

#include "bat.cpp"
#include "slime.cpp"