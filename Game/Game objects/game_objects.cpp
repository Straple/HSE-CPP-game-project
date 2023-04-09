
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
    efloat jump_radius; // радиус возможного прыжка до врага

    efloat ddp_speed;

    efloat paralyzed_cooldown;
    efloat attack_cooldown;
};

struct enemy_state {
    int damage;

    efloat collision_radius;
    efloat jump_radius;

    efloat ddp_speed;

    efloat paralyzed_cooldown;
    efloat attack_cooldown;


    enemy_state(
        int damage,

        efloat collision_radius,
        
        efloat jump_radius,  // dist jump

        efloat ddp_speed,

        efloat paralyzed_cooldown,
        efloat attack_cooldown

    ) {
        this->damage = damage;

        this->collision_radius = collision_radius;
        this->jump_radius = jump_radius;

        this->ddp_speed = ddp_speed;

        this->paralyzed_cooldown = paralyzed_cooldown;
        this->attack_cooldown = attack_cooldown;

    }
};

// функции внутри game objects
// simulate - обновить объект
// draw - рисовать объект
// get_collision - выдать коллизию объекта (если есть)

#include "button.hpp"
#include "cursor.hpp"
#include "bush.hpp"
#include "effect.hpp"

#include "player.cpp"
#include "bullet.hpp"
#include "weapon.hpp"

#include "loot.cpp"
#include "heart.cpp"

#include "log.hpp"
#include "tree.hpp"
// don't shuffle
// #include "fireplace_unused.hpp"

// enemies

#include "mini_slime.hpp"
#include "bat.hpp"
#include "slime.cpp"

#include "room.hpp"