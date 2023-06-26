#ifndef GAME_ENGINE_COLLISION_HPP
#define GAME_ENGINE_COLLISION_HPP

#include "Geometry/geometry.hpp"
#include <memory>

struct Collision {
  // некоторая позиция
  [[nodiscard]] virtual Dot get_pos() const = 0;

  // находится ли точка внутри коллизии
  [[nodiscard]] virtual bool trigger(const Dot &pos) const = 0;

  [[nodiscard]] virtual bool trigger(const Collision &other) const = 0;

  // вернет вытолкнутую точку за пределы коллизии
  [[nodiscard]] virtual Dot bubble(Dot pos) const = 0;

  [[nodiscard]] virtual std::unique_ptr<Collision>
  bubble(const Collision &other) const = 0;

  virtual ~Collision() = default;
};

struct CollisionBox;

struct CollisionCircle : Collision {
  Circle circle;

  CollisionCircle() = default;

  explicit CollisionCircle(const Circle &circle);

  CollisionCircle(Dot pos, efloat radius);

  [[nodiscard]] Dot get_pos() const override;

  [[nodiscard]] bool trigger(const Dot &pos) const override;

  [[nodiscard]] bool trigger(const CollisionCircle &other) const;

  [[nodiscard]] bool trigger(const CollisionBox &other) const;

  [[nodiscard]] bool trigger(const Collision &other) const override;

  [[nodiscard]] Dot bubble(Dot pos) const override;

  [[nodiscard]] CollisionCircle bubble(const CollisionCircle &other) const;

  [[nodiscard]] CollisionBox bubble(const CollisionBox &other) const;

  [[nodiscard]] std::unique_ptr<Collision>
  bubble(const Collision &other) const override;
};

/*  p0---+
 *  |    |
 *  |    |
 *  |    |
 *  +----p1
 */
struct CollisionBox : Collision {
  Dot p0, p1;

  CollisionBox() = default;

  CollisionBox(const Dot &new_p0, const Dot &new_p1);

  // некоторая позиция
  [[nodiscard]] Dot get_pos() const override;

  // находится ли точка внутри коллизии
  [[nodiscard]] bool trigger(const Dot &pos) const override;

  [[nodiscard]] bool trigger(const CollisionCircle &other) const;

  [[nodiscard]] bool trigger(const CollisionBox &other) const;

  [[nodiscard]] bool trigger(const Collision &other) const override;

  // выталкивает pos за пределы коллизии
  [[nodiscard]] Dot bubble(Dot pos) const override;

  [[nodiscard]] CollisionCircle bubble(const CollisionCircle &other) const;

  [[nodiscard]] CollisionBox bubble(CollisionBox other) const;

  [[nodiscard]] std::unique_ptr<Collision>
  bubble(const Collision &other) const override;
};

#endif // GAME_ENGINE_COLLISION_HPP