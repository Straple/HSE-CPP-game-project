#include "weapon.hpp"
//
#include "game_variables.hpp"
#include "../../render.hpp"
#include "../../sprites.hpp"

Weapon::Weapon(efloat cooldown, int damage) : cooldown(cooldown), cooldown_accum(cooldown), damage(damage) {
}

bool Weapon::may_shot() {
    return cooldown_accum >= cooldown;
}

void Weapon::shot(Dot pos, Dot target, BulletHostType bullet_host, sprite_t sprite) {
    if (!may_shot()) {
        return;
    }
    cooldown_accum = 0;
    if (hand == Dot(-5, 15)) {
        pos += Dot(7, -2);
    } else {
        pos += Dot(-2.5, -1.5);
    }
    // pos+=dulo;
    Dot dir = target - pos;
    dir = dir.normalize();
    dir += Circle(Dot(), 0.1).get_random_dot();
    game_variables::Bullets.emplace_back(bullet_host, pos + dulo, pos + dulo + dir, 1, 1000, sprite);
}

void Weapon::simulate(efloat delta_time, Dot target) {
    cooldown_accum += delta_time;
    auto angle = get_good_angle(target, Dot(1, 0)) * 57.295779513;
    int ind = angle / 15;
}

void Weapon::draw(Dot pos, Dot target) const {
    double angle;
    angle = get_good_angle(target - pos, Dot(1, 0)) * 57.295779513;
    int ind = angle / 15;

    if (0 <= ind && ind <= 5) {
        hand = Dot(-10, 18);
        dulo = Dot(14, 8);
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind)) - dulo.y * sin((15 / 57.2957795) * (-ind));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind)) + dulo.y * cos((15 / 57.2957795) * (-ind));
        dulo = new_dulo;
    } else if (ind == 6) {
        hand = Dot(-20, 18);
        dulo = Dot(2, -8);
    } else if (20 > ind && ind >= 7) {
        hand = Dot(-20, 18);
        dulo = Dot(-1.75, -8);
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-(ind - 6))) - dulo.y * sin((15 / 57.2957795) * (-(ind - 6)));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-(ind - 6))) + dulo.y * cos((15 / 57.2957795) * (-(ind - 6)));
        dulo = new_dulo;
    } else if (ind >= 20) {
        hand = Dot(-10, 18);
        dulo = Dot(8, 3);
        Dot new_dulo{};
        new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind + 2)) - dulo.y * sin((15 / 57.2957795) * (-ind + 2));
        new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind + 2)) + dulo.y * cos((15 / 57.2957795) * (-ind + 2));
        dulo = new_dulo;
    }
    draw_spritesheet(pos + hand, 0.7, SS_GOLDEN_GUN, ind);
}
