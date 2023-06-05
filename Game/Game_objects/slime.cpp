#include "slime.hpp"

    Slime::Slime(const Dot &position) {
        pos = position;

        // чтобы разнообразить кучу мобов, которые будут иметь одновременные
        // анимации
        anim.frame_cur_count = get_random_engine()() % 24;
    }

    [[nodiscard]] bool Slime::is_invulnerable() const {
        return is_devour || paralyzed_accum > 0;
    }

    [[nodiscard]] std::unique_ptr<Collision> Slime::get_collision() const {
        return std::make_unique<CollisionCircle>(pos, collision_radius);
    }

    [[nodiscard]] std::unique_ptr<Collision> Slime::get_hitbox() const {
        return get_collision();
    }

    bool Slime::is_reachable(Dot from, Dot to, std::vector<CollisionBox> &Walls) {
        Dot dir = (to - from).normalize();
        while ((to - from).get_len() >= 2) {
            for (auto &wall : Walls) {
                if (wall.trigger(from)) {
                    return false;
                }
            }
            from += dir;
        }
        return true;
    }

    void Slime::simulate(
            const efloat delta_time,
            const std::set<grid_pos_t> &visitable_grid_dots,
            std::vector<CollisionBox> &Walls
    ) {
        paralyzed_accum -= delta_time;
        shot_accum -= delta_time;
        devour_accum -= delta_time;

        // мы парализованы и отлетаем от удара
        if (paralyzed_accum > 0) {
            simulate_move2d(pos, dp, Dot(), delta_time);
        } else if (is_devour) {  // мы едим игрока
            int index = find_player_index(target_client_id);

            if (index == -1) {
                // этот клиент вышел из игры

                is_devour = false;
                anim = animation_idle;
                anim.frame_cur_count = get_random_engine()() % 24;
                devour_accum = devour_cooldown;
                return;
            }

            auto &player = game_variables::Players[index];

            // анимация атаки закончилась
            if (anim.frame_update(delta_time)) {
                is_devour = false;
                anim = animation_idle;
                anim.frame_cur_count = get_random_engine()() % 24;
                devour_accum = devour_cooldown;
                dp = Dot();

                // так как мы ели игрока, то почему бы нам не получить доп хп
                hp += 2;
            }

            // шарик лопнул
            if ((anim.frame_cur_count > 25 || !is_devour) && player.is_paralyzed) {
                // выплюнуть игрока
                player.dp = Circle(Dot(), 500).get_random_dot();
                player.is_paralyzed = false;
                player.hp -= devour_damage;
                player.set_invulnerable();
                add_hit_effect(player.pos);
                if (player.hp <= 0) {
                    player.die();
                }
            }
        } else if (is_shooting) {
            int index = find_player_index(target_client_id);
            if (index == -1) {
                // этот клиент вышел из игры
                shot_accum = shot_cooldown;
                is_shooting = false;
                anim = animation_idle;
                anim.frame_cur_count = get_random_engine()() % 24;
                dp = Dot();
                return;
            }

            if (anim.frame_update(delta_time)) {
                shot_accum = shot_cooldown;
                is_shooting = false;
                anim = animation_idle;
                anim.frame_cur_count = get_random_engine()() % 24;
                dp = Dot();

                Dot bullet_pos = pos + Dot(0, 15);
                game_variables::Bullets.emplace_back(BulletHostType::ENEMY, bullet_pos, game_variables::Players[index].pos, 1, 1000, SP_SLIME_BULLET);
            }
        } else {
            anim.frame_update(delta_time);

            update_target(delta_time);

            int index = find_player_index(target_client_id);
            if (index == -1) {
                return;  // нет игроков
            }
            auto &player = game_variables::Players[index];

            update_move_dir(delta_time, player.pos, visitable_grid_dots);
            // move_dir уже нормализован в get_direction_to_shortest_path
            simulate_move_to2d(pos, pos + move_dir_to_target, dp, move_dir_to_target * ddp_speed, delta_time);

            if (
                // игрока никто не ест
                    !player.is_paralyzed &&
                    // перезарядка выстрела прошла
                    shot_accum <= 0 &&
                    // мы можем попасть по нему
                    is_reachable(pos + Dot(0, 15), player.pos, Walls)
                    ) {
                anim = animation_shot;
                is_shooting = true;
            } else if (
                // игрока никто не ест
                    !player.is_paralyzed &&
                    // игрок не прыгает
                    !player.is_jumped &&
                    // игрок не неуязвим
                    !player.is_invulnerable() &&
                    // мы близко к игроку
                    (player.pos - pos).get_len() <= jump_radius &&
                    // перезарядка атаки прошла
                    devour_accum <= 0
                    ) {
                // игрок не может двигаться и у нас анимация атаки
                player.is_paralyzed = is_devour = true;
                pos = player.pos;  // прыгаем на игрока
                anim = animation_devour;
            }
        }
    }

    void Slime::draw() const {
        if (is_devour) {
            if (is_between<uint8_t>(9, anim.frame_count, 25)) {
                draw_sprite(pos + delta_draw_pos, size, SP_SLIME_LARGE_SHADOW, shadow_color_func());
            } else {
                draw_sprite(pos + delta_draw_pos, size, SP_SLIME_MEDIUM_SHADOW, shadow_color_func());
            }
            anim.draw(pos + delta_draw_pos, size, alpha_color_func<draw_alpha>());
        } else {
            draw_sprite(pos + delta_draw_pos, size, SP_SLIME_MEDIUM_SHADOW, shadow_color_func());

            anim.draw(pos + delta_draw_pos, size, [&](Color color) {
                return paralyzed_accum > 0 ? Color(0xffffff, 128) : Color(static_cast<uint32_t>(color), draw_alpha);
            });
        }

        draw_collision(*this);
        draw_hitbox(*this);
        draw_hp(*this);
    }
