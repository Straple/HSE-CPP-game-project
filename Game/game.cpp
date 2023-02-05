static const dot world_half_size(500, 500);

#include "game_utils.cpp"

#include "UI Objects/ui_objects.cpp"

#include "Game objects/game_objects.cpp"

// game objects

std::vector<Bush> Bushes;

std::vector<Slime> Slimes;

std::vector<Tree> Trees;

std::vector<Bat> Bats;

std::vector<Fireplace> Fireplaces = {
	dot(30, -30),
};

// Players
// 
// Slimes
// Bats
// 
// Trees
// Bushes
// Logs
// Fireplaces
// 
// Effects

std::string get_world_state() {

	for (auto& player : Players) {
		if (player.lvl >= 20) {
			char* memory = new char[sizeof(player.name) + 1];
			memory[0] = 't'; // have winner

			for (int i = 0; i < sizeof(player.name); i++) {
				memory[i + 1] = player.name[i];
			}

			return memory;
		}
	}

	std::ostringstream oss(std::ios::binary);

	oss << "f";

	serialization_traits<std::vector<Player>>::serialize(oss, Players);

	serialization_traits<std::vector<Slime>>::serialize(oss, Slimes);
	serialization_traits<std::vector<Bat>>::serialize(oss, Bats);

	serialization_traits<std::vector<Tree>>::serialize(oss, Trees);
	serialization_traits<std::vector<Bush>>::serialize(oss, Bushes);
	serialization_traits<std::vector<Log>>::serialize(oss, Logs);
	serialization_traits<std::vector<Fireplace>>::serialize(oss, Fireplaces);

	serialization_traits<std::vector<effect>>::serialize(oss, Effects);

	return oss.str();
}

void set_world_state(std::string s) {

	// ERROR HERE!
	
	std::istringstream iss(s);

	std::string af = iss.str();

	Players = serialization_traits<std::vector<Player>>::deserialize(iss);

	Slimes = serialization_traits<std::vector<Slime>>::deserialize(iss);
	Bats = serialization_traits<std::vector<Bat>>::deserialize(iss);

	Trees = serialization_traits<std::vector<Tree>>::deserialize(iss);
	Bushes = serialization_traits<std::vector<Bush>>::deserialize(iss);
	Logs = serialization_traits<std::vector<Log>>::deserialize(iss);
	Fireplaces = serialization_traits<std::vector<Fireplace>>::deserialize(iss);

	Effects = serialization_traits<std::vector<effect>>::deserialize(iss);
}

#include "game_collision.cpp"

#define GAME_OBJECT_MAX 200
#define GAME_OBJECT_MIN 170
#define GAME_ENEMIES_MAX 40
#define GAME_ENEMIES_MIN 30

void build_world() {
	std::uniform_int_distribution<s32> random_size(GAME_OBJECT_MIN, GAME_OBJECT_MAX);
	std::uniform_int_distribution<s32> random_size_enemies(GAME_ENEMIES_MIN, GAME_ENEMIES_MAX);
	std::uniform_int_distribution<s32> random_x(-world_half_size.x, world_half_size.x);
	std::uniform_int_distribution<s32> random_y(-world_half_size.y, world_half_size.y);

	auto random_dot = [&]() -> dot {
		return dot(random_x(rnd), random_y(rnd));
	};

	auto build = [&](auto& Objects, u32 size) {
		Objects.resize(size);

		for (auto& obj : Objects) {
			obj = random_dot();
		}
	};

	// build bushes and trees
	{
		build(Bushes, random_size(rnd));
		build(Trees, random_size(rnd));

		auto get_coll = [&](auto obj) -> collision_circle {
			auto res = collision_circle(obj.get_collision());
			res.circle.radius += 5;
			return res;
		};

		for (auto& tree1 : Trees) {
			for (auto& tree2 : Trees) {
				if (verify_others_obj(tree1, tree2)) {

					update_collision(tree1, get_coll(tree2));
				}
			}
		}

		for (auto& bush1 : Bushes) {
			for (auto& bush2 : Bushes) {
				if (verify_others_obj(bush1, bush2)) {

					update_collision(bush1, get_coll(bush2));
				}
			}
		}

		for (auto& tree : Trees) {
			for (auto& bush : Bushes) {
				if (verify_others_obj(tree, bush)) {

					update_collision(tree, get_coll(bush));
				}
			}
		}
		for (auto& bush : Bushes) {
			for (auto& tree : Trees) {
				if (verify_others_obj(bush, tree)) {

					update_collision(bush, get_coll(tree));
				}
			}
		}

		if (!Fireplaces.empty()) {
			for (s32 i = 0; i < Bushes.size(); i++) {
				if (Bushes[i].get_collision().trigger(Fireplaces[0].get_collision())) {
					Bushes.erase(Bushes.begin() + i);
					i--;
				}
			}

			for (s32 i = 0; i < Trees.size(); i++) {
				if (Trees[i].get_collision().trigger(Fireplaces[0].get_collision())) {
					Trees.erase(Trees.begin() + i);
					i--;
				}
			}
		}

		for (s32 i = 0; i < Bushes.size(); i++) {
			for (s32 j = i + 1; j < Bushes.size(); j++) {
				if (Bushes[i].get_collision().trigger(get_coll(Bushes[j]))) {
					Bushes.erase(Bushes.begin() + i);
					i--;
					break;
				}
			}
		}

		for (s32 i = 0; i < Trees.size(); i++) {
			for (s32 j = i + 1; j < Trees.size(); j++) {
				if (Trees[i].get_collision().trigger(get_coll(Trees[j]))) {
					Trees.erase(Trees.begin() + i);
					i--;
					break;
				}
			}
		}
	}

	build(Slimes, random_size_enemies(rnd));
	build(Bats, random_size_enemies(rnd));
}

