#ifndef GAME_WEAPON_HPP
#define GAME_WEAPON_HPP

#include "../../render.hpp"
#include "bullet.hpp"
#include "cursor.hpp"

std::mt19937 rng(293);
std::uniform_int_distribution<> gen_int(-1, 1);

#include <map>

std::map<int, Dot> BulletDeltas = {
    {0, Dot(9, 4)},       {1, Dot(9, 2.5)},    {2, Dot(8.7, 0.5)},
    {3, Dot(8.5, -1)},    {4, Dot(7.5, -2.5)}, {5, Dot(6.5, -3.5)},
    {6, Dot(5.5, -4)},    {7, Dot(4.4, -4.4)}, {8, Dot(2.3, -4.4)},
    {9, Dot(1, -4)},      {10, Dot(0, -3)},    {11, Dot(-1, -1.7)},
    {12, Dot(-2, -0.5)},  {13, Dot(-3, 0.7)},  {14, Dot(-2.7, 1.7)},
    {15, Dot(-2.3, 3.5)}, {16, Dot(-1.3, 5)},  {17, Dot(-0.3, 6.5)},
    {18, Dot(0.7, 7.5)},  {19, Dot(2.5, 8)},   {20, Dot(3.7, 7.9)},
    {21, Dot(4.7, 7.6)},  {22, Dot(6, 6.8)},   {23, Dot(8.2, 4.8)}};

struct Weapon {
    efloat cooldown;
    efloat cooldown_accum;
    int damage;

    Dot bullet_delta;

    Weapon(efloat cooldown = 0.5, int damage = 50)
        : cooldown(cooldown), cooldown_accum(cooldown), damage(damage) {
    }

    void shot(Dot pos) {
        if (cooldown_accum >= cooldown) {
            cooldown_accum = 0;

            Bullets.emplace_back(
                // player.pos,
                pos + Dot(6, 2),  // центрированная позиция относительно спрайта
                cursor.pos + global_variables::camera.pos, 50, 1000
            );
        }
    }

    void simulate(efloat delta_time) {
        cooldown_accum += delta_time;
        auto angle = get_good_angle(cursor.pos, Dot(1, 0)) * 57.295779513;
        int ind = angle / 15;
        bullet_delta = BulletDeltas[ind];
    }

    void draw(Dot pos) const {
        Dot ox(1, 0);
        auto angle = get_angle(cursor.pos, ox) * 57.295779513;

        if (angle < 0) {
            angle += 360;
        }
        int ind = angle / 15;

        Dot drawing_delta;
        switch (ind) {
            case 0:
                drawing_delta = Dot(1.1, 7);
                break;
            case 1:
                drawing_delta = Dot(1.15, 7);
                break;
            case 2:
                drawing_delta = Dot(1, 6);
                break;
            case 3:
                drawing_delta = Dot(1.15, 5.5);
                break;
            case 4:
                drawing_delta = Dot(1, 5.0);
                break;
            case 5:
                drawing_delta = Dot(0.65, 4.5);
                break;
            case 6:
                drawing_delta = Dot(0.5, 4);
                break;
            case 7:
                drawing_delta = Dot(0, 3.7);
                break;
            case 8:
                drawing_delta = Dot(-1, 3.5);
                break;
            case 9:
                drawing_delta = Dot(-1.5, 3.7);
                break;
            case 10:
                drawing_delta = Dot(-2, 4);
                break;
            case 11:
                drawing_delta = Dot(-2.5, 4.5);
                break;
            case 12:
                drawing_delta = Dot(-3, 5);
                break;
            case 13:
                drawing_delta = Dot(-3.5, 5.5);
                break;
            case 14:
                drawing_delta = Dot(-3.5, 6);
                break;
            case 15:
                drawing_delta = Dot(-3.5, 7);
                break;
            case 16:
                drawing_delta = Dot(-3, 7.5);
                break;
            case 17:
                drawing_delta = Dot(-2.5, 8);
                break;
            case 18:
                drawing_delta = Dot(-2.5, 8.5);
                break;
            case 19:
                drawing_delta = Dot(-1.2, 9);
                break;
            case 20:
                drawing_delta = Dot(-1, 9);
                break;
            case 21:
                drawing_delta = Dot(-0.2, 8.8);
                break;
            case 22:
                drawing_delta = Dot(0.3, 8.5);
                break;
            case 23:
                drawing_delta = Dot(0.5, 7.5);
                break;
        }

        draw_spritesheet(pos + drawing_delta, 0.2, SS_GOLDEN_GUN, ind);
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