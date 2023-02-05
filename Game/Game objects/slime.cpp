// visibility
#define SLIME_ALPHA 210
#define SLIME_DELTA_DRAW_POS dot(-30, 38) * gobj_state.size
#define SLIME_FRAME_DURATION 1.0 / 7

// animations
#define SLIME_ANIM_ATTACK animation(SP_SLIME, 25, 30, SLIME_FRAME_DURATION, 64)
#define SLIME_ANIM_IDLE animation(SP_SLIME, 0, 24, SLIME_FRAME_DURATION, 64)

#define SLIME_PUSH_DP 400


struct Slime {

	inline static const game_object_state gobj_state = game_object_state(150, 5, 0.4);

	inline static const enemy_state enemy_state = enemy_state(100, 3, 30, 40, 8, 250, 0.3, 3, 2, 3);

	serialization_traits_byte(Slime);

	// pos and move
	dot pos;
	dot dp;
	dot ddp;

	dot walk_to;

	// cooldowns
	point_t attack_cooldown_accum = enemy_state.attack_cooldown;
	point_t paralyzed_cooldown_accum = enemy_state.paralyzed_cooldown;
	point_t walk_accum = 0;

	// settings
	s16 hp = gobj_state.hp;
	s16 target = -1; // цели преследования нет

	animation anim = SLIME_ANIM_IDLE;

	// boolean
	bool is_attack = false;

	Slime(){}
	Slime(const dot& p) {
		walk_to = pos = p;
		std::uniform_int_distribution<s64> random_range(1, 1000);
		walk_accum = (random_range(rnd) / 1000.0) * enemy_state.walk_sum_time;
	}

	collision_circle get_collision() const {
		return Circle(pos, enemy_state.collision_radius);
	}

	void simulate(point_t delta_time) {
		paralyzed_cooldown_accum += delta_time;
		attack_cooldown_accum += delta_time;

		if (paralyzed_cooldown_accum < enemy_state.paralyzed_cooldown) {
			simulate_move2d(pos, dp, ddp, delta_time);
		}
		else {
			ddp = dot();

			if (is_attack) {

				Player& player = Players[target];

				// анимация атаки закончилась
				if (anim.frame_update(delta_time)) {
					// idle animation beginner

					player.is_paralyzed = is_attack = false; // позиция игрока не статична и не анимация атаки

					attack_cooldown_accum = 0; // cooldown reset

					anim = SLIME_ANIM_IDLE;
				}

				// шарик лопнул
				if (anim.frame_count > 25 && player.is_paralyzed) {

					// push player
					player.dp = Circle(dot(), SLIME_PUSH_DP).get_random_dot();

					player.is_paralyzed = false; // у игрока не статическая позиция
					player.paralyzed_cooldown_acc = 0; // перезарядка

					player.hp -= enemy_state.damage;

					add_hit_effect(player.pos + dot(-8, 16) * player.gobj_state.size);
				}
			}
			else {
				anim.frame_update(delta_time);

				auto bad_target = [&](s16 target, point_t radius) -> bool {
					ASSERT(target >= 0, "wrong target");

					return Players[target].is_paralyzed || Players[target].is_jumped ||
						(Players[target].pos - pos).getLen() > radius;
				};

				// цель потеряна
				if (target != -1 && bad_target(target, enemy_state.persec_radius)) {
					target = -1;
					walk_accum = enemy_state.walk_time;
				}

				if (target == -1) { // цели нет

					// поищем

					// найдем самого близжайшего подходящего игрока
					for(s32 i = 0; i < Players.size(); i++) {

						if(!bad_target(i, target != -1 ? (Players[target].pos - pos).getLen() : enemy_state.locator_radius)){
							target = i;
						}
					}
				}

				if (target != -1) {

					auto& player = Players[target];

					move_to2d(pos, player.pos, dp, (player.pos - pos).normalize() * enemy_state.ddp_speed, delta_time);

					// игрока никто не ест и мы близко к игроку и
					if (!player.is_paralyzed && (player.pos - pos).getLen() <= enemy_state.jump_radius &&

						// и перезарядка атаки прошла и перезарядка игрока тоже
						attack_cooldown_accum >= enemy_state.attack_cooldown && player.paralyzed_cooldown_acc >= PLAYER_STATICPOS_COOLDOWN) {

						// attack animation beginner

						player.is_paralyzed = is_attack = true; // игрок не может двигаться и у нас анимация атаки

						pos = player.pos; // прыгаем на игрока

						anim = SLIME_ANIM_ATTACK;
					}
				}
				else {
					// цели нет. Можем погулять

					walk_accum += delta_time;

					if (pos == walk_to) {
						walk_to = Circle(pos, 20).get_random_dot();
					}

					if (walk_accum > enemy_state.walk_sum_time) {
						walk_accum = 0;
						walk_to = Circle(pos, 20).get_random_dot();
					}

					if (walk_accum < enemy_state.walk_time) {
						move_to2d(pos, walk_to, dp, (walk_to - pos).normalize() * enemy_state.ddp_speed, delta_time);
					}
				}
			}
		}
	}

	void draw() const {

		if (is_attack) {
			if (is_between<u8>(9, anim.frame_count, 25)) {
				draw_sprite(pos + SLIME_DELTA_DRAW_POS, gobj_state.size, SP_SLIME_LARGE_SHADOW, shadow_pixel_func);
			}
			else {
				draw_sprite(pos + SLIME_DELTA_DRAW_POS, gobj_state.size, SP_SLIME_MEDIUM_SHADOW, shadow_pixel_func);
			}
			anim.draw(pos + SLIME_DELTA_DRAW_POS, gobj_state.size, alpha_pixel_func<SLIME_ALPHA>);
		}
		else {
			draw_sprite(pos + SLIME_DELTA_DRAW_POS, gobj_state.size, SP_SLIME_MEDIUM_SHADOW, shadow_pixel_func);

			anim.draw(pos + SLIME_DELTA_DRAW_POS, gobj_state.size, [&](Color color) {
				return paralyzed_cooldown_accum < enemy_state.paralyzed_cooldown ? 
					WHITE :
					Color(color.operator u32(), SLIME_ALPHA);
			});
		}
		
		draw_collision_obj(*this);

		draw_hp(*this);

		if (eng_state.show_locator()) {
			dot p = pos;
			static_pos_update(p);

			draw_circle(Circle(p, enemy_state.persec_radius), Color(0xf000f0, 64));

			draw_circle(Circle(p, enemy_state.locator_radius), Color(0xfff000, 32));

			draw_circle(Circle(p, enemy_state.jump_radius), Color(0xff0000, 16));

			draw_object(walk_accum, p, 0.4, 0xffffffff);
			
			p = walk_to;
			static_pos_update(p);

			draw_rect(p, dot(0.3, 0.3), 0xffff0000);
		}
	}

	void simulate_hit(const Player& player) {
		if (is_attack) {
			return;
		}

		add_hit_effect(pos + dot(-8, 8) * gobj_state.size);

		hp -= player.damage;

		if (hp <= 0) {
			add_death_effect(pos + dot(-15, 15) * gobj_state.size);
		}
		else {
			ddp += player.get_dir() * enemy_state.ddp_speed * 1.5;
			paralyzed_cooldown_accum = 0;
		}
	}
};