// UI objects

Mouse mouse(SP_CURSOR, SP_FOCUS_CURSOR, 0.09);


int find_player_ind(u8 player_id) {
	int i;
	for (i = 0; i < Players.size() && Players[i].id != player_id; i++) {}
	return i;
}

void simulate_player(const Input& input, point_t delta_time, u8 player_id) {

	player_id = find_player_ind(player_id);

	// накопление вектора движения
	auto accum_ddp = [&input](button_t left, button_t right, button_t top, button_t bottom) -> dot {
		return dot(is_down(right) - is_down(left), is_down(top) - is_down(bottom));
	};

	Players[player_id].simulate(delta_time, accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S), is_down(BUTTON_J), pressed(BUTTON_SPACE));

	// player attack
	{
		if (Players[player_id].simulate_attack(Logs) |
			Players[player_id].simulate_attack(Slimes) |
			Players[player_id].simulate_attack(Bats) |
			Players[player_id].simulate_attack(Trees) |
			Players[player_id].simulate_attack(Players)) {

			Players[player_id].now_is_attached = false;
		}
	}
}

void simulate_physics(point_t delta_time) {

	// add new world objects
	{
		std::uniform_int_distribution<s32> random_x(-world_half_size.x, world_half_size.x);
		std::uniform_int_distribution<s32> random_y(-world_half_size.y, world_half_size.y);
		std::uniform_int_distribution<s32> range(1, 3);

		auto random_dot = [&]() -> dot {
			return dot(random_x(rnd), random_y(rnd));
		};

		
		if (Slimes.size() < GAME_ENEMIES_MIN) {
			int cnt = range(rnd);

			while (cnt--) {
				Slimes.push_back(random_dot());
				//std::cout << "new slime: " << Slimes.back().pos << "\n";
			}
		}

		if (Bats.size() < GAME_ENEMIES_MIN) {
			int cnt = range(rnd);

			while (cnt--) {
				Bats.push_back(random_dot());
				//std::cout << "new bat: " << Bats.back().pos << "\n";
			}
		}

		if (Trees.size() < GAME_OBJECT_MIN) {
			int cnt = range(rnd);

			while (cnt--) {
				Trees.push_back(random_dot());
				//std::cout << "new tree: " << Trees.back().pos << "\n";
			}

			auto get_coll = [&](auto obj) -> collision_circle {
				auto res = collision_circle(obj.get_collision());
				res.circle.radius += 5;
				return res;
			};

			for (auto& tree1 : Trees) {
				for (auto& tree2 : Trees) {
					if (verify_others_obj(tree1, tree2)) {

						update_collision(tree1, get_coll(tree2));
					}
				}
			}

			for (auto& tree : Trees) {
				for (auto& bush : Bushes) {
					if (verify_others_obj(tree, bush)) {

						update_collision(tree, get_coll(bush));
					}
				}
			}
		}
	}

	// simulate logs
	{
		for (auto& log : Logs) {
			log.simulate(delta_time);
		}
		simulate_game_collision(Logs);
	}

	// simulate slimes
	{
		for (auto& slime : Slimes) {
			slime.simulate(delta_time);
		}

		simulate_game_collision(Slimes);
	}

	// simulate bats
	{
		for (auto& bat : Bats) {
			bat.simulate(delta_time);
		}

		simulate_game_collision(Bats);
	}

	// simulate fireplaces
	{
		for (auto& fireplace : Fireplaces) {
			fireplace.simulate(delta_time);
		}
	}

	// simulate effects
	{
		for (u32 i = 0; i < Effects.size(); i++) {
			if (Effects[i].simulate(delta_time)) {
				Effects.erase(Effects.begin() + i);
				i--;
			}
		}
	}
}

