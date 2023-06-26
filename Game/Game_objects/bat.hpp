#ifndef GAME_BAT_HPP
#define GAME_BAT_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"
#include "mob.hpp"

struct Bat : Mob {
    ADD_BYTE_SERIALIZATION();

    // visible
    inline const static efloat size = 0.8;
    inline const static Dot delta_draw_pos = Dot(-8, 26) * size;
    // physics
    inline const static efloat collision_radius = 7;
    inline const static efloat jump_radius = 8;
    inline const static efloat ddp_speed = 600;
    // cooldowns
    inline const static efloat attack_cooldown = 4;
    // others
    inline const static int damage = 1;

    efloat attack_accum = 0;

    animation anim = animation(SS_BAT, 0, 5, 1.0 / 7);

    int hp = 3;

    Bat(const Dot &position = Dot());

    [[nodiscard]] bool is_invulnerable() const;

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

    [[nodiscard]] std::unique_ptr<Collision> get_hitbox() const;

    void simulate(
        efloat delta_time,
        const std::set<grid_pos_t> &visitable_grid_dots
    );

    void draw() override;
};

#endif  // GAME_BAT_HPP