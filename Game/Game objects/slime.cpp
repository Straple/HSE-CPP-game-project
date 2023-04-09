// visibility
#define SLIME_ALPHA 210
#define SLIME_DELTA_DRAW_POS Dot(-30, 38) * gobj_state.size
#define SLIME_FRAME_DURATION 1.0 / 7

// animations
#define SLIME_ANIM_ATTACK animation(SS_SLIME, 25, 30, SLIME_FRAME_DURATION)
#define SLIME_ANIM_IDLE animation(SS_SLIME, 0, 24, SLIME_FRAME_DURATION)

#define SLIME_PUSH_DP 400

struct Slime {
    inline static const game_object_state gobj_state =
        game_object_state(150, 5, 0.8);

    inline static const enemy_state enemy_state =
        ::enemy_state(100, 6, 100, 100, 8, 250, 0.3, 3, 2, 3);

    // pos and move
    Dot pos;
    Dot dp;
    Dot ddp;

    Dot walk_to;

    // cooldowns
    efloat attack_cooldown_accum = enemy_state.attack_cooldown;
    efloat paralyzed_cooldown_accum = enemy_state.paralyzed_cooldown;
    efloat walk_accum = 0;

    // settings
    int hp = gobj_state.hp;

    animation anim = SLIME_ANIM_IDLE;

    // boolean
    bool is_attack = false;
    bool looking = true;

    Slime() {
    }

    Slime(const Dot &p) {
        walk_to = pos = p;
        std::uniform_int_distribution<s64> random_range(1, 1000);
        walk_accum = (random_range(rnd) / 1000.0) * enemy_state.walk_sum_time;
    }

    collision_circle get_collision() const {
        return collision_circle(Circle(pos, enemy_state.collision_radius));
    }

    void simulate(efloat delta_time) {
        paralyzed_cooldown_accum += delta_time;
        attack_cooldown_accum += delta_time;

        if (paralyzed_cooldown_accum < enemy_state.paralyzed_cooldown) {
            simulate_move2d(pos, dp, ddp, delta_time);
        } else {
            ddp = Dot();

            if (is_attack) {
                // анимация атаки закончилась
                if (anim.frame_update(delta_time)) {
                    // idle animation beginner

                    Players[0].is_paralyzed = is_attack =
                        false;  // позиция игрока не статична и не анимация
                                // атаки

                    attack_cooldown_accum = 0;  // cooldown reset
                    looking = true;

                    anim = SLIME_ANIM_IDLE;
                }

                // шарик лопнул
                if (anim.frame_count > 25 && Players[0].is_paralyzed) {
                    // push player
                    Players[0].dp = Circle(Dot(), SLIME_PUSH_DP).get_random_dot();

                    Players[0].is_paralyzed =
                        false;  // у игрока не статическая позиция
                    Players[0].paralyzed_cooldown_acc = 0;  // перезарядка

                    Players[0].hp -= enemy_state.damage;

                    add_hit_effect(
                        Players[0].pos + Dot(-8, 16) * Players[0].gobj_state.size
                    );
                }
            } else {
                anim.frame_update(delta_time);

                if (looking) {
                    if (!Players[0].is_paralyzed && !Players[0].is_jumped &&
                        (Players[0].pos - pos).get_len() <
                            enemy_state.locator_radius) {
                        looking = false;
                    }
                } else {
                    if (Players[0].is_paralyzed || Players[0].is_jumped ||
                        (Players[0].pos - pos).get_len() >
                            enemy_state.persec_radius) {
                        looking = true;
                    }
                }

                if (!looking) {
                    move_to2d(
                        pos, Players[0].pos, dp,
                        (Players[0].pos - pos).normalize() * enemy_state.ddp_speed,
                        delta_time
                    );

                    // игрока никто не ест и мы близко к игроку и
                    if (!Players[0].is_paralyzed &&
                        (Players[0].pos - pos).get_len() <=
                            enemy_state.jump_radius &&

                        // и перезарядка атаки прошла и перезарядка игрока тоже
                        attack_cooldown_accum >= enemy_state.attack_cooldown &&
                        Players[0].paralyzed_cooldown_acc >=
                            PLAYER_STATICPOS_COOLDOWN) {
                        // attack animation beginner

                        Players[0].is_paralyzed = is_attack =
                            true;  // игрок не может двигаться и у нас анимация
                                   // атаки

                        pos = Players[0].pos;  // прыгаем на игрока

                        anim = SLIME_ANIM_ATTACK;
                    }
                } else {
                    // цели нет. Можем погулять
                    looking = true;

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
    }

    void draw() const {
        if (is_attack) {
            if (is_between<u8>(9, anim.frame_count, 25)) {
                draw_sprite(
                    pos + SLIME_DELTA_DRAW_POS, gobj_state.size,
                    SP_SLIME_LARGE_SHADOW, shadow_pixel_func
                );
            } else {
                draw_sprite(
                    pos + SLIME_DELTA_DRAW_POS, gobj_state.size,
                    SP_SLIME_MEDIUM_SHADOW, shadow_pixel_func
                );
            }
            anim.draw(pos + SLIME_DELTA_DRAW_POS, gobj_state.size, alpha_pixel_func<SLIME_ALPHA>);
        } else {
            draw_sprite(
                pos + SLIME_DELTA_DRAW_POS, gobj_state.size,
                SP_SLIME_MEDIUM_SHADOW, shadow_pixel_func
            );

            anim.draw(
                pos + SLIME_DELTA_DRAW_POS, gobj_state.size,
                [&](Color color) {
                    return paralyzed_cooldown_accum <
                                   enemy_state.paralyzed_cooldown
                               ? WHITE
                               : Color(
                                     color.operator unsigned int(), SLIME_ALPHA
                                 );
                }
            );
        }

        draw_collision_obj(*this);

        draw_hp(*this);

        if (global_variables::show_locator) {
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

    // for sword
    /*void simulate_hit(const Player &player) {
        if (is_attack) {
            return;
        }

        add_hit_effect(pos + Dot(-8, 8) * gobj_state.size);

        hp -= player.damage;

        if (hp <= 0) {
            add_death_effect(pos + Dot(-15, 15) * gobj_state.size);
            Loot_objects.push_back(std::unique_ptr<Loot>(new Heart(this->get_collision().circle.pos)));
        } else {
            ddp += player.get_dir() * enemy_state.ddp_speed * 1.5;
            paralyzed_cooldown_accum = 0;
        }
    }*/
};

std::vector<Slime> Slimes;