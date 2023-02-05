
#define TREE_DELTA_DRAW_POS dot(-16, 36) * gobj_state.size

struct Tree {

	inline static const game_object_state gobj_state = game_object_state(200, 4, 0.5);

	inline static const point_t collision_radius = 5;

	serialization_traits_byte(Tree);

	dot pos;

	s16 hp = gobj_state.hp;

	Tree(){}
	Tree(const dot& p) {
		pos = p;
	}

	collision_circle get_collision() const {
		return Circle(pos, collision_radius);
	}

	void draw() const {
		draw_sprite(pos + dot(-16, 6) * gobj_state.size, gobj_state.size, SP_LARGE_SHADOW);
		draw_sprite(pos + TREE_DELTA_DRAW_POS, gobj_state.size, SP_TREE);

		draw_collision_obj(*this);
		draw_hp(*this);
	}

	void simulate_hit(const Player& player) {
		add_hit_effect(pos + dot(-8, 8) * gobj_state.size);

		hp -= player.damage;

		if (hp <= 0) {
			add_death_effect(pos + dot(-12, 12) * gobj_state.size);
			Logs.push_back(pos);
		}
	}

};
