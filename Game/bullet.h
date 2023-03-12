//
// Created by greaf on 3/10/2023.
//

#ifndef BULLETPROOF_BULLET_H
#define BULLETPROOF_BULLET_H

#define BULLET_COLLISION_RADIUS 2

#include "../Objects/Geometry/dot.h"
#include "../Objects/Geometry/line.h"
#include "../Objects/Geometry/circle.h"
#include "../Objects/Sprite/color.h"
#include "../Objects/Sprite/sprite.h"
#include "Game objects/bat.cpp"
#include "../Objects/collision.h"

struct Shape;


struct Bullet {
    Dot pos;
//    Shape form; // Добавим позже поле формы пули, чтобы можно было стрелять как кругами, так и прямоугольниками и ромбами.
// возможно, добавим лучевое оружие
    Dot dp;
    Dot ddp;
    Dot shoot_to;
    efloat attack_cooldown = 1;
    bool ricochet; //TODO
    efloat damage; // потом лучше перенести это поле в Weapon
    Bullet(Dot to): shoot_to(to) {}

    [[nodiscard]] collision_circle get_collision() const {
        return collision_circle(Circle(pos, BULLET_COLLISION_RADIUS));
    }

    Dot get_closest_dir() const { // будем использовать это, чтобы отбросить врага в сторону, приближенную к вектору удара
        const std::vector<Dot>directions = {{1, 0},{0, 1},{-1, 0},{0, -1}};
        efloat angle = 2; // косинус меньше единицы, так что инициализируем чем угодно > 1
        for (Dot el: directions) {
            angle = std::min(angle,abs(get_angle(el, shoot_to)));
        }
    }

    void bubble_bullet(Bat& bat, collision_circle& col_circ) const {
        if (col_circ.circle.isin(pos)) {
            if (bat.pos == col_circ.circle.pos) {
                bat.pos = col_circ.circle.get_random_dot();
            } else {
                Dot vector = (bat.pos - col_circ.circle.pos).normalize() * col_circ.circle.radius;
                bat.pos = col_circ.circle.pos + vector;
            }
            //TODO надо различать, кому мы можем нанести урон, а кому нет. стенке - не можем. мышке - можем.

        }
    }

    template<typename container_t>
    bool simulate_shot(container_t &Enemies) {
        bool is_attack = false;
        for (int i = 0; i < Enemies.size(); i++) {
            if (attack_trigger(Enemies[i].pos) &&
                reinterpret_cast<char *>(&Enemies[i]) !=
                reinterpret_cast<char *>(this)) {
                is_attack = true;

                Enemies[i].simulate_hit(*this);

                if (Enemies[i].hp <= 0 &&
                    typeid(Enemies[i]) != typeid(Player)) {
                    exp += Enemies[i].gobj_state.exp_cnt;
                    Enemies.erase(Enemies.begin() + i);
                    i--;
                }
            }
        }

        return is_attack;
    }
    void shoot(Dot to) {
        //TODO проверка на кулдаун, которую лучше запихнуть в Weapon
        ddp = Dot();
        Dot vec = Line(pos, to).get_vector();
        //std::optional<Object> hitted_object;
        //while (!(hitted = got_hit()).has_value())
//        draw_circle(Circle(pos, 3), Color(0xff0000, 100)); почему-то не работает
          pos += vec;
        //Если объект - NPC: NPC.hp -= damage;
        //TODO уничтожение объекта с эффектом
    }
};

struct Weapon {
    Sprite image;
    Bullet bullet;
    efloat cooldown;

    //TODO
};






#endif //BULLETPROOF_BULLET_H
