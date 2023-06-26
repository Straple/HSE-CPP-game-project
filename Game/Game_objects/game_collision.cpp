#include "game_collision.hpp"
//

// world
#include "bush.hpp"
#include "log.hpp"
#include "table.hpp"
#include "tree.hpp"

// enemies
#include "bat.hpp"
#include "bomber.hpp"
#include "slime.hpp"

// player
#include "player.hpp"

// loot
#include "heart_and_coin.hpp"

//
#include "game_variables.hpp"

void simulate_game_collisions(efloat delta_time,
                              const std::vector<CollisionBox> &Walls) {
#define PUSH_OUT_HARD_ONE(collision, Objects)                                  \
  for (auto &object : game_variables::Objects) {                               \
    object.push_out_of_collision_hard(collision);                              \
  }

  // через стены никто пройти не может
  for (const auto &wall : Walls) {
    PUSH_OUT_HARD_ONE(wall, Players);
    PUSH_OUT_HARD_ONE(wall, Bats);
    PUSH_OUT_HARD_ONE(wall, Slimes);
    PUSH_OUT_HARD_ONE(wall, Bombers);
    PUSH_OUT_HARD_ONE(wall, Loot_hearts);
    PUSH_OUT_HARD_ONE(wall, Loot_coins);
    PUSH_OUT_HARD_ONE(wall, Knights);
    PUSH_OUT_HARD_ONE(wall, Barrels);
    PUSH_OUT_HARD_ONE(wall, Tables);
  }

  // никто не может пройти через Pillar
  for (const auto &pillar : game_variables::Pillars) {
    auto collision_ptr = pillar.get_collision();
    const Collision &collision = *collision_ptr;
    PUSH_OUT_HARD_ONE(collision, Players);
    PUSH_OUT_HARD_ONE(collision, Bats);
    PUSH_OUT_HARD_ONE(collision, Slimes);
    PUSH_OUT_HARD_ONE(collision, Bombers);
    PUSH_OUT_HARD_ONE(collision, Loot_hearts);
    PUSH_OUT_HARD_ONE(collision, Loot_coins);
    PUSH_OUT_HARD_ONE(collision, Knights);
    PUSH_OUT_HARD_ONE(collision, Barrels);
    PUSH_OUT_HARD_ONE(collision, Tables);
  }

  // никто не может пройти через NunStatue
  for (const auto &nun_statue : game_variables::NunStatues) {
    auto collision_ptr = nun_statue.get_collision();
    const Collision &collision = *collision_ptr;
    PUSH_OUT_HARD_ONE(collision, Players);
    PUSH_OUT_HARD_ONE(collision, Bats);
    PUSH_OUT_HARD_ONE(collision, Slimes);
    PUSH_OUT_HARD_ONE(collision, Bombers);
    PUSH_OUT_HARD_ONE(collision, Loot_hearts);
    PUSH_OUT_HARD_ONE(collision, Loot_coins);
    PUSH_OUT_HARD_ONE(collision, Knights);
    PUSH_OUT_HARD_ONE(collision, Barrels);
    PUSH_OUT_HARD_ONE(collision, Tables);
  }

  // сами с собой
  {
    // object2 выталкивает object1
    // возможно object1 нельзя выталкивать: слайм ест игрока, они оба статичны
#define PUSH_OUT_YOURSELF_SOFT(Objects, condition)                             \
  for (auto &object1 : game_variables::Objects) {                              \
    for (auto &object2 : game_variables::Objects) {                            \
      if (&object1 != &object2 && (condition)) {                               \
        object1.push_out_of_collision_soft(*object2.get_collision(),           \
                                           delta_time);                        \
      }                                                                        \
    }                                                                          \
  }

    PUSH_OUT_YOURSELF_SOFT(Players, !object1.is_paralyzed && !object2.is_dead())
    PUSH_OUT_YOURSELF_SOFT(Slimes, !object1.is_devour)
    PUSH_OUT_YOURSELF_SOFT(Bombers, !object1.is_booming && !object2.is_booming)

    PUSH_OUT_YOURSELF_SOFT(Bats, true)
    PUSH_OUT_YOURSELF_SOFT(Loot_coins, true)
    PUSH_OUT_YOURSELF_SOFT(Loot_hearts, true)
    PUSH_OUT_YOURSELF_SOFT(Barrels, true)
    PUSH_OUT_YOURSELF_SOFT(Knights, true)
    PUSH_OUT_YOURSELF_SOFT(Tables, true)

#undef PUSH_OUT_YOURSELF_SOFT
  }

#define PUSH_OUT_SOFT(Objects, OneWhoPushes, condition)                        \
  do {                                                                         \
    for (auto &object : game_variables::Objects) {                             \
      for (auto &he_pushes : game_variables::OneWhoPushes) {                   \
        if (condition) {                                                       \
          object.push_out_of_collision_soft(*he_pushes.get_collision(),        \
                                            delta_time);                       \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

#define PUSH_OUT_HARD(Objects, OneWhoPushes, condition)                        \
  do {                                                                         \
    for (auto &object : game_variables::Objects) {                             \
      for (auto &he_pushes : game_variables::OneWhoPushes) {                   \
        if (condition) {                                                       \
          object.push_out_of_collision_hard(*he_pushes.get_collision());       \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

  // heart <-> coin
  PUSH_OUT_SOFT(Loot_hearts, Loot_coins, true);
  PUSH_OUT_SOFT(Loot_coins, Loot_hearts, true);

  // слайм толкает игрока (игрок не мертв и не парализован)
  PUSH_OUT_SOFT(Players, Slimes, !object.is_dead() && !object.is_paralyzed);
  // игрок толкает слайма. (игрок не мертв и слайм не поедает)
  PUSH_OUT_SOFT(Slimes, Players, !he_pushes.is_dead() && !object.is_devour);

  // slime <-> bomber
  PUSH_OUT_SOFT(Slimes, Bombers, !object.is_devour && !he_pushes.is_booming);
  PUSH_OUT_SOFT(Bombers, Slimes, !object.is_booming);

  // player <-> bomber
  PUSH_OUT_SOFT(Players, Bombers,
                he_pushes.is_booming && !object.is_dead() &&
                    !object.is_paralyzed);
  PUSH_OUT_SOFT(Bombers, Players, !he_pushes.is_dead() && !object.is_booming);

  // slime with world object
  {
    // слайма толкают эти предметы, если он не ест
    PUSH_OUT_SOFT(Slimes, Bushes, !object.is_devour);
    PUSH_OUT_SOFT(Slimes, Trees, !object.is_devour);
    PUSH_OUT_SOFT(Slimes, Tables, !object.is_devour);

    // слайм толкает эти предметы
    PUSH_OUT_SOFT(Tables, Slimes, true);
    PUSH_OUT_SOFT(Knights, Slimes, true);
    PUSH_OUT_SOFT(Barrels, Slimes, true);
  }

  // bomber with world object
  {
    // бомбера толкают эти предметы, если он не взрыв
    PUSH_OUT_SOFT(Bombers, Bushes, !object.is_booming);
    PUSH_OUT_SOFT(Bombers, Trees, !object.is_booming);
    PUSH_OUT_SOFT(Bombers, Tables, !object.is_booming);

    // бомбер толкает эти предметы, если он не взрыв
    PUSH_OUT_SOFT(Tables, Bombers, !he_pushes.is_booming);
    PUSH_OUT_SOFT(Knights, Bombers, !he_pushes.is_booming);
    PUSH_OUT_SOFT(Barrels, Bombers, !he_pushes.is_booming);
  }

  // player with world object
  {
    // player <-> barrel
    PUSH_OUT_SOFT(Players, Barrels, !object.is_paralyzed);
    PUSH_OUT_SOFT(Barrels, Players, !he_pushes.is_dead());

    // player <-> knight
    PUSH_OUT_SOFT(Players, Knights, !object.is_paralyzed);
    PUSH_OUT_SOFT(Knights, Players, !he_pushes.is_dead());

    // player <-> tables
    PUSH_OUT_SOFT(Players, Tables, !object.is_paralyzed);
    PUSH_OUT_SOFT(Tables, Players, !he_pushes.is_dead());

    PUSH_OUT_SOFT(Players, Bushes, true);
    PUSH_OUT_SOFT(Players, Trees, true);
  }

  // bat with world object
  { PUSH_OUT_SOFT(Bats, Trees, true); }

  // barrel
  {
    PUSH_OUT_HARD(Barrels, Bushes, true);
    PUSH_OUT_HARD(Barrels, Trees, true);
    PUSH_OUT_HARD(Barrels, Tables, true);
  }

#undef PUSH_OUT_HARD
}
