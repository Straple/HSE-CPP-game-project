// visibility
#define BUSH_SIZE 0.5

// fixeds
#define BUSH_DELTA_DRAW_POS dot(-15, 20) * BUSH_SIZE

// physics
#define BUSH_COLLISION_RADIUS 5

struct Bush {
	dot pos;

	serialization_traits_byte(Bush);

	Bush(dot pos) {
		this->pos = pos - BUSH_DELTA_DRAW_POS;
	}
	Bush() {}

	collision_circle get_collision() const {
		return Circle(pos, BUSH_COLLISION_RADIUS);
	}

	void draw() const {
		draw_sprite(pos + BUSH_DELTA_DRAW_POS + dot(0, -13) * BUSH_SIZE, BUSH_SIZE, SP_LARGE_SHADOW);
		draw_sprite(pos + BUSH_DELTA_DRAW_POS, BUSH_SIZE, SP_BUSH);

		draw_collision_obj(*this);
	}
};
