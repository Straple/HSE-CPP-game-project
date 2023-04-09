#ifndef GAME_BAT_HPP
#define GAME_BAT_HPP

struct Bat : abstract_game_object, enemy_state_for_trivial_enemy {
    int hp = 100;

    Dot ddp;

    Dot walk_to;

    efloat attack_cooldown_accum;
    efloat paralyzed_cooldown_accum;
    efloat walk_accum = 0;

    bool looking = true;

    // animation
    animation anim = animation(SS_BAT, 0, 5, 1.0 / 7);

    Bat(const Dot &position = Dot()) {
        // abstract_game_object
        pos = position;
        size = 0.4;
        collision_radius = 3.5;
        delta_draw_pos = Dot(-8, 26) * size;

        // enemy_state_for_trivial_enemy
        damage = 50;
        jump_radius = 8;
        ddp_speed = 300;
        paralyzed_cooldown_accum = paralyzed_cooldown = 0.3;
        attack_cooldown_accum = attack_cooldown = 3;
        // Bat
        walk_to = position;
    }

    [[nodiscard]] collision_circle get_collision() const override {
        return collision_circle(Circle(pos, collision_radius));
    }

    void simulate(efloat delta_time) {
        attack_cooldown_accum += delta_time;
        paralyzed_cooldown_accum += delta_time;

        if (paralyzed_cooldown_accum < paralyzed_cooldown) {
            simulate_move2d(pos, dp, ddp, delta_time);
        } else {
            ddp = Dot();

            anim.frame_update(delta_time);

            if (dp.x < 0) {
                anim.sprite_sheet = SS_BAT_INVERSE;
            } else {
                anim.sprite_sheet = SS_BAT;
            }
            {
                move_to2d(
                    pos, Players[0].pos, dp,
                    (Players[0].pos - pos).normalize() * ddp_speed, delta_time
                );

                // мы близко к игроку и
                if ((Players[0].pos - pos).get_len() <= jump_radius &&
                    attack_cooldown_accum >= attack_cooldown) {
                    // hit

                    attack_cooldown_accum = 0;

                    pos = Players[0].pos;  // прыгаем на игрока

                    Players[0].hp -= damage;
                    add_hit_effect(
                        Players[0].pos +
                        Dot(-8, 16) * Players[0].gobj_state.size
                    );
                }
            }
        }
    }

    void draw() const override {
        draw_sprite(pos + Dot(-5, 0) * size, size, SP_SMALL_SHADOW);

        anim.draw(pos + delta_draw_pos, size, [&](const Color &color) {
            return paralyzed_cooldown_accum < paralyzed_cooldown ? WHITE
                                                                 : color;
        });

        draw_collision_obj(*this);

        draw_hp(*this);

        if (global_variables::show_locator) {
            Dot p = pos;
            static_pos_update(p);
            draw_circle(Circle(p, jump_radius), Color(0xff0000, 16));

        }
    }

 
};

std::vector<Bat> Bats;

#endif  // GAME_BAT_HPP