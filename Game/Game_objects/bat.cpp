#include "bat.hpp"
//
#include "game_variables.hpp"
#include "player.hpp"

Bat::Bat(const Dot &position) {
    pos = position;
    // чтобы разнообразить кучу летучих мышей, которые будут иметь
    // одновременные анимации
    anim.frame_cur_count = get_random_engine()() % 5;
}

[[nodiscard]] bool Bat::is_invulnerable() const {
    return paralyzed_accum > 0;
}

[[nodiscard]] std::unique_ptr<Collision> Bat::get_collision() const {
    return std::make_unique<CollisionCircle>(pos, collision_radius);
}

[[nodiscard]] std::unique_ptr<Collision> Bat::get_hitbox() const {
    return std::make_unique<CollisionCircle>(pos + Dot(0, 13), 5);
}

void Bat::simulate(efloat delta_time, const std::set<grid_pos_t> &visitable_grid_dots) {
    attack_accum -= delta_time;
    paralyzed_accum -= delta_time;

    if (paralyzed_accum > 0) {
        simulate_move2d(pos, dp, Dot(), delta_time);
    } else {
        anim.frame_update(delta_time);

        // чтобы летучая мышь не дергалась туда-сюда
        if (dp.x < -15) {
            anim.sprite_sheet = SS_BAT_INVERSE;
        } else if (dp.x > 15) {
            anim.sprite_sheet = SS_BAT;
        }

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
                // игрок не прыгает
                !player.is_jumped &&
                // игрок не неуязвим
                !player.is_invulnerable() &&
                // мы близко к игроку
                (player.pos - pos).get_len() <= jump_radius &&
                // перезарядка атаки прошла
                attack_accum <= 0) {
            // hit
            attack_accum = attack_cooldown;
            pos = player.pos;  // прыгаем на игрока
            player.hp -= damage;
            player.set_invulnerable();
            add_hit_effect(player.pos);
            if (player.hp <= 0) {
                player.die();
            }
        }
    }
}

void Bat::draw() const {
    draw_sprite(pos + Dot(-5, 0) * size, size, SP_SMALL_SHADOW);

    anim.draw(pos + delta_draw_pos, size, [&](const Color &color) {
        return paralyzed_accum > 0 ? Color(0xffffff, 128) : color;
    });

    draw_collision(*this);
    draw_hitbox(*this);
    draw_hp(*this);
}