void render_game(u8 player_id) {

	clear_screen(0xff000000);

	draw_texture(dot(-world_half_size.x, world_half_size.y), 32, 32, 0.5, SP_GRASS_BACKGROUND);

	// draw players, slimes, bats, 
	// bushes, trees, fireplaces,
	// logs
	{
		// top sort

		struct top_sort_object {

			enum type_object {
				TO_PLAYER,
				TO_BUSH,
				TO_SLIME,
				TO_TREE,
				TO_LOG,
				TO_BAT,
				TO_FIREPLACE,

				TO_UNDEFIND,
			};

			type_object type;
			const void* ptr;

			top_sort_object() {
				type = TO_UNDEFIND;
				ptr = nullptr;
			}

			top_sort_object(const Player& player) {
				type = TO_PLAYER;
				ptr = reinterpret_cast<const void*>(&player);
			}
			top_sort_object(const Fireplace& fireplace) {
				type = TO_FIREPLACE;
				ptr = reinterpret_cast<const void*>(&fireplace);
			}
			top_sort_object(const Bat& player) {
				type = TO_BAT;
				ptr = reinterpret_cast<const void*>(&player);
			}
			top_sort_object(const Log& log) {
				type = TO_LOG;
				ptr = reinterpret_cast<const void*>(&log);
			}
			top_sort_object(const Bush& bush) {
				type = TO_BUSH;
				ptr = reinterpret_cast<const void*>(&bush);
			}
			top_sort_object(const Slime& slime) {
				type = TO_SLIME;
				ptr = reinterpret_cast<const void*>(&slime);
			}
			top_sort_object(const Tree& tree) {
				type = TO_TREE;
				ptr = reinterpret_cast<const void*>(&tree);
			}

			point_t get_y() const {
				switch (type) {
				case TO_PLAYER: {
					return reinterpret_cast<const Player*>(ptr)->pos.y;
				}break;
				case TO_BUSH: {
					return reinterpret_cast<const Bush*>(ptr)->pos.y;
				}break;
				case TO_SLIME: {
					return reinterpret_cast<const Slime*>(ptr)->pos.y;
				}break;
				case TO_TREE: {
					return reinterpret_cast<const Tree*>(ptr)->pos.y;
				}break;
				case TO_LOG: {
					return reinterpret_cast<const Log*>(ptr)->pos.y;
				}break;
				case TO_BAT: {
					return reinterpret_cast<const Bat*>(ptr)->pos.y;
				}break;
				case TO_FIREPLACE: {
					return reinterpret_cast<const Fireplace*>(ptr)->pos.y;
				}break;
				}

				ASSERT(false, "undefind object type");
			}

			void draw() {
				switch (type) {
				case TO_PLAYER: {
					reinterpret_cast<const Player*>(ptr)->draw();
				}break;
				case TO_BUSH: {
					reinterpret_cast<const Bush*>(ptr)->draw();
				}break;
				case TO_SLIME: {
					reinterpret_cast<const Slime*>(ptr)->draw();
				}break;
				case TO_TREE: {
					reinterpret_cast<const Tree*>(ptr)->draw();
				}break;
				case TO_LOG: {
					reinterpret_cast<const Log*>(ptr)->draw();
				}break;
				case TO_BAT: {
					reinterpret_cast<const Bat*>(ptr)->draw();
				}break;
				case TO_FIREPLACE: {
					reinterpret_cast<const Fireplace*>(ptr)->draw();
				}break;
				case TO_UNDEFIND: {
					ASSERT(false, "undefind object type");
				}break;
				}
			}

			bool operator < (const top_sort_object& Rhs) const {
				return get_y() > Rhs.get_y();
			}
		};

		std::vector<top_sort_object> Objects;
		for (auto& player : Players) {
			Objects.push_back(player);
		}
		for (auto& fireplace : Fireplaces) {
			Objects.push_back(fireplace);
		}
		for (auto& slime : Slimes) {
			Objects.push_back(slime);
		}
		for (auto& bush : Bushes) {
			Objects.push_back(bush);
		}
		for (auto& tree : Trees) {
			Objects.push_back(tree);
		}
		for (auto& log : Logs) {
			Objects.push_back(log);
		}
		for (auto& bat : Bats) {
			Objects.push_back(bat);
		}

		std::stable_sort(Objects.begin(), Objects.end());

		for (auto& obj : Objects) {
			obj.draw();
		}
	}
	
	// draw effects
	{
		for (u32 i = 0; i < Effects.size(); i++) {
			Effects[i].draw();
		}
	}


	player_id = find_player_ind(player_id);
	
	// hp
	ui_state(dot(5 - arena_half_size.x, arena_half_size.y - 5), dot(25 - arena_half_size.x, arena_half_size.y - 7.5))
		.draw(Players[player_id].hp, Players[player_id].max_hp, GREY, RED);

	// exp
	ui_state(dot(5 - arena_half_size.x, arena_half_size.y - 10), dot(25 - arena_half_size.x, arena_half_size.y - 12.5))
		.draw(Players[player_id].exp, s16(10), GREY, YELLOW);

	// damage
	draw_object(Players[player_id].damage, dot(5 - arena_half_size.x, arena_half_size.y - 15), 0.6, BLUE);

	draw_object(Players[player_id].lvl, dot(5 - arena_half_size.x, arena_half_size.y - 20), 0.6, PURPLE);


	mouse.draw();
}

