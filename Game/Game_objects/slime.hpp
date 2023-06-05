#ifndef GAME_SLIME_HPP
#define GAME_SLIME_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"
#include "effect.hpp"
#include "game_utils.hpp"
#include "mob.hpp"
#include "player.hpp"

struct Slime : Mob {
    ADD_BYTE_SERIALIZATION();

    // visible
    inline const static efloat size = 0.8;
    inline const static Dot delta_draw_pos = Dot(-30, 38) * size;
    inline const static uint8_t draw_alpha = 210;
    inline const static efloat frame_duration = 1.0 / 7;
    inline const static animation animation_idle = animation(SS_SLIME, 0, 24, frame_duration),
                                  animation_devour = animation(SS_SLIME, 25, 30, frame_duration),
                                  animation_shot = animation(SS_SLIME, 55, 13, frame_duration);
    // physics
    inline const static efloat collision_radius = 6;
    inline const static efloat ddp_speed = 400;
    // cooldowns
    inline const static efloat devour_cooldown = 5;
    inline const static efloat shot_cooldown = 10;
    // others
    inline const static efloat jump_radius = 14;
    inline const static int devour_damage = 2;

    int hp = 4;

    efloat devour_accum = 0;
    efloat shot_accum = 0;

    animation anim = animation_idle;

    bool is_devour = false;
    bool is_shooting = false;

    Slime(const Dot &position = Dot());

    [[nodiscard]] bool is_invulnerable() const;

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

    [[nodiscard]] std::unique_ptr<Collision> get_hitbox() const;

    bool is_reachable(Dot from, Dot to, std::vector<CollisionBox> &Walls);

    void simulate(
        const efloat delta_time,
        const std::set<grid_pos_t> &visitable_grid_dots,
        std::vector<CollisionBox> &Walls
    );

    void draw() const override;
};



#endif  // GAME_SLIME_HPP