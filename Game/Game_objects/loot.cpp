#include "loot.hpp"
//
#include "game_variables.hpp"
#include "player.hpp"

Loot::Loot(Dot position, Dot dir)
    : AbstractPhysicalObject(position) {
    dp = dir * 200;
}

[[nodiscard]] std::unique_ptr<Collision> Loot::get_collision() const {
    return std::make_unique<CollisionCircle>(pos, collision_radius);
}

[[nodiscard]] bool Loot::collection_trigger(Dot player_pos) const {
    return get_collision()->trigger(player_pos);
}

void Loot::simulate(efloat delta_time) {
    int index = find_nearest_player_index(pos);

    if (index == -1) {
        // Нет игроков
        return;
    }
    Dot to = game_variables::Players[index].pos;

    if ((to - pos).get_len() <= magnet_radius) {
        Dot ddp = (to - pos) * 100;
        simulate_move_to2d(pos, to, dp, ddp, delta_time);
    } else {
        simulate_move2d(pos, dp, Dot(), delta_time);
    }
}
