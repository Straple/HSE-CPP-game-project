#define HEART_DELTA_DRAW_DOT Dot(10,10)
struct Heart: Loot {
    sprite_t image = SP_HEART;
    Heart(Dot init): Loot(init) {}

    bool simulate_collection() override {
        if (collection_trigger()) {
            player.hp+=50;
            return true;
        }
        return false;
    }
    void draw() const override {
        draw_sprite(pos+HEART_DELTA_DRAW_DOT*0.25-Dot(4.25,0.5), 0.3, SP_HEART);
        draw_collision_obj(*this);
    }
};