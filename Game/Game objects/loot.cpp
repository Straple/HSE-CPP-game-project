#define LOOT_COLLISION_RADIUS 4

struct Loot;
static std::vector<std::unique_ptr<Loot>>Loot_objects;

struct Loot {
    Dot pos;
    const efloat magnet_radius=30;
    Dot initial_pos;
    bool initial_flight = true;
    Dot dp;
    Loot() = default;
    Loot(Dot init): initial_pos(init), pos(init) {}

    [[nodiscard]] collision_circle get_collision() const {
        return collision_circle(Circle(pos, LOOT_COLLISION_RADIUS));
    }

    [[nodiscard]] bool collection_trigger() const {
        return get_collision().trigger(player.pos);
    }
    virtual bool simulate_collection() = 0;

    virtual void draw() const = 0;

    void simulate(efloat delta_time) {
        if ((player.pos-pos).get_len() <= magnet_radius) {
            initial_flight = false;
            Dot ddp = (player.pos-pos).normalize() * 100;
            move_to2d(pos, player.pos, dp, ddp, delta_time);
        }
        else if (initial_flight) {
            if ((pos-initial_pos).get_len() >= 6) {
                initial_flight=false;
            }
            else {
                move_to2d(pos,initial_pos+Dot(4,4),dp,(initial_pos+Dot(2,2))*2, delta_time);
            }
        }
    }
};
