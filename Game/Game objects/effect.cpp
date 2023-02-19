﻿
struct effect {
	Dot pos;

	animation anim;

	effect() {}

	effect(const Dot& pos, const animation& anim) {
		this->pos = pos;
		this->anim = anim;
	}

	// вернет правду, если его анимация закончилась и нужно удалить объект
	bool simulate(efloat delta_time) {
		return anim.frame_update(delta_time);
	}

	void draw() {
		anim.draw(pos, 0.4);
	}
};

#define HIT_EFFECT_ANIM animation(SS_HIT_EFFECT, 0, 2, 0.1)
#define DEATH_EFFECT_ANIM animation(SS_DEATH_EFFECT, 0, 10, 0.1)

std::vector<effect> Effects;

void add_hit_effect(const Dot& pos) {
	Effects.push_back(effect(pos, HIT_EFFECT_ANIM));
}

void add_death_effect(const Dot& pos) {
	Effects.push_back(effect(pos, DEATH_EFFECT_ANIM));
}

