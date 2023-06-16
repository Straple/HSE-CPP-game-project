#include "weapon.hpp"
//
#include "../../Audio/audio.hpp"
#include "game_variables.hpp"

Weapon::Weapon(Dot p, weapon_t t, efloat cooldown, int damage)
    : AbstractObject(p), type(t), cooldown(cooldown), cooldown_accum(cooldown), damage(damage) {
    switch (type) {
        case GOLDEN_GUN:
            delta = Dot(19, -12);
            break;
        case STAFF:
            delta = Dot(6, -9);
            break;
        case RIFLE:
            delta = Dot(30, -22);
            break;
        case SNIPER_RIFLE:
            delta = Dot(30, -23);
            break;
    }
}

void Weapon::shot(BulletHostType bullet_host) {
    switch (type) {
        case GOLDEN_GUN:
            GoldenGun_shot(bullet_host);
            break;
        case STAFF:
            Staff_shot();
            break;
        case RIFLE:
            Rifle_shot(bullet_host);
            break;
        case SNIPER_RIFLE:
            Sniper_shot(bullet_host);
            break;
    }
}

void Weapon::draw() {
    switch (type) {
        case GOLDEN_GUN:
            GoldenGun_draw();
            break;
        case STAFF:
            Staff_draw();
            break;
        case RIFLE:
            Rifle_draw();
            break;
        case SNIPER_RIFLE:
            Sniper_draw();
            break;
    }
}

bool Weapon::may_shot() {
    return cooldown_accum >= cooldown;
}

void Weapon::simulate(efloat delta_time) {
    cooldown_accum += delta_time;
}

void Weapon::GoldenGun_shot(BulletHostType bullet_host) {
    if (!may_shot()) {
        return;
    }

    Audio::play_sound(Audio::ST_golden_gun_shot);

    cooldown_accum = 0;
    Dot shooting_pos = pos;
    if (hand == Dot(-10, 18)) {
        //        shooting_pos += Dot(2, -1);
    } else {
        shooting_pos += Dot(-2.5, -1.5);
    }
    // pos+=dulo;
    Dot dir = target - shooting_pos;
    dir = dir.normalize();
    dir += Circle(Dot(), 0.1).get_random_dot();
    game_variables::Bullets.emplace_back(bullet_host, shooting_pos + dulo, shooting_pos + dulo + dir, 1, 1000, SP_COIN);
}

void Weapon::GoldenGun_draw() {
    if (!is_picked) {
        draw_spritesheet(pos, 0.7, SS_GOLDEN_GUN, 0);
        //        draw_rect(pos + Dot(19, -12)-global_variables::camera.pos, Dot(0.5,0.5), RED);
        return;
    }
    double angle;
    angle = get_good_angle(target - pos, Dot(1, 0)) * 57.295779513;
    int ind = round(angle / 15);

    if (0 <= ind && ind <= 5) {
        hand = Dot(-10, 18);
        dulo = Dot(14, 6.5);
        Dot pivot(3.5, 3);
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind)) - dulo.y * sin((15 / 57.2957795) * (-ind));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind)) + dulo.y * cos((15 / 57.2957795) * (-ind));
        dulo = new_dulo + pivot;
    } else if (ind == 6) {
        hand = Dot(-18, 18);
        dulo = Dot(1, -6);
    } else if (20 > ind && ind >= 7) {
        hand = Dot(-18, 18);
        dulo = Dot(-5.5, -5.5);
        Dot pivot(-3.5, 4);
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-(ind - 6))) - dulo.y * sin((15 / 57.2957795) * (-(ind - 6)));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-(ind - 6))) + dulo.y * cos((15 / 57.2957795) * (-(ind - 6)));
        dulo = new_dulo + pivot;
    } else if (ind >= 20) {
        hand = Dot(-10, 18);
        dulo = Dot(15, 6);
        Dot pivot(3.5, 3);
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind + 2)) - dulo.y * sin((15 / 57.2957795) * (-ind + 2));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind + 2)) + dulo.y * cos((15 / 57.2957795) * (-ind + 2));
        dulo = new_dulo + pivot;
    }
    //    draw_rect(pos + Dot(-3.5,3) - global_variables::camera.pos, Dot(0.5,0.5), GREEN);
    //    draw_rect(pos+dulo-global_variables::camera.pos, Dot(0.5,0.5), GREEN);
    draw_spritesheet(pos + hand, 0.7, SS_GOLDEN_GUN, ind);
}

void Weapon::Staff_draw() {
    if (!is_picked) {
        draw_spritesheet(pos, 0.5, SS_STAFF, 0);
        //        draw_rect(pos + Dot(6, -9)-global_variables::camera.pos, Dot(0.5,0.5), GREEN);
        return;
    } else {
        double angle;
        angle = get_good_angle(target - pos, Dot(1, 0)) * 57.295779513;
        int ind = (angle / 90);
        if (ind == 0 || ind == 3) {
            hand = Dot(1, 14);
            ind = 0;
        } else {
            hand = Dot(-16, 14);
            ind = 1;
        }
        draw_spritesheet(pos + hand, 0.5, SS_STAFF, ind);
    }
}

void Weapon::Staff_shot() {
    if (!may_shot()) {
        return;
    }
    Audio::play_sound(Audio::ST_staff_spell);
    add_flower_dome_effect(target);
}

