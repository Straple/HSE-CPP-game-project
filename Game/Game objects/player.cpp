// visibility
#define PLAYER_DELTA_DRAW_POS dot(-30.5, 41) * gobj_state.size

// physics
#define PLAYER_DDP_SPEED 500
#define PLAYER_STATICPOS_COOLDOWN 5
#define PLAYER_COLLISION_RADIUS 3

#define PLAYER_MAX_COMBO 3
#define PLAYER_ATTACK_COOLDOWN 1.3
#define PLAYER_JUMP_COOLDOWN 0.5
#define PLAYER_RECOVERY_COOLDOWN 1

const static animation player_anims[] = {
		animation(SP_PLAYER, 0, 6, 0.1, 64),  // run right
		animation(SP_PLAYER, 6, 6, 0.1, 64),  // run up
		animation(SP_PLAYER, 12, 6, 0.1, 64), // run left
		animation(SP_PLAYER, 18, 6, 0.1, 64), // run down

		animation(SP_PLAYER, 24, 4, 0.12, 64), // attack right
		animation(SP_PLAYER, 28, 4, 0.12, 64), // attack up
		animation(SP_PLAYER, 32, 4, 0.12, 64), // attack left
		animation(SP_PLAYER, 36, 4, 0.12, 64), // attack down

		animation(SP_PLAYER, 40, 5, 0.1, 64), // roll right
		animation(SP_PLAYER, 45, 5, 0.1, 64), // roll up
		animation(SP_PLAYER, 50, 5, 0.1, 64), // roll left
		animation(SP_PLAYER, 55, 5, 0.1, 64), // roll down

		animation(SP_PLAYER, 0, 1, 0.1, 64),  // idle right
		animation(SP_PLAYER, 6, 1, 0.1, 64),  // idle up
		animation(SP_PLAYER, 12, 1, 0.1, 64), // idle left
		animation(SP_PLAYER, 18, 1, 0.1, 64), // idle down
};

// дерево анимаций игрока
struct Player_anim_tree {

	enum anim_t: u8 {
		NONE,
		RUN,
		ATTACK,
		ROLL,
		IDLE,

		COUNT,
	};

	struct anim_and_dir_t {
		anim_t anim = NONE;
		direction_t dir = direction_t::NONE;

		u32 get_num() const {
			return (static_cast<u32>(anim) - 1) * 4 + (static_cast<u32>(dir) - 1);
		}
	};

	// возвращает анимацию
	static anim_and_dir_t get_anim(dot dir, anim_and_dir_t type) {
		// дефолтная анимация
		if (type.anim == NONE) {
			return { IDLE, direction_t::RIGHT };
		}
		// run
		else if (dir.x < 0) {
			return { RUN, direction_t::LEFT };
		}
		else if (dir.x > 0) {
			return { RUN, direction_t::RIGHT };
		}
		else if (dir.y < 0) {
			return { RUN, direction_t::DOWN };
		}
		else if (dir.y > 0) {
			return { RUN, direction_t::UP };
		}
		// idle
		else {
			return { IDLE, type.dir };
		}
	}
};

// |dp.x/y| <= 33.(3)

struct Player {

	inline static const game_object_state gobj_state = game_object_state(200, 10, 0.5);

	serialization_traits_byte(Player);

	// move and pos
	dot pos;
	dot dp;

	// info
	s16 max_hp = gobj_state.hp;
	s16 hp = max_hp;
	s16 damage = 70;
	s16 exp = 0;

	// каждый уровень:
	// max_hp += 30
	// damage += 2
	// exp -= 50
	s16 lvl = 0;
	
	// state
	point_t paralyzed_cooldown_acc = PLAYER_STATICPOS_COOLDOWN;
	point_t jump_cooldown_acc = PLAYER_JUMP_COOLDOWN;
	point_t time_between_attack = PLAYER_ATTACK_COOLDOWN;
	point_t time_recovery_hp = 0;
	point_t time_jump = 0;

