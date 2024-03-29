#ifndef GAME_WEAPON_HPP
#define GAME_WEAPON_HPP

#include "bullet.hpp"

enum weapon_t {
    GOLDEN_GUN,
    STAFF,
    RIFLE,
    SNIPER_RIFLE,
};

struct Weapon : AbstractObject {
    ADD_BYTE_SERIALIZATION();
    efloat cooldown;
    efloat cooldown_accum;
    int damage;
    Dot dulo;
    Dot hand;
    int ind;
    bool is_picked = false;
    Dot target;
    Dot delta;
    weapon_t type;

    Weapon() = default;

    explicit Weapon(Dot p, weapon_t t, efloat cooldown = 1, int damage = 1);

    bool may_shot();

    void shot(BulletHostType bullet_host);

    void simulate(efloat delta_time);

    void draw() override;

    void GoldenGun_simulate();
    void GoldenGun_draw();
    void GoldenGun_shot(BulletHostType bullet_host);

    void Staff_simulate();
    void Staff_draw();
    void Staff_shot();

    void Rifle_simulate();
    void Rifle_draw();
    void Rifle_shot(BulletHostType bullet_host);

    void Sniper_simulate();
    void Sniper_draw();
    void Sniper_shot(BulletHostType bullet_host);
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