void Weapon::Rifle_draw() {
    if (!is_picked) {
        draw_spritesheet(pos, 0.2, SS_RIFLE, 0);
        draw_rect(pos + Dot(30, -22) - global_variables::camera.pos, Dot(0.5, 0.5), RED);
        return;
    }
    double angle;
    angle = get_good_angle(target - pos, Dot(1, 0)) * 57.295779513;
    int ind = round(angle / 15);

    if (0 <= ind && ind <= 5) {
        hand = Dot(-30, 27);
        dulo = Dot(16, 6.5);
        Dot pivot(-3.5, 4);
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind)) - dulo.y * sin((15 / 57.2957795) * (-ind));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind)) + dulo.y * cos((15 / 57.2957795) * (-ind));
        dulo = new_dulo + pivot;
    } else if (ind == 6) {
        hand = Dot(-20, 28);
        dulo = Dot(10, -17);
    } else if (20 > ind && ind >= 7) {
        hand = Dot(-20, 27);
        dulo = Dot(7, -18);
        Dot pivot(4.7, 2.5);
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-(ind - 6))) - dulo.y * sin((15 / 57.2957795) * (-(ind - 6)));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-(ind - 6))) + dulo.y * cos((15 / 57.2957795) * (-(ind - 6)));
        dulo = new_dulo + pivot;
    } else if (ind >= 20) {
        hand = Dot(-30, 26);
        dulo = Dot(14, 6.5);
        Dot pivot(-3.9, 3);
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind + 2)) - dulo.y * sin((15 / 57.2957795) * (-ind + 2));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind + 2)) + dulo.y * cos((15 / 57.2957795) * (-ind + 2));
        dulo = new_dulo + pivot;
    }
    draw_rect(pos + Dot(-3, 2) - global_variables::camera.pos, Dot(0.5, 0.5), GREEN);
    draw_rect(pos + dulo - global_variables::camera.pos, Dot(0.5, 0.5), GREEN);
    draw_spritesheet(pos + hand, 0.2, SS_RIFLE, ind);
}

void Weapon::Rifle_shot(BulletHostType bullet_host) {
    if (!may_shot()) {
        return;
    }
    Audio::play_sound(Audio::ST_rifle_shot);
    cooldown_accum = 0;
    Dot shooting_pos = pos;
    if (hand == Dot(-10, 18)) {
        //        shooting_pos += Dot(2, -1);
    } else {
        //        shooting_pos += Dot(-2.5, -1.5);
    }
    // pos+=dulo;
    Dot dir = target - shooting_pos;
    dir = dir.normalize();
    //    dir += Circle(Dot(), 0.1).get_random_dot();
    game_variables::Bullets.emplace_back(
        bullet_host, shooting_pos + dulo, shooting_pos + dulo + dir, 1, 1000, SP_RIFLE_BULLET
    );
}

void Weapon::Sniper_draw() {
    if (!is_picked) {
        draw_spritesheet(pos, 0.2, SS_SNIPER_RIFLE, 0);
        draw_rect(pos + Dot(30, -23) - global_variables::camera.pos, Dot(0.5, 0.5), RED);
        return;
    }
    double angle;
    angle = get_good_angle(target - pos, Dot(1, 0)) * 57.295779513;
    int ind = round(angle / 15);

    if (0 <= ind && ind <= 5) {
        hand = Dot(-30, 27);
        dulo = Dot(16, 4.5);
        Dot pivot(-3.5, 4);
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind)) - dulo.y * sin((15 / 57.2957795) * (-ind));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind)) + dulo.y * cos((15 / 57.2957795) * (-ind));
        dulo = new_dulo + pivot;
    } else if (ind == 6) {
        hand = Dot(-20, 28);
        dulo = Dot(8, -17);
    } else if (20 > ind && ind >= 7) {
        hand = Dot(-20, 27);
        dulo = Dot(8, -18);
        Dot pivot(4.7, 2.5);
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-(ind - 6))) - dulo.y * sin((15 / 57.2957795) * (-(ind - 6)));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-(ind - 6))) + dulo.y * cos((15 / 57.2957795) * (-(ind - 6)));
        dulo = new_dulo + pivot;
    } else if (ind >= 20) {
        hand = Dot(-30, 26);
        dulo = Dot(14, 6.5);
        Dot pivot(-3.9, 3);
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind + 2)) - dulo.y * sin((15 / 57.2957795) * (-ind + 2));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind + 2)) + dulo.y * cos((15 / 57.2957795) * (-ind + 2));
        dulo = new_dulo + pivot;
    }
    draw_rect(pos + Dot(-3, 2) - global_variables::camera.pos, Dot(0.5, 0.5), GREEN);
    draw_rect(pos + dulo - global_variables::camera.pos, Dot(0.5, 0.5), GREEN);
    draw_spritesheet(pos + hand, 0.2, SS_SNIPER_RIFLE, ind);
}

void Weapon::Sniper_shot(BulletHostType bullet_host) {
    if (!may_shot()) {
        return;
    }
    Audio::play_sound(Audio::ST_sniper_shot);
    cooldown_accum = 0;
    Dot shooting_pos = pos;
    if (hand == Dot(-10, 18)) {
        //        shooting_pos += Dot(2, -1);
    } else {
        //        shooting_pos += Dot(-2.5, -1.5);
    }
    // pos+=dulo;
    Dot dir = target - shooting_pos;
    dir = dir.normalize();
    dir += Circle(Dot(), 0.1).get_random_dot();
    game_variables::Bullets.emplace_back(
        bullet_host, shooting_pos + dulo, shooting_pos + dulo + dir, 5, 8000, SP_RIFLE_BULLET
    );
}
