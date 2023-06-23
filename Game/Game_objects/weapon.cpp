#include "weapon.hpp"
//
#include "game_variables.hpp"
#include "../../Audio/audio.hpp"


Weapon::Weapon(Dot p, weapon_t t, efloat cooldown, int damage) : AbstractObject(p), type(t), cooldown(cooldown), cooldown_accum(cooldown), damage(damage) {
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
    switch(type) {
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
    switch(type) {
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
//    Dot shooting_pos = pos;
    // pos+=dulo;
    Dot dir = target - (pos + dulo);
    dir = dir.normalize();
//    dir += Circle(Dot(), 0.1).get_random_dot();
    game_variables::Bullets.emplace_back(bullet_host, pos + dulo, pos + dulo + dir, 1, 1000, SP_COIN);
}

void Weapon::GoldenGun_draw() {
    if (!is_picked) {
        draw_spritesheet(pos, 0.7, SS_GOLDEN_GUN, 0);
//        draw_rect(pos + Dot(19, -12)-global_variables::camera.pos, Dot(0.5,0.5), RED);
        return;
    }
    double angle;
    angle = get_good_angle(target - pos, Dot(1, 0)) * 57.295779513;
    int half, ind = (angle / 90);
    if (ind == 0 || ind == 3) {
        half = 0;
    }
    else {
        half = 1;
    }
    if (half == 0) {
        hand = Dot(-10, 18);
        dulo = Dot(14, 7.5);
        Dot pivot(5,3.3);
        draw_rect(pos+pivot-global_variables::camera.pos, Dot(1,1), GREEN);
        draw_rect(pos+dulo-global_variables::camera.pos, Dot(0.5,0.5), GREEN);
        angle = get_good_angle(target - pos - pivot, Dot(1, 0)) * 57.295779513;
        ind = static_cast<int>(round(angle/15));
        if (ind <= 7) {
            ind = std::min(ind, 6);
        }
        if (ind > 7) {
            ind = std::max(ind, 19);
        }
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind)) - dulo.y * sin((15 / 57.2957795) * (-ind));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind)) + dulo.y * cos((15 / 57.2957795) * (-ind));
        dulo = new_dulo + pivot;
        draw_rect(pos + dulo - global_variables::camera.pos, Dot(1,1), RED);
        if (ind == 6) {
            dulo.y +=1;
        }
        if (ind > 6) {
            ind += 2;
        }
    }
    else {
        hand = Dot(-18, 18);
        dulo = Dot(-7, -4.5);
        Dot pivot(-3, 4);
        draw_rect(pos+dulo-global_variables::camera.pos, Dot(0.5,0.5), GREEN);
        draw_rect(pos+pivot-global_variables::camera.pos, Dot(1,1), GREEN);
        angle = get_good_angle(target - pos - pivot, Dot(1, 0)) * 57.295779513;
        ind = static_cast<int>(round(angle/15));
        ind-=1;
        if (angle <= 180) {
            ind = std::max(ind, 7);
        }
        dulo-=pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-(ind-7))) - dulo.y * sin((15 / 57.2957795) * (-(ind-7)));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-(ind-7))) + dulo.y * cos((15 / 57.2957795) * (-(ind-7)));
        dulo = new_dulo+pivot;
        draw_rect(pos + dulo - global_variables::camera.pos, Dot(1,1), RED);
    }
    if (ind == 26) ind = 0;
    draw_rect(pos+dulo-global_variables::camera.pos, Dot(0.5,0.5), GREEN);
    draw_spritesheet(pos + hand, 0.7, SS_GOLDEN_GUN, ind);
}

void Weapon::Staff_draw() {
    if (!is_picked) {
        draw_spritesheet(pos, 0.5, SS_STAFF, 0);
//        draw_rect(pos + Dot(6, -9)-global_variables::camera.pos, Dot(0.5,0.5), GREEN);
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
    Audio::play_sound(Audio::ST_staff_spell);
    add_flower_dome_effect(target);
}

void Weapon::Rifle_draw() {
    if (!is_picked) {
        draw_spritesheet(pos, 0.2, SS_RIFLE, 0);
        draw_rect(pos + Dot(30, -22)-global_variables::camera.pos, Dot(0.5,0.5), RED);
        return;
    }
    double angle;
    angle = get_good_angle(target - pos, Dot(1, 0)) * 57.295779513;
    int half, ind = (angle / 90);
    if (ind == 0 || ind == 3) {
        half = 0;
    }
    else {
        half = 1;
    }
    if (half == 0) {
        hand = Dot(-30, 27);
        dulo = Dot(14, 6.7);
        Dot pivot(-4.2, 2);
        draw_rect(pos+pivot-global_variables::camera.pos, Dot(1,1), GREEN);
        draw_rect(pos+dulo-global_variables::camera.pos, Dot(0.5,0.5), GREEN);
        angle = get_good_angle(target - pos - pivot, Dot(1, 0)) * 57.295779513;
        ind = static_cast<int>(round(angle/15));
        if (angle <= 90) {
            ind = std::min(ind, 5);
        }
        if (ind > 5) {
            ind = std::max(ind, 17);
        }
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind)) - dulo.y * sin((15 / 57.2957795) * (-ind));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind)) + dulo.y * cos((15 / 57.2957795) * (-ind));
        dulo = new_dulo + pivot;
        if (ind == 5) {
            dulo += Dot(-1,-1);
        }
        draw_rect(pos + dulo - global_variables::camera.pos, Dot(1,1), RED);
        if (ind > 5) {
            ind-=2;
        }
    }
    else {
        hand = Dot(-20, 27);
        dulo = Dot(1, -17);
        Dot pivot(5.5,2);
        draw_rect(pos+dulo-global_variables::camera.pos, Dot(0.5,0.5), GREEN);
        draw_rect(pos+pivot-global_variables::camera.pos, Dot(1,1), GREEN);
        angle = get_good_angle(target - pos - pivot, Dot(1, 0)) * 57.295779513;
        ind = static_cast<int>(round(angle/15));
        ind-=1;
        if (angle <= 180) {
            ind = std::max(ind, 6);
        }
        dulo-=pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-(ind-5))) - dulo.y * sin((15 / 57.2957795) * (-(ind-5)));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-(ind-5))) + dulo.y * cos((15 / 57.2957795) * (-(ind-5)));
        dulo = new_dulo+pivot;
        draw_rect(pos + dulo - global_variables::camera.pos, Dot(1,1), RED);

    }
