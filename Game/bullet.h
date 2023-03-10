//
// Created by greaf on 3/10/2023.
//

#ifndef BULLETPROOF_BULLET_H
#define BULLETPROOF_BULLET_H

#include "../Objects/Geometry/geometry.h"
#include "../Objects/Sprite/sprite.h"

struct Shape;


struct Bullet {
    Dot pos;
//    Shape form; // Добавим позже поле формы пули, чтобы можно было стрелять как кругами, так и прямоугольниками и ромбами.
// возможно, добавим лучевое оружие
    Dot dp;
    Dot ddp;
    Dot shoot_to;
    bool ricochet; //TODO
    efloat damage; // потом лучше перенести это поле в Weapon
    Bullet(Dot to): shoot_to(to) {}
//    std::optional<Object> got_hit() {
//        //TODO for obj: objects проверить их нахождение в точке. Объектов мало, поэтому проверка не будет долгой, наверное
//        //TODO Кроме того, нужно проверить принадлежность точки границе карты
//    }
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