template<typename func_t>
void simulate_input(const Input& input, func_t&& window_mode_callback) {
	if (pressed(BUTTON_ESC)) {
		eng_state.flip(RUN_SETUP);
	}

	if (pressed(BUTTON_ENTER)) {
		window_mode_callback();
	}

	/*if (pressed(BUTTON_TAB)) {
		eng_state.flip(DEBUG);
	}

	if (pressed(BUTTON_K)) {
		eng_state.flip(LOCATOR_VIS);
	}

	if (pressed(BUTTON_F)) {
		eng_state.flip(FPS_VIS);
	}*/

	// update render_scale
	/*{
		if (is_down(BUTTON_UP)) {

			point_t pt_x = (mouse.pos.x + arena_half_size.x) * scale_factor;
			point_t pt_y = (mouse.pos.y + arena_half_size.y) * scale_factor;


			render_scale *= 0.95;

			// relax scaling
			scale_factor = render_state.height * render_scale;

			// relax arena
			arena_half_size = dot(static_cast<point_t>(render_state.width) / scale_factor, static_cast<point_t>(1) / render_scale) * 0.5;


			mouse.pos = dot(pt_x, pt_y)
				/ scale_factor - arena_half_size;
		}

		if (is_down(BUTTON_DOWN)) {
			point_t pt_x = (mouse.pos.x + arena_half_size.x) * scale_factor;
			point_t pt_y = (mouse.pos.y + arena_half_size.y) * scale_factor;


			render_scale /= 0.95;

			// relax scaling
			scale_factor = render_state.height * render_scale;

			// relax arena
			arena_half_size = dot(static_cast<point_t>(render_state.width) / scale_factor, static_cast<point_t>(1) / render_scale) * 0.5;


			mouse.pos = dot(pt_x, pt_y)
				/ scale_factor - arena_half_size;
		}
	}*/

	mouse.simulate(input);
}

template<typename func_t>
void simulate_game(const Input& input, point_t delta_time, func_t&& window_mode_callback) {

	simulate_input(input, window_mode_callback);

	camera.simulate(Players[0].pos, delta_time);

	// simulate players
	{
		simulate_player(input, delta_time, 0);
		//simulate_player(input, delta_time, 1);
		//...

		simulate_game_collision(Players);
	}

	simulate_physics(delta_time);

	mouse.simulate(input);

	render_game(0);

	/*draw_text_align("\
+---------------+\n\
|1234567890+-!?.|\n\
+---------------+\n\
", dot(0, 0), 0.6, 0xffffffffff);
	draw_text_align("\
+--------------------------+\n\
|ABCDEFGHIJKLMNOPQRSTUVWXYZ|\n\
|abcdefghijklmnopqrstuvwxyz|\n\
+--------------------------+\n\
", dot(0, -20), 0.6, 0xffffffffff);*/
}
