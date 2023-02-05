

template<typename T1, typename T2>
bool verify_others_obj(const T1& Lhs, const T2& Rhs) {
	return reinterpret_cast<const void*>(&Lhs) != reinterpret_cast<const void*>(&Rhs);
}

template<typename collision>
void update_collision(Player& player, const collision& coll) {
	if (!player.is_paralyzed) {
		player.pos = coll.bubble(player.get_collision());
	}
}
template<typename collision>
void update_collision(Slime& slime, const collision& coll) {
	if (!slime.is_attack) {
		slime.pos = coll.bubble(slime.get_collision());
	}
}
template<typename collision>
void update_collision(Bat& bat, const collision& coll) {
	bat.pos = coll.bubble(bat.get_collision());
}
template<typename collision>
void update_collision(Log& log, const collision& coll) {
	log.pos = coll.bubble(log.get_collision());
}
template<typename collision>
void update_collision(Bush& bush, const collision& coll) {
	bush.pos = coll.bubble(bush.get_collision());
}
template<typename collision>
void update_collision(Tree& tree, const collision& coll) {
	tree.pos = coll.bubble(tree.get_collision());
}

// массив объектов с коллизией выталкивет другие объекты
template<typename container_t>
void simulate_game_collision(container_t& Objects) {

	if (Objects.empty()) {
		return;
	}

	for (auto& obj : Objects) {
		obj.pos = dot(clamp(-world_half_size.x, obj.pos.x, world_half_size.x),
			clamp(-world_half_size.y, obj.pos.y, world_half_size.y));
	}

	if (typeid(Objects.back()) == typeid(Bat)) {

		// bat2 толкает bat1
		for (auto& bat1 : Bats) {
			for (auto& bat2 : Bats) {
				if (verify_others_obj(bat1, bat2)) {
					update_collision(bat2, bat1.get_collision());
				}
			}
		}
	}
	else { // obj is slime or player

		// players
		if (typeid(Objects.back()) != typeid(Slime) && typeid(Objects.back()) != typeid(Player)) {
			for (auto& obj : Objects) {
				for (auto& player : Players) {
					update_collision(player, obj.get_collision());
				}
			}
		}

		// slimes
		for (auto& obj : Objects) {
			for (auto& slime : Slimes) {
				if (verify_others_obj(slime, obj)) {

					update_collision(slime, obj.get_collision());
				}
			}
		}

		// trees
		for (auto& obj : Objects) {
			for (auto& tree : Trees) {
				if (verify_others_obj(tree, obj)) {

					update_collision(obj, tree.get_collision());
				}
			}
		}

		// bushes
		for (auto& obj : Objects) {
			for (auto& bush : Bushes) {
				if (verify_others_obj(bush, obj)) {

					update_collision(obj, bush.get_collision());
				}
			}
		}

		// fireplaces
		// trees
		for (auto& obj : Objects) {
			for (auto& fireplace : Fireplaces) {
				if (verify_others_obj(fireplace, obj)) {

					update_collision(obj, fireplace.get_collision());
				}
			}
		}

		// logs
		for (auto& obj : Objects) {
			for (auto& logs : Logs) {
				if (verify_others_obj(logs, obj)) {

					update_collision(logs, obj.get_collision());
				}
			}
		}
	}
}
