// visibility
#define BAT_DELTA_DRAW_POS Dot(-8, 26) * gobj_state.size
#define BAT_FRAME_DURATION 1.0 / 7

struct Bat {
    inline static const game_object_state gobj_state =
        game_object_state(100, 3, 0.4);

    inline static const enemy_state enemy_state =
        ::enemy_state(50, 3.5, 40, 50, 8, 300, 0.3, 5, 3, 4.5);

    // pos and move
    Dot pos;
    Dot dp;
    Dot ddp;

    Dot walk_to;

    // cool-downs
    efloat attack_cooldown_accum = enemy_state.attack_cooldown;
    efloat paralyzed_cooldown_accum = enemy_state.paralyzed_cooldown;
    efloat walk_accum = 0;

    // settings
    int hp = gobj_state.hp;

    bool looking = true;

    // animation
    animation anim = animation(SS_BAT, 0, 5, BAT_FRAME_DURATION);

    Bat() {
    }

    Bat(const Dot &p) {
        walk_to = pos = p;
        std::uniform_int_distribution<s64> random_range(1, 1000);
        walk_accum = (random_range(rnd) / 1000.0) * enemy_state.walk_sum_time;
    }

    collision_circle get_collision() const {
        return collision_circle(Circle(pos, enemy_state.collision_radius));
    }

    void simulate(efloat delta_time) {
        attack_cooldown_accum += delta_time;
        paralyzed_cooldown_accum += delta_time;

        if (paralyzed_cooldown_accum < enemy_state.paralyzed_cooldown) {
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
                if (!player.is_paralyzed &&
                    (player.pos - pos).get_len() < enemy_state.locator_radius) {
                    looking = false;
                }
            } else {
                if (player.is_paralyzed ||
                    (player.pos - pos).get_len() > enemy_state.persec_radius) {
                    looking = true;
                }
            }

            if (!looking) {
                move_to2d(
                    pos, player.pos, dp,
                    (player.pos - pos).normalize() * enemy_state.ddp_speed,
                    delta_time
                );

                // мы близко к игроку и
                if ((player.pos - pos).get_len() <= enemy_state.jump_radius &&
                    attack_cooldown_accum >= enemy_state.attack_cooldown) {
                    // hit

                    attack_cooldown_accum = 0;

                    pos = player.pos;  // прыгаем на игрока

                    player.hp -= enemy_state.damage;
                    add_hit_effect(
                        player.pos + Dot(-8, 16) * player.gobj_state.size
                    );
                }
            } else {
                // цели нет. Можем погулять

                walk_accum += delta_time;

                if (pos == walk_to) {
                    walk_to = Circle(pos, 20).get_random_dot();
                }

                if (walk_accum > enemy_state.walk_sum_time) {
                    walk_accum = 0;
                    walk_to = Circle(pos, 20).get_random_dot();
                }

                if (walk_accum < enemy_state.walk_time) {
                    move_to2d(
                        pos, walk_to, dp,
                        (walk_to - pos).normalize() * enemy_state.ddp_speed,
                        delta_time
                    );
                }
            }
        }
    }

    void draw() const {
        draw_sprite(
            pos + Dot(-5, 0) * gobj_state.size, gobj_state.size, SP_SMALL_SHADOW
        );

        anim.draw(
            pos + BAT_DELTA_DRAW_POS, gobj_state.size,
            [&](const Color &color) {
                return paralyzed_cooldown_accum < enemy_state.paralyzed_cooldown
                           ? WHITE
                           : color;
            }
        );

        draw_collision_obj(*this);

        draw_hp(*this);

        if (show_locator) {
            Dot p = pos;
            static_pos_update(p);

            draw_circle(
                Circle(p, enemy_state.persec_radius), Color(0xf000f0, 64)
            );

            draw_circle(
                Circle(p, enemy_state.locator_radius), Color(0xfff000, 32)
            );

            draw_circle(
                Circle(p, enemy_state.jump_radius), Color(0xff0000, 16)
            );

            draw_object(walk_accum, p, 0.4, 0xffffffff);

            p = walk_to;
            static_pos_update(p);

            draw_rect(p, Dot(0.3, 0.3), 0xffff0000);
        }
    }

    // вернет правду, если враг уничтожен
    void simulate_hit(const Player &player) {
        add_hit_effect(pos + BAT_DELTA_DRAW_POS);

        hp -= player.damage;

        if (hp <= 0) {
            add_death_effect(pos + Dot(-16, 22) * gobj_state.size);
        } else {
            ddp += player.get_dir() * enemy_state.ddp_speed * 1.5;
            paralyzed_cooldown_accum = 0;
        }
    }
};
