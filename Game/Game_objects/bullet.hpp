#ifndef GAME_BULLET_HPP
#define GAME_BULLET_HPP

#include "../../render.hpp"
#include "abstract_physical_object.hpp"

struct Slime;
struct Bat;
struct Bomber;
struct Player;

enum class BulletHostType {
    // эту пулю выпустил игрок
    PLAYER,
    // эту пулю выпустил моб
    ENEMY,
};

struct Bullet : AbstractPhysicalObject {
    ADD_BYTE_SERIALIZATION();

    // Добавим позже поле формы пули, чтобы можно было
    // стрелять как кругами, так и прямоугольниками и ромбами.
    // Возможно, добавим лучевое оружие

    // physics
    inline const static efloat collision_radius = 2;

    Dot ddp;

    int damage;

    sprite_t sprite;

    BulletHostType host;

    Bullet() = default;

    Bullet(BulletHostType host, Dot from, Dot to, int damage, int speed, sprite_t sp);

    [[nodiscard]] std::unique_ptr<Collision> get_collision() const override;

    // вернет правду, если атака кого-то зацепила
    bool simulate_attack_on_mob(std::vector<Slime> &Enemies);
    bool simulate_attack_on_mob(std::vector<Bat> &Enemies);
    bool simulate_attack_on_mob(std::vector<Bomber> &Enemies);

    bool simulate_attack_on_player(std::vector<Player> &Players);

    void simulate(efloat delta_time);

    void draw() override;
};

#endif  // GAME_BULLET_HPP