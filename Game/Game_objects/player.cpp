#include "player.hpp"
#include "game_variables.hpp"

[[nodiscard]] unsigned int Player_anim_tree::anim_and_dir_t::get_num() const {
    return (static_cast<unsigned int>(anim) - 1) * 4 + (static_cast<unsigned int>(dir) - 1);
}

// возвращает анимацию
Player_anim_tree::anim_and_dir_t Player_anim_tree::get_anim(Dot dir, anim_and_dir_t type) {
    // дефолтная анимация
    if (type.anim == NONE) {
        return {IDLE, direction_t::RIGHT};
    }
    // run
    else if (dir.x < 0) {
        return {RUN, direction_t::LEFT};
    } else if (dir.x > 0) {
        return {RUN, direction_t::RIGHT};
    } else if (dir.y < 0) {
        return {RUN, direction_t::DOWN};
    } else if (dir.y > 0) {
        return {RUN, direction_t::UP};
    }
    // idle
    else {
        return {IDLE, type.dir};
    }
}

Player::Player(const Dot &position) {
    pos = position;

    weapon.cooldown = 0;  // мы боги
}

[[nodiscard]] bool Player::is_dead() const {
    return hp <= 0;
}

void Player::die() {
    hp = 0;
    set_invulnerable();
    add_death_effect(pos);
    // Сразу сделаем методом, чтобы сюда занести анимации и тд
}

void Player::reborn() {
    // Аналогично, можно не только для возрождения но и для появления/телепортации использовать
    set_invulnerable();
    hp = 1;
}

[[nodiscard]] bool Player::is_invulnerable() const {
    return invulnerable_accum > 0;
}

void Player::set_invulnerable() {
    invulnerable_accum = invulnerable_cooldown;
}

void Player::simulate(efloat delta_time, Dot ddp, bool pressed_jump) {
    jump_accum -= delta_time;

    if (is_paralyzed) {
        // игрока ест слайм
        anim_type.anim = Player_anim_tree::IDLE;
        anim = player_anims[anim_type.get_num()];
    } else if (is_jumped) {
        Dot dp_tmp = dp;
        simulate_move2d(pos, dp_tmp, Dot(), delta_time);

        if (anim.frame_update(delta_time)) {
            is_jumped = false;
            jump_accum = jump_cooldown;

            anim_type.anim = Player_anim_tree::IDLE;

            anim = player_anims[anim_type.get_num()];
        }
    } else if (pressed_jump && jump_accum <= 0) {
        is_jumped = true;

        anim_type.anim = Player_anim_tree::ROLL;
        anim = player_anims[anim_type.get_num()];

        Dot jump_dir;
        if (ddp.get_len() == 0) {
            jump_dir = get_dir();
        } else {
            jump_dir = ddp.normalize();
        }
        dp = jump_dir * jump_speed;
    } else {
        if (!is_dead()) {
            invulnerable_accum -= delta_time;
            weapon.simulate(delta_time, cursor_dir + pos);
        }

        // simulate move
        {
            ddp *= ddp_speed;
            simulate_move2d(pos, dp, ddp, delta_time);
        }

        // текущая анимация+
        auto current_anim = Player_anim_tree::get_anim(ddp.normalize(), anim_type);

        if (anim_type.get_num() != current_anim.get_num()) {  // у нас сменились анимации

            anim_type = current_anim;

            anim = player_anims[current_anim.get_num()];

            // начинаем с 1 кадра, чтобы мы сразу начинали движение
            anim.frame_cur_count = std::min<unsigned int>(1, anim.frame_count - 1);
        }

        anim.frame_update(delta_time);
    }
}

void Player::draw() const {
    draw_sprite(pos + Dot(-8, 3), size, SP_MEDIUM_SHADOW);

    anim.sprite_sheet = SS_PLAYER_CLOACK;
    anim.draw(pos + delta_draw_pos, size, [&](const Color &color) {
        if (invulnerable_accum > 0 || is_dead()) {
            return Color(0xffffff, 128);
        } else if (color == WHITE) {
            return customization_colors[2 * cloack_color_id];
        } else {
            return customization_colors[2 * cloack_color_id + 1];
        }
    });
    anim.sprite_sheet = SS_PLAYER_T_SHIRT;
    anim.draw(pos + delta_draw_pos, size, [&](const Color &color) {
        if (invulnerable_accum > 0 || is_dead()) {
            return Color(0xffffff, 128);
        } else if (color == WHITE) {
            return customization_colors[2 * t_shirt_color_id];
        } else {
            return customization_colors[2 * t_shirt_color_id + 1];
        }
    });
    anim.sprite_sheet = SS_PLAYER_BODY;
    anim.draw(pos + delta_draw_pos, size, [&](const Color &color) {
        if (invulnerable_accum > 0 || is_dead()) {
            return Color(0xffffff, 128);
        } else {
            return color;
        }
    });

    if (!now_is_customization && !is_jumped && !is_dead()) {
        weapon.draw(pos, cursor_dir + pos);
    }

    draw_collision(*this);
    draw_hitbox(*this);
}

// возвращает направление анимации игрока
[[nodiscard]] Dot Player::get_dir() const {
    return get_direction(anim_type.dir);
}

[[nodiscard]] std::unique_ptr<Collision> Player::get_hitbox() const {
    return std::make_unique<CollisionBox>(pos + Dot(-7, 20), pos + Dot(7, -5));
}

[[nodiscard]] std::unique_ptr<Collision> Player::get_collision() const {
    return std::make_unique<CollisionCircle>(pos, collision_radius);
}

// найти индекс в game_variables:: Players, у которого такой client_id
// вернет -1, если такого не нашли
int find_player_index(int client_id) {
    for (std::size_t index = 0; index < game_variables::game_variables::Players.size(); index++) {
        if (game_variables::Players[index].client_id == client_id) {
            return index;
        }
    }
    return -1;
}

// найти самого близкого игрока к этой точке
// вернет index
int find_nearest_player_index(Dot pos) {
    int best = -1;
    for (std::size_t index = 0; index < game_variables::Players.size(); index++) {
        if (game_variables::Players[index].is_dead() || game_variables::Players[index].is_paralyzed) {
            continue;
        }
        if (best == -1 || (game_variables::Players[index].pos - pos).get_len() <
                              (game_variables::Players[best].pos - pos).get_len()) {
            best = index;
        }
    }
    return best;
}

// моб стоит в pos
// мы должны найти лучшую для него цель
// вернет client_id, а не index, для более безопасной работы
int find_best_player(Dot pos) {
    // return find_nearest_player(pos);
    int best = -1;
    for (std::size_t index = 0; index < game_variables::Players.size(); index++) {
        if (game_variables::Players[index].is_dead() || game_variables::Players[index].is_paralyzed) {
            continue;
        }
        if (best == -1 || (game_variables::Players[index].pos - pos).get_len() <
                              (game_variables::Players[best].pos - pos).get_len()) {
            best = index;
        }
    }
    if (best == -1) {
        return -1;
    }
    return game_variables::Players[best].client_id;
}
