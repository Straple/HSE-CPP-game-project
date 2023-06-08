#include "weapon.hpp"
//
#include "game_variables.hpp"
#include "../../render.hpp"
#include "../../sprites.hpp"


Weapon::Weapon(Dot p, weapon_t t, efloat cooldown, int damage) : AbstractObject(p), type(t), cooldown(cooldown), cooldown_accum(cooldown), damage(damage) {
}



void Weapon::shot(BulletHostType bullet_host, sprite_t sprite) {
    switch(type) {
        case GOLDEN_GUN:
            GoldenGun_shot(bullet_host, sprite);
            break;
        case STAFF:
            Staff_shot();
            break;
    }
}
void Weapon::draw() {
    switch(type) {
        case GOLDEN_GUN:
            GoldenGun_draw();
            break;
        case STAFF:
            Staff_draw();
            break;
    }
}

bool Weapon::may_shot() {
    return cooldown_accum >= cooldown;
}

void Weapon::simulate(efloat delta_time) {
    cooldown_accum += delta_time;
}

void Weapon::GoldenGun_shot(BulletHostType bullet_host, sprite_t sprite) {
    if (!may_shot()) {
        return;
    }
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
    game_variables::Bullets.emplace_back(bullet_host, shooting_pos + dulo, shooting_pos + dulo + dir, 1, 1000, sprite);
}

void Weapon::GoldenGun_draw() {
    if (!is_picked) {
        draw_spritesheet(pos, 0.7, SS_GOLDEN_GUN, 0);
        return;
    }
    double angle;
    angle = get_good_angle(target - pos, Dot(1, 0)) * 57.295779513;
    int ind = ceil(angle / 15);

    if (0 <= ind && ind <= 5) {
        hand = Dot(-10, 18);
        dulo = Dot(14, 6.5);
        Dot pivot(3.5,3);
        dulo-=pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind)) - dulo.y * sin((15 / 57.2957795) * (-ind));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind)) + dulo.y * cos((15 / 57.2957795) * (-ind));
        dulo = new_dulo+pivot;
    } else if (ind == 6) {
        hand = Dot(-18, 18);
        dulo = Dot(1, -6);
    } else if (20 > ind && ind >= 7) {
        hand = Dot(-18, 18);
        dulo = Dot(-5.5, -5.5);
        Dot pivot(-3.5, 4);
        dulo-=pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-(ind - 6))) - dulo.y * sin((15 / 57.2957795) * (-(ind - 6)));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-(ind - 6))) + dulo.y * cos((15 / 57.2957795) * (-(ind - 6)));
        dulo = new_dulo+pivot;
    } else if (ind >= 20) {
        hand = Dot(-10, 18);
        dulo = Dot(15, 6);
        Dot pivot(3.5,3);
        dulo-=pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind + 2)) - dulo.y * sin((15 / 57.2957795) * (-ind + 2));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind + 2)) + dulo.y * cos((15 / 57.2957795) * (-ind + 2));
        dulo = new_dulo+pivot;
    }
    draw_rect(pos + Dot(-3.5,3) - global_variables::camera.pos, Dot(0.5,0.5), GREEN);
    draw_rect(pos+dulo-global_variables::camera.pos, Dot(0.5,0.5), GREEN);
    draw_spritesheet(pos + hand, 0.7, SS_GOLDEN_GUN, ind);
}

void Weapon::Staff_draw() {
    if (!is_picked) {
        draw_spritesheet(pos, 0.5, SS_STAFF, 0);
        return;
    }
    else {
        double angle;
        angle = get_good_angle(target - pos, Dot(1, 0)) * 57.295779513;
        int ind = (angle / 90);
        if (ind == 0 || ind == 3) {
            hand = Dot(1, 14);
            ind = 0;
        }
        else {
            hand = Dot(-16,14);
            ind = 1;
        }
        draw_spritesheet(pos + hand, 0.5, SS_STAFF, ind);
    }
}

void Weapon::Staff_shot() {
    if (!may_shot()) {
        return;
    }
    add_flower_dome_effect(target);
}
