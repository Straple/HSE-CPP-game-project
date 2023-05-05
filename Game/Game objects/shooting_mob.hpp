//
// Created by greaf on 5/5/2023.
//

#ifndef BULLETPROOF_SHOOTING_MOB_H
#define BULLETPROOF_SHOOTING_MOB_H

std::mt19937 mt(100);
std::uniform_int_distribution<int> rand_shot_num(1,3);

struct ShootingSlime : abstract_game_object, enemy_state_for_shooting_enemy {
    inline const static u8 draw_alpha = 210;
    inline const static efloat frame_duration = 1.0 / 7;
    inline const static animation
            animation_idle = animation(SS_SLIME, 0, 24, frame_duration),
            animation_attack = animation(SS_SLIME, 25, 30, frame_duration);

    int hp = 150;

    Weapon weapon;

    efloat attack_accum;
    efloat paralyzed_accum;

    animation anim = animation_idle;



    ShootingSlime(const Dot &position = Dot()) {
        // abstract_game_object
        pos = position;
        size = 0.8;
        collision_radius = 6;
        delta_draw_pos = Dot(-30, 38) * size;

        // enemy_state_for_trivial_enemy
        damage = 100;
        shooting_radius = 14;
        ddp_speed = 250;
        paralyzed_accum = paralyzed_cooldown = 0.3;
        attack_accum = attack_cooldown = 3;
    }

    [[nodiscard]] bool is_invulnerable() const {
        return paralyzed_accum < paralyzed_cooldown;
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }

    [[nodiscard]] collision_circle get_hitbox() const {
        return get_collision();
    }

    void simulate(efloat delta_time) {
        // мы парализованы и отлетаем от удара
        if (paralyzed_accum < paralyzed_cooldown) {
            paralyzed_accum += delta_time;
            simulate_move2d(pos, dp, Dot(), delta_time);
        } else {
            attack_accum += delta_time;
            anim.frame_update(delta_time);

            move_to2d(
                    pos, Players[0].pos, dp,
                    (Players[0].pos - pos).normalize() * ddp_speed, delta_time
            );

            if (
                // игрока никто не ест
                    !Players[0].is_paralyzed &&
                    // игрок не прыгает
                    !Players[0].is_jumped &&
                    // игрок не неуязвим
                    !Players[0].is_invulnerable() &&
                    // мы близко к игроку
                    (Players[0].pos - pos).get_len() <= shooting_radius &&
                    // перезарядка атаки прошла
                    attack_accum >= attack_cooldown) {


                for (int i = 0; i < rand_shot_num(mt); i++) {
                    weapon.shot(Players[0].pos);
                }
                anim = animation_idle;
            }
        }
        weapon.simulate(delta_time);
    }

    void draw() const override {
        draw_sprite(
                pos + delta_draw_pos, size, SP_SLIME_MEDIUM_SHADOW,
                shadow_pixel_func
        );

        anim.draw(pos + delta_draw_pos, size, [&](Color color) {
            return paralyzed_accum < paralyzed_cooldown
                   ? Color(0xffffff, 128)
                   : Color(color.operator unsigned int(), draw_alpha);
        });


        draw_collision(*this);

        draw_hitbox(*this);

        draw_hp(*this);

        weapon.draw(pos);

        if (global_variables::show_locator) {
            draw_circle(
                    Circle(pos - global_variables::camera.pos, shooting_radius),
                    Color(0xff0000, 50)
            );
        }
    }
};

std::vector<ShootingSlime> ShootingSlimes;

#endif //BULLETPROOF_SHOOTING_MOB_H