	s8 cnt_combo = 0;

	bool now_is_attached = false;

	bool is_jumped = false;
	bool is_attack = false;
	bool is_paralyzed = false;

	// animations
	animation anim;
	Player_anim_tree::anim_and_dir_t anim_type;

	char name[12];
	s8 id = 0;

	Player() {}
	Player(dot pos) {
		this->pos = pos;
	}

	void draw() const {
		draw_sprite(pos + dot(-7, 4) * gobj_state.size, gobj_state.size, SP_MEDIUM_SHADOW);

		anim.draw(pos + PLAYER_DELTA_DRAW_POS, gobj_state.size);

		dot p = pos;
		static_pos_update(p);
		draw_text_align(name, p + dot(0, 29) * gobj_state.size, gobj_state.size * 0.75, WHITE);

		draw_collision_obj(*this);
	}

	void simulate(point_t delta_time, dot ddp, bool is_attached, bool pressed_jump) {

		if (hp <= 0) {
			restart();
		}

		if (is_paralyzed) {
			// игрок не может двигаться
			anim_type.anim = Player_anim_tree::IDLE;
			anim = player_anims[anim_type.get_num()];
			is_attack = now_is_attached = false;
		} 
		else if (is_jumped) {

			// на 50% быстрее бега
			dot dp_tmp = dp;
			simulate_move2d(pos, dp_tmp, dot(), delta_time);

			if (anim.frame_update(delta_time)) {
				is_jumped = false;
				jump_cooldown_acc = 0;

				anim_type.anim = Player_anim_tree::IDLE;

				anim = player_anims[anim_type.get_num()];
			}
		}
		else {

			if (pressed_jump && jump_cooldown_acc >= PLAYER_JUMP_COOLDOWN) {
				is_jumped = true;

				anim_type.anim = Player_anim_tree::ROLL;
				anim = player_anims[anim_type.get_num()];

				if (ddp.getLen() == 0) {
					dp = get_direction(anim_type.dir) * 50;
				}
				else {
					dp = ddp.normalize() * 50;
				}

				return; // out here!
			}

			jump_cooldown_acc += delta_time;

			time_recovery_hp += delta_time;

			if (time_recovery_hp >= PLAYER_RECOVERY_COOLDOWN) {
				time_recovery_hp = 0;
				hp = std::min<s16>(hp + 1, max_hp);
			}

			time_between_attack += delta_time;
			paralyzed_cooldown_acc += delta_time;
			
			// ddp - вектор направления игрока

			// нужно понять какую анимацию воспроизводить

			// игрок начал атаковать
			if (is_attached && !is_attack) {

				bool new_attack = false;

				if (time_between_attack <= 0.2 && cnt_combo < PLAYER_MAX_COMBO - 1) {
					new_attack = true;
					cnt_combo++;
				}
				else if (time_between_attack >= PLAYER_ATTACK_COOLDOWN) {
					new_attack = true;
					cnt_combo = 0;
				}

				if (new_attack) {
					is_attack = true;
					now_is_attached = true;

					anim_type.anim = Player_anim_tree::ATTACK;
					anim = player_anims[anim_type.get_num()];

					dp = dot();
				}
			}

			if (is_attack) {

				// удар закончился
				if (anim.frame_update(delta_time)) {
					now_is_attached = is_attack = false;
					anim_type.anim = Player_anim_tree::IDLE;
					anim = player_anims[anim_type.get_num()];
					time_between_attack = 0;
				}
			}
			else {

				// simulate move
				{
					ddp *= PLAYER_DDP_SPEED;

					simulate_move2d(pos, dp, ddp, delta_time);
				}

				// текущая анимация+
				auto current_anim = Player_anim_tree::get_anim(ddp.normalize(), anim_type);

				if (anim_type.get_num() != current_anim.get_num()) { // у нас сменились анимации
					is_attack = false;

					anim_type = current_anim;

					anim = player_anims[current_anim.get_num()];

					// начинаем с 1 кадра, чтобы мы сразу начинали движение
					anim.frame_count = std::min<u32>(1, anim.frame_size - 1);
				}

				anim.frame_update(delta_time);
			}
		}
	}

