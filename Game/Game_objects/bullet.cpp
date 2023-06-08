#include "bullet.hpp"

Bullet::Bullet(BulletHostType host, Dot from, Dot to, int damage, int speed, sprite_t sp)
    : host(host), ddp((to - from).normalize() * speed), damage(damage), sprite(sp) {
    pos = from;
}

[[nodiscard]] std::unique_ptr<Collision> Bullet::get_collision() const {
    return std::make_unique<CollisionCircle>(pos, collision_radius);
}

void Bullet::simulate(efloat delta_time) {
    simulate_move2d(pos, dp, ddp, delta_time);
}

void Bullet::draw() {
    draw_sprite(pos + Dot(-2, 2), 0.4, sprite);
    draw_collision(*this);
}
