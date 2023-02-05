
//s16 hp, s16 exp_cnt, s16 damage,
//point_t collision_radius, point_t locator_radius, point_t persec_radius, point_t size

struct game_object_state {

	s16 hp;
	s16 exp_cnt;

	point_t size;

	game_object_state(s16 hp, s16 exp_cnt, point_t size) {

		this->hp = hp;
		this->exp_cnt = exp_cnt;
		this->size = size;
	}
};

struct enemy_state {

	s16 damage;

	point_t collision_radius;
	point_t locator_radius;
	point_t persec_radius;
	point_t jump_radius; // dist jump

	point_t ddp_speed;

	point_t paralyzed_cooldown;
	point_t attack_cooldown;

	point_t walk_time;
	point_t walk_sum_time;

	enemy_state(s16 damage,

	point_t collision_radius,
	point_t locator_radius,
	point_t persec_radius,
	point_t jump_radius, // dist jump

	point_t ddp_speed,

	point_t paralyzed_cooldown,
	point_t attack_cooldown,

	point_t walk_time,
	point_t walk_sum_time) {


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

#include "effect.cpp"

#include "rain.cpp"

#include "bush.cpp"

#include "player.cpp"

// enemies

#include "log.cpp"

#include "fireplace.cpp"

#include "tree.cpp"

#include "slime.cpp"

#include "bat.cpp"
