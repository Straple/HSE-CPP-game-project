#ifndef GAME_WEAPON_HPP
#define GAME_WEAPON_HPP

#include "bullet.hpp"
#include "cursor.hpp"
#include "../../render.hpp"

struct Weapon {
    efloat cooldown;
    efloat cooldown_accum;
    int damage;
    mutable Dot dulo;
    mutable Dot hand;

    Weapon(efloat cooldown = 1, int damage = 1)
        : cooldown(cooldown), cooldown_accum(cooldown), damage(damage) {
    }

    bool may_shot(){
        return cooldown_accum >= cooldown;
    }

    void shot(Dot pos, Dot target, BulletHostType bullet_host, sprite_t sprite) {
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
            Bullets.emplace_back(bullet_host, pos + dulo, pos + dulo + dir, 1, 1000, sprite);

    }

    void simulate(efloat delta_time, Dot target) {
        cooldown_accum += delta_time;
        auto angle = get_good_angle(target, Dot(1, 0)) * 57.295779513;
        int ind = angle / 15;
    }

    void draw(Dot pos, Dot target) const {
        double angle;
        angle = get_good_angle(target - pos, Dot(1, 0)) * 57.295779513;
        int ind = angle / 15;

        if (0 <= ind && ind <= 5) {
            hand = Dot(-5, 15);
            dulo = Dot(8, 3);
            Dot new_dulo{};
            new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind)) - dulo.y * sin((15 / 57.2957795) * (-ind));
            new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind)) + dulo.y * cos((15 / 57.2957795) * (-ind));
            dulo = new_dulo;
        } else if (ind == 6) {
            hand = Dot(-15, 15);
            dulo = Dot(2, -8);
        }

        else if (20 > ind && ind >= 7) {
            hand = Dot(-15, 15);
            dulo = Dot(-1.75, -8);
            Dot new_dulo{};
            new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-(ind - 6))) - dulo.y * sin((15 / 57.2957795) * (-(ind - 6)));
            new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-(ind - 6))) + dulo.y * cos((15 / 57.2957795) * (-(ind - 6)));
            dulo = new_dulo;
        } else if (ind >= 20) {
            hand = Dot(-5, 15);
            dulo = Dot(8, 3);
            Dot new_dulo{};
            new_dulo.x = dulo.x * cos((15 / 57.2957795) * (-ind + 2)) - dulo.y * sin((15 / 57.2957795) * (-ind + 2));
            new_dulo.y = dulo.x * sin((15 / 57.2957795) * (-ind + 2)) + dulo.y * cos((15 / 57.2957795) * (-ind + 2));
            dulo = new_dulo;
        }
        draw_spritesheet(pos + hand, 1, SS_GOLDEN_GUN, ind);
    }
};

/*struct Melee_weapon : Weapon {
    Spritesheet anim;

    Melee_weapon(Sprite img, efloat cooldown, int dmg, Spritesheet an)
        : Weapon(std::move(img), cooldown, dmg), anim(std::move(an)) {
    }
};

struct Range_weapon : Weapon {
    Bullet bullet;
    std::vector<Bullet> bullets;

    Range_weapon(Sprite img, efloat cooldown, int dmg, Bullet blt)
        : Weapon(std::move(img), cooldown, dmg), bullet(blt) {
    }

    void shoot(Dot to) {
        bullets.emplace_back(to, damage);
    }

    template <class T>
    void simulate_bullets(std::vector<T> enemies) {
        for (auto bul_it = bullets.begin(); bul_it != bullets.end(); bul_it++) {
            bul_it->simulate_flight();
            bul_it->draw_bullet();
            if (bul_it->simulate_attack(player, enemies)) {
                bullets.erase(bul_it);
                bul_it--;
            }
        }
    }
};*/

#endif  // GAME_WEAPON_HPP