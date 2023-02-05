
#define FIREPLACE_SIZE 0.4

#define FIREPLACE_DELTA_DRAW_POS Dot(-42, 10) * FIREPLACE_SIZE

#define FIREPLACE_COLLISION_RADIUS 10

#define FIREPLACE_LOG_EATE_RADIUS 20

#define FIREPLACE_LOG_ADD_TIME 1.5 * 60

struct Fireplace {
    Dot pos;

    Fire_machine fire;

    efloat time = 5 * 60;
    efloat cooldown_add_time_accum = 0;

    Fireplace() {
    }

    Fireplace(const Dot &p) {
        pos = p;
        fire = Fire_machine(pos, 5, 0.5, 0.5, 4, 0.15);
    }

    collision_circle get_collision() const {
        return collision_circle(Circle(
            pos + Dot(0, -10) * FIREPLACE_SIZE, FIREPLACE_COLLISION_RADIUS
        ));
    }

    void simulate(efloat delta_time) {
        /*if (rain.is_full_raining()) {
                time -= delta_time;
        }*/
        time -= delta_time;

        cooldown_add_time_accum += delta_time;

        if (cooldown_add_time_accum > 1) {
            fire.cooldown_add += 0.0005;
            cooldown_add_time_accum = 0;
        }

        fire.simulate(delta_time);

        for (int i = 0; i < Logs.size(); i++) {
            if ((pos - Logs[i].pos).get_len() <= FIREPLACE_LOG_EATE_RADIUS) {
                Logs.erase(Logs.begin() + i);
                i--;

                time += FIREPLACE_LOG_ADD_TIME;
                fire.cooldown_add = std::max(0.3, fire.cooldown_add - 0.3);
            }
        }
    }

    void draw() const {
        draw_sprite(
            pos + FIREPLACE_DELTA_DRAW_POS + Dot(10, 0) * FIREPLACE_SIZE,
            FIREPLACE_SIZE * 2, SP_LARGE_SHADOW
        );
        draw_sprite(
            pos + FIREPLACE_DELTA_DRAW_POS, FIREPLACE_SIZE, SP_FIREPLACE
        );

        fire.draw();

        Dot p = pos + Dot(-10, -30) * FIREPLACE_SIZE;
        static_pos_update(p);
        draw_object(int(time), p, 0.5, 0xff000000);

        draw_collision_obj(*this);
    }
};