	// возвращает направление анимации игрока
	dot get_dir() const {
		return get_direction(anim_type.dir);
	}

	collision_circle get_collision() const {
		return Circle(pos, PLAYER_COLLISION_RADIUS);
	}

	collision_box get_attack_collision() const {
		collision_box box;

		switch (anim_type.dir) {
			case direction_t::RIGHT: {
				box = collision_box(pos + dot(0, 14), pos + dot(15, -4));
			}break;
			case direction_t::LEFT: {
				box = collision_box(pos + dot(-15, 14), pos + dot(0, -4));
			}break;
			case direction_t::UP: {
				box = collision_box(pos + dot(-9, 20), pos + dot(9, 0));
			}break;
			case direction_t::DOWN: {
				box = collision_box(pos + dot(-9, 3), pos + dot(9, -14));
			}break;
		}

		return box;
	}

	bool attack_trigger(const dot& p) {

		return is_attack && now_is_attached && // attached
			anim.frame_count >= 1 && // animation checked
			get_attack_collision().trigger(p); // collision checked
	}

	// вернет правду, если атака кого-то зацепила
	template<typename container_t>
	bool simulate_attack(container_t& Enemies) {

		bool is_attack = false;

		for (s32 i = 0; i < Enemies.size(); i++) {
			if (attack_trigger(Enemies[i].pos) && reinterpret_cast<char*>(&Enemies[i]) != reinterpret_cast<char*>(this)) {
				is_attack = true;

				Enemies[i].simulate_hit(*this);

				if (Enemies[i].hp <= 0 && typeid(Enemies[i]) != typeid(Player)) {
					exp += Enemies[i].gobj_state.exp_cnt;
					Enemies.erase(Enemies.begin() + i);
					i--;

					if (exp >= 10) {
						max_hp += 15;
						damage += 2;
						exp -= 10;
						lvl++;
					}
				}
			}
		}

		return is_attack;
	}

	void restart() {

		add_death_effect(pos + dot(-16, 22) * gobj_state.size);

		std::uniform_int_distribution<s32> random_x(-world_half_size.x, world_half_size.x);
		std::uniform_int_distribution<s32> random_y(-world_half_size.y, world_half_size.y);

		auto random_dot = [&]() -> dot {
			return dot(random_x(rnd), random_y(rnd));
		};

		Player nplayer(random_dot());
		nplayer.id = id;
		for (int i = 0; i < sizeof(name); i++) {
			nplayer.name[i] = name[i];
		}
		*this = std::move(nplayer);

		/*pos = random_dot();
		dp = dot();
		max_hp = hp = gobj_state.hp;
		damage = 70;
		lvl = 0;


		paralyzed_cooldown_acc = PLAYER_STATICPOS_COOLDOWN;
		jump_cooldown_acc = PLAYER_JUMP_COOLDOWN;
		time_between_attack = PLAYER_ATTACK_COOLDOWN;
		time_recovery_hp = 0;
		time_jump = 0;

		cnt_combo = 0;

		now_is_attached = false;

		is_jumped = false;
		is_attack = false;
		is_paralyzed = false;*/
	}

	void simulate_hit(Player& player) {
		add_hit_effect(pos + dot(0, 10) * gobj_state.size);

		hp -= player.damage;

		if (hp <= 0) {
			restart();
			player.exp += gobj_state.exp_cnt;

			if (player.exp >= 10) {
				player.max_hp += 15;
				player.damage += 2;
				player.exp -= 10;
				player.lvl++;
			}
		}
		else {
			dp += player.get_dir() * 50;
		}
	}
};

std::vector<Player> Players = {
	Player(),
};
