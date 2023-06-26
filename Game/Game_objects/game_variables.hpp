#ifndef GAME_VARIABLES_HPP
#define GAME_VARIABLES_HPP

#include "barrel.hpp"
#include "bat.hpp"
#include "bomber.hpp"
#include "bullet.hpp"
#include "bush.hpp"
#include "effect.hpp"
#include "heart_and_coin.hpp"
#include "knight.hpp"
#include "log.hpp"
#include "nun_statue.hpp"
#include "pillar.hpp"
#include "player.hpp"
#include "slime.hpp"
#include "table.hpp"
#include "tree.hpp"
#include "weapon.hpp"

struct game_variables {
  static inline std::vector<Player> Players;

  static inline std::vector<Bat> Bats;
  static inline std::vector<Bomber> Bombers;
  static inline std::vector<Slime> Slimes;

  static inline std::vector<Weapon> Weapons;

  static inline std::vector<Bullet> Bullets;

  static inline std::vector<Coin> Loot_coins;
  static inline std::vector<Heart> Loot_hearts;

  static inline std::vector<Effect> Effects;

  static inline std::vector<Tree> Trees;
  static inline std::vector<Bush> Bushes;
  static inline std::vector<Pillar> Pillars;
  static inline std::vector<NunStatue> NunStatues;
  static inline std::vector<Knight> Knights;
  static inline std::vector<Barrel> Barrels;

  static inline std::vector<Log> Logs;
  static inline std::vector<Table> Tables;
};

#endif // GAME_VARIABLES_HPP
