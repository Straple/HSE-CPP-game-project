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
        locator_radius = 40;
        pursuit_radius = 50;
        jump_radius = 8;
        ddp_speed = 300;
        paralyzed_cooldown_accum = paralyzed_cooldown = 0.3;
        attack_cooldown_accum = attack_cooldown = 3;
        walk_time = 3;
        walk_sum_time = 4.5;

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

            if (looking) {
                if (!Players[0].is_paralyzed &&
                    (Players[0].pos - pos).get_len() < locator_radius) {
                    looking = false;
                }
            } else {
                if (Players[0].is_paralyzed ||
                    (Players[0].pos - pos).get_len() > pursuit_radius) {
                    looking = true;
                }
            }

            if (!looking) {
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
            } else {
                // цели нет. Можем погулять

                walk_accum += delta_time;

                if (pos == walk_to) {
                    walk_to = Circle(pos, 20).get_random_dot();
                }

                if (walk_accum > walk_sum_time) {
                    walk_accum = 0;
                    walk_to = Circle(pos, 20).get_random_dot();
                }

                if (walk_accum < walk_time) {
                    move_to2d(
                        pos, walk_to, dp,
                        (walk_to - pos).normalize() * ddp_speed, delta_time
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

            draw_circle(Circle(p, pursuit_radius), Color(0xf000f0, 64));

            draw_circle(Circle(p, locator_radius), Color(0xfff000, 32));

            draw_circle(Circle(p, jump_radius), Color(0xff0000, 16));

            draw_object(walk_accum, p, 0.4, 0xffffffff);

            p = walk_to;
            static_pos_update(p);

            draw_rect(p, Dot(0.3, 0.3), 0xffff0000);
        }
    }

    // вернет правду, если враг уничтожен
    void simulate_hit(const Player &player) {
        add_hit_effect(pos + Dot(-8, 26) * size);

        hp -= player.damage;

        if (hp <= 0) {
            add_death_effect(pos + Dot(-16, 22) * size);
        } else {
            ddp += player.get_dir() * ddp_speed * 1.5;
            paralyzed_cooldown_accum = 0;
        }
    }
};

std::vector<Bat> Bats;

#endif  // GAME_BAT_HPP