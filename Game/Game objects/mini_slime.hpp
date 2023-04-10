#ifndef MINI_SLIME
#define MINI_SLIME
/*

// visibility
#define SLIME_ALPHA 210

#define SLIME_DELTA_DRAW_POS Dot(-30, 38) * size

#define SLIME_FRAME_DURATION 1.0 / 7


// animations
#define SLIME_ANIM_ATTACK animation(SS_SLIME, 25, 30, SLIME_FRAME_DURATION)
#define SLIME_ANIM_IDLE animation(SS_SLIME, 0, 24, SLIME_FRAME_DURATION)

#define SLIME_PUSH_DP 400

struct Mini_slime : abstract_game_object, enemy_state_for_trivial_enemy {
    int hp = 50;

    // pos and move
    Dot pos;
    Dot dp;
    Dot ddp;

    Dot walk_to;

    // cooldowns
    efloat attack_cooldown_accum;
    efloat paralyzed_cooldown_accum;


    // settings


    animation anim = SLIME_ANIM_IDLE;

    // boolean
    bool is_attack = false;
    bool looking = true;

    Mini_slime() {
    }

    Mini_slime(const Dot& p) {
        pos = p;


        size = 0.4;
        collision_radius = 3;
        delta_draw_pos = Dot(-8, 26) * size;

        // enemy_state_for_trivial_enemy
        damage = 25;
        jump_radius = 4;
        ddp_speed = 350;
        paralyzed_cooldown_accum = paralyzed_cooldown = 0.3;
        attack_cooldown_accum = attack_cooldown = 3;

    }

    collision_circle get_collision() const {
        return collision_circle(Circle(pos, collision_radius));
    }

    void simulate(efloat delta_time) {
        paralyzed_cooldown_accum += delta_time;
        attack_cooldown_accum += delta_time;

        if (paralyzed_cooldown_accum < paralyzed_cooldown) {
            simulate_move2d(pos, dp, ddp, delta_time);
        }
        else {
            ddp = Dot();

            if (is_attack) {
                // �������� ����� �����������
                if (anim.frame_update(delta_time)) {
                    // idle animation beginner

                    Players[0].is_paralyzed = is_attack =
                        false;  // ������� ������ �� �������� � �� ��������
                    // �����

                    attack_cooldown_accum = 0;  // cooldown reset
                    looking = true;

                    anim = SLIME_ANIM_IDLE;
                }

                // ����� ������
                if (anim.frame_count > 25 && Players[0].is_paralyzed) {
                    // push player
                    Players[0].dp = Circle(Dot(),
SLIME_PUSH_DP).get_random_dot();

                    Players[0].is_paralyzed =
                        false;  // � ������ �� ����������� �������
                    Players[0].paralyzed_cooldown_acc = 0;  // �����������

                    Players[0].hp -= damage;

                    add_hit_effect(
                        Players[0].pos + Dot(-8, 16) *
Players[0].gobj_state.size
                    );
                }
            }
            else {
                anim.frame_update(delta_time);



                {
                    move_to2d(
                        pos, Players[0].pos, dp,
                        (Players[0].pos - pos).normalize() * ddp_speed,
                        delta_time
                    );

                    // ������ ����� �� ��� � �� ������ � ������ �
                    if (!Players[0].is_paralyzed &&
                        (Players[0].pos - pos).get_len() <=
                        jump_radius &&

                        // � ����������� ����� ������ � ����������� ������ ����
                        attack_cooldown_accum >= attack_cooldown &&
                        Players[0].paralyzed_cooldown_acc >=
                        PLAYER_STATICPOS_COOLDOWN) {
                        // attack animation beginner

                        Players[0].is_paralyzed = is_attack =
                            true;  // ����� �� ����� ��������� � � ��� ��������
                        // �����

                        pos = Players[0].pos;  // ������� �� ������

                        anim = SLIME_ANIM_ATTACK;
                    }
                }
            }
        }
    }

    void draw() const {
        if (is_attack) {
            if (is_between<u8>(9, anim.frame_count, 25)) {
                draw_sprite(
                    pos + SLIME_DELTA_DRAW_POS, size,
                    SP_SLIME_LARGE_SHADOW, shadow_pixel_func
                );
            }
            else {
                draw_sprite(
                    pos + SLIME_DELTA_DRAW_POS, size,
                    SP_SLIME_MEDIUM_SHADOW, shadow_pixel_func
                );
            }
            anim.draw(pos + SLIME_DELTA_DRAW_POS, size,
alpha_pixel_func<SLIME_ALPHA>);
        }
        else {
            draw_sprite(
                pos + SLIME_DELTA_DRAW_POS, size,
                SP_SLIME_MEDIUM_SHADOW, shadow_pixel_func
            );

            anim.draw(
                pos + SLIME_DELTA_DRAW_POS, size,
                [&](Color color) {
                    return paralyzed_cooldown_accum <
                    paralyzed_cooldown
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
                Circle(p, jump_radius), Color(0xff0000, 16)
            );
        }
    }

};



*/
#endif  // MINI_SLIME