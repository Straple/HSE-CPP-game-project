
struct effect {
    Dot pos;
    animation anim;

    effect() = default;

    effect(const Dot &pos, const animation &anim) {
        this->pos = pos;
        this->anim = anim;
    }

    // вернет правду, если его анимация закончилась и нужно удалить объект
    bool simulate(efloat delta_time) {
        return anim.frame_update(delta_time);
    }

    void draw() const {
        anim.draw(pos, 0.4);
    }
};

std::vector<effect> Effects;

void add_hit_effect(const Dot &pos) {
    Effects.emplace_back(pos, animation(SS_HIT_EFFECT, 0, 2, 0.1));
}

void add_death_effect(const Dot &pos) {
    Effects.emplace_back(pos, animation(SS_DEATH_EFFECT, 0, 10, 0.1));
}
