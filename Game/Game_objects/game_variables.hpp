#ifndef GAME_VARIABLES_HPP
#define GAME_VARIABLES_HPP

struct Bullet;
struct Player;
struct Bat;
struct Bomber;
struct Log;
struct Bush;
struct Slime;
struct Tree;
struct Effect;
struct Heart;
struct Coin;
struct Table;


struct game_variables {
    static inline std::vector<Player> Players;
    static inline std::vector<Bat> Bats;
    static inline std::vector<Bomber> Bombers;
    static inline std::vector<Bullet> Bullets;
    static inline std::vector<Bush> Bushes = {
            // Bush(Dot(49, -106)),
    };
    static inline std::vector<Coin> Loot_coins;
    static inline std::vector<Heart> Loot_hearts;
    static inline std::vector<Log> Logs;
    static inline std::vector<Slime> Slimes;
    static inline std::vector<Table> Tables;
    static inline std::vector<Tree> Trees;
    static inline std::vector<Effect> Effects;
};



#endif //GAME_VARIABLES_HPP
