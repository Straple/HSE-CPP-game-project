
#define LOG_DELTA_DRAW_POS Dot(-30, 18) * gobj_state.size

#define LOG_DP_SPEED 200

struct Log {
    inline static const game_object_state gobj_state =
        game_object_state(1, 0, 0.3);

    inline static const efloat collision_radius = 4;

    inline static const int hp = 1;

    Dot pos;
    Dot dp;

    Log() = default;

    Log(const Dot &p) : pos(p) {
    }

    [[nodiscard]] collision_circle get_collision() const {
        return collision_circle(Circle(pos, collision_radius));
    }

    void simulate(efloat delta_time) {
        Dot ddp;
        simulate_move2d(pos, dp, ddp, delta_time);
    }

    void draw() const {
        draw_sprite(pos + LOG_DELTA_DRAW_POS, gobj_state.size, SP_LOG);

        draw_collision_obj(*this);
    }

    void simulate_hit(const Player &player) {
        add_hit_effect(pos + Dot(-10, 8) * gobj_state.size);

        dp += player.get_dir() * LOG_DP_SPEED;
    }
};

std::vector<Log> Logs;