//    std::cout << ind << ' ' << angle <<  std::endl;
    if (ind == 22) ind = 0;
    draw_spritesheet(pos + hand, 0.2, SS_RIFLE, ind);
}
void Weapon::Rifle_shot(BulletHostType bullet_host) {
    if (!may_shot()) {
        return;
    }
    Audio::play_sound(Audio::ST_rifle_shot);

    cooldown_accum = 0;

    Dot dir = target - (pos+dulo);
    dir = dir.normalize();
    dir += Circle(Dot(), 0.1).get_random_dot();
    game_variables::Bullets.emplace_back(bullet_host, pos + dulo, pos + dulo + dir, 1, 1000, SP_RIFLE_BULLET);
}

void Weapon::Sniper_draw() {
    if (!is_picked) {
        draw_spritesheet(pos, 0.2, SS_SNIPER_RIFLE, 0);
        draw_rect(pos + Dot(30, -23)-global_variables::camera.pos, Dot(0.5,0.5), RED);
        return;
    }
    double angle;
//    draw_rect(pos+Dot(0,2.5)-global_variables::camera.pos, Dot(0.5,0.5), RED);
    angle = get_good_angle(target - pos - Dot(0,5), Dot(1, 0)) * 57.295779513;
    int half, ind = (angle / 90);
    if (ind == 0 || ind == 3) {
        half = 0;
    }
    else {
        half = 1;
    }
    if (half == 0) {
        hand = Dot(-30, 27);
        dulo = Dot(14, 5);
        Dot pivot(-4.5, 2);
        draw_rect(pos+pivot-global_variables::camera.pos, Dot(1,1), GREEN);
        draw_rect(pos+dulo-global_variables::camera.pos, Dot(0.5,0.5), GREEN);
        angle = get_good_angle(target - pos - pivot, Dot(1, 0)) * 57.295779513;
        ind = static_cast<int>(round(angle/15));
        if (angle <= 90) {
            ind = std::min(ind, 5);
        }
        if (ind > 5) {
            ind = std::max(ind, 17);
        }
        dulo -= pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind)) - dulo.y * sin((15 / 57.2957795) * (-ind));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind)) + dulo.y * cos((15 / 57.2957795) * (-ind));
        dulo = new_dulo + pivot;
        draw_rect(pos + dulo - global_variables::camera.pos, Dot(1,1), RED);
        if (ind > 5) {
            ind-=2;
        }
    }
    else {
        hand = Dot(-20, 27);
        dulo = Dot(3.5, -15);
        draw_rect(pos+dulo-global_variables::camera.pos, Dot(0.5,0.5), GREEN);
        Dot pivot(5,3);
        draw_rect(pos+pivot-global_variables::camera.pos, Dot(2,2), GREEN);
        angle = get_good_angle(target - pos - pivot, Dot(1, 0)) * 57.295779513;
        ind = static_cast<int>(round(angle/15));
        ind-=1;
        if (angle <= 180) {
            ind = std::max(ind, 6);
        }
        dulo-=pivot;
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-(ind-5))) - dulo.y * sin((15 / 57.2957795) * (-(ind-5)));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-(ind-5))) + dulo.y * cos((15 / 57.2957795) * (-(ind-5)));
        dulo = new_dulo+pivot;
        draw_rect(pos + dulo - global_variables::camera.pos, Dot(1,1), RED);

    }
//    std::cout << ind << ' ' << angle <<  std::endl;
    if (ind == 22) ind = 0;
    draw_spritesheet(pos + hand, 0.2, SS_SNIPER_RIFLE, ind);
}

void Weapon::Sniper_shot(BulletHostType bullet_host) {
    if (!may_shot()) {
        return;
    }
    Audio::play_sound(Audio::ST_sniper_shot);
    cooldown_accum = 0;
    Dot dir = target - (pos+dulo);
    dir = dir.normalize();
    game_variables::Bullets.emplace_back(bullet_host, pos + dulo, pos + dulo + dir, 5, 8000, SP_RIFLE_BULLET);
}
