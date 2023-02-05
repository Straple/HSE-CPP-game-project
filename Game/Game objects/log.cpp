
#define LOG_DELTA_DRAW_POS dot(-30, 18) * gobj_state.size

#define LOG_DP_SPEED 200

struct Log {

	inline static const game_object_state gobj_state = game_object_state(1, 0, 0.3);

	inline static const point_t collision_radius = 4;

	inline static const s16 hp = 1;

	serialization_traits_byte(Log);

	dot pos;
	dot dp;


	Log(){}
	Log(const dot& p) {
		pos = p;
	}

	collision_circle get_collision() const {
		return Circle(pos, collision_radius);
	}

	void simulate(point_t delta_time) {
		dot ddp;
		simulate_move2d(pos, dp, ddp, delta_time);
	}

	void draw() const {
		draw_sprite(pos + LOG_DELTA_DRAW_POS, gobj_state.size, SP_LOG);

		draw_collision_obj(*this);
	}
	
	void simulate_hit(const Player& player) {
		add_hit_effect(pos + dot(-10, 8) * gobj_state.size);

		dp += player.get_dir() * LOG_DP_SPEED;
	}
};

std::vector<Log> Logs;
