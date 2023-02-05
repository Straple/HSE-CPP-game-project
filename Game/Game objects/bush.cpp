// visibility
#define BUSH_SIZE 0.5

// fixeds
#define BUSH_DELTA_DRAW_POS Dot(-15, 20) * BUSH_SIZE

// physics
#define BUSH_COLLISION_RADIUS 5

struct Bush {
    Dot pos;

    Bush() = default;

    explicit Bush(const Dot &new_pos) : pos(new_pos - BUSH_DELTA_DRAW_POS) {
    }

    [[nodiscard]] collision_circle get_collision() const {
        return collision_circle(Circle(pos, BUSH_COLLISION_RADIUS));
    }

    void draw() const {
        draw_sprite(
            pos + BUSH_DELTA_DRAW_POS + Dot(0, -13) * BUSH_SIZE, BUSH_SIZE,
            SP_LARGE_SHADOW
        );
        draw_sprite(pos + BUSH_DELTA_DRAW_POS, BUSH_SIZE, SP_BUSH);

        draw_collision_obj(*this);
    }
};
