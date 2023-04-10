static const Dot world_half_size(500, 500);

#include "game_utils.cpp"
// don't shuffle
//#include "UI Objects\ui_objects.cpp"
// don't shuffle
#include "Game objects\game_objects.cpp"

// game objects

std::vector<Bush> Bushes;

std::vector<Slime> Slimes;

std::vector<Tree> Trees;

std::vector<Bat> Bats;

std::vector<Fireplace> Fireplaces = {
    Fireplace(Dot(30, -30)),
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

#include "game_collision.cpp"

#define GAME_OBJECT_MAX 200
#define GAME_OBJECT_MIN 170
#define GAME_ENEMIES_MAX 40
#define GAME_ENEMIES_MIN 30

template <typename T>
void build_array_of_objects(std::vector<T> &Objects, unsigned int size) {
    Objects.resize(size);

    std::uniform_int_distribution<int> random_x(
        -world_half_size.x, world_half_size.x
    );
    std::uniform_int_distribution<int> random_y(
        -world_half_size.y, world_half_size.y
    );

    for (auto &obj : Objects) {
        obj = T(Dot(random_x(rnd), random_y(rnd)));
    }
}

void build_world() {
    std::uniform_int_distribution<int> random_size(
        GAME_OBJECT_MIN, GAME_OBJECT_MAX
    );
    std::uniform_int_distribution<int> random_size_enemies(
        GAME_ENEMIES_MIN, GAME_ENEMIES_MAX
    );

    // build bushes and trees
    {
        build_array_of_objects(Bushes, random_size(rnd));
        build_array_of_objects(Trees, random_size(rnd));

        auto get_coll = [&](auto obj) -> collision_circle {
            auto res = collision_circle(obj.get_collision());
            res.circle.radius += 5;
            return res;
        };

        for (auto &tree1 : Trees) {
            for (auto &tree2 : Trees) {
                if (verify_others_obj(tree1, tree2)) {
                    update_collision(tree1, get_coll(tree2));
                }
            }
        }

        for (auto &bush1 : Bushes) {
            for (auto &bush2 : Bushes) {
                if (verify_others_obj(bush1, bush2)) {
                    update_collision(bush1, get_coll(bush2));
                }
            }
        }

        for (auto &tree : Trees) {
            for (auto &bush : Bushes) {
                if (verify_others_obj(tree, bush)) {
                    update_collision(tree, get_coll(bush));
                }
            }
        }
        for (auto &bush : Bushes) {
            for (auto &tree : Trees) {
                if (verify_others_obj(bush, tree)) {
                    update_collision(bush, get_coll(tree));
                }
            }
        }

        if (!Fireplaces.empty()) {
            for (int i = 0; i < Bushes.size(); i++) {
                if (Bushes[i].get_collision().trigger(
                        Fireplaces[0].get_collision()
                    )) {
                    Bushes.erase(Bushes.begin() + i);
                    i--;
                }
            }

            for (int i = 0; i < Trees.size(); i++) {
                if (Trees[i].get_collision().trigger(
                        Fireplaces[0].get_collision()
                    )) {
                    Trees.erase(Trees.begin() + i);
                    i--;
                }
            }
        }

        for (int i = 0; i < Bushes.size(); i++) {
            for (int j = i + 1; j < Bushes.size(); j++) {
                if (Bushes[i].get_collision().trigger(get_coll(Bushes[j]))) {
                    Bushes.erase(Bushes.begin() + i);
                    i--;
                    break;
                }
            }
        }

        for (int i = 0; i < Trees.size(); i++) {
            for (int j = i + 1; j < Trees.size(); j++) {
                if (Trees[i].get_collision().trigger(get_coll(Trees[j]))) {
                    Trees.erase(Trees.begin() + i);
                    i--;
                    break;
                }
            }
        }
    }

    build_array_of_objects(Slimes, random_size_enemies(rnd));
    build_array_of_objects(Bats, random_size_enemies(rnd));
}

// UI objects



void simulate_player(const Input &input, efloat delta_time) {
    // накопление вектора движения
    auto accum_ddp = [&input](
                         button_t left, button_t right, button_t top,
                         button_t bottom
                     ) -> Dot {
        return Dot(
            is_down(right) - is_down(left), is_down(top) - is_down(bottom)
        );
    };

    player.simulate(
        delta_time, accum_ddp(BUTTON_A, BUTTON_D, BUTTON_W, BUTTON_S),
        is_down(BUTTON_J), pressed(BUTTON_SPACE)
    );

    // player attack
    {
        if (/*player.simulate_attack(Logs) | */ player.simulate_attack(Slimes) |
            player.simulate_attack(Bats) | player.simulate_attack(Trees)) {
            player.now_is_attached = false;
        }
    }
}

void simulate_physics(efloat delta_time) {
    // add new world objects
    {
        std::uniform_int_distribution<int> random_x(
            -world_half_size.x, world_half_size.x
        );
        std::uniform_int_distribution<int> random_y(
            -world_half_size.y, world_half_size.y
        );
        std::uniform_int_distribution<int> range(1, 3);

        auto random_dot = [&]() -> Dot {
            return Dot(random_x(rnd), random_y(rnd));
        };

        if (Slimes.size() < GAME_ENEMIES_MIN) {
            int cnt = range(rnd);

            while (cnt--) {
                Slimes.push_back(random_dot());
                // std::cout << "new slime: " << Slimes.back().pos << "\n";
            }
        }

        if (Bats.size() < GAME_ENEMIES_MIN) {
            int cnt = range(rnd);

            while (cnt--) {
                Bats.push_back(random_dot());
                // std::cout << "new bat: " << Bats.back().pos << "\n";
            }
        }

        if (Trees.size() < GAME_OBJECT_MIN) {
            int cnt = range(rnd);

            while (cnt--) {
                Trees.emplace_back(random_dot());
                // std::cout << "new tree: " << Trees.back().pos << "\n";
            }

            auto get_coll = [&](auto obj) -> collision_circle {
                auto res = collision_circle(obj.get_collision());
                res.circle.radius += 5;
                return res;
            };

            for (auto &tree1 : Trees) {
                for (auto &tree2 : Trees) {
                    if (verify_others_obj(tree1, tree2)) {
                        update_collision(tree1, get_coll(tree2));
                    }
                }
            }

            for (auto &tree : Trees) {
                for (auto &bush : Bushes) {
                    if (verify_others_obj(tree, bush)) {
                        update_collision(tree, get_coll(bush));
                    }
                }
            }
        }
    }

    // simulate logs
    {
        for (auto &log : Logs) {
            log.simulate(delta_time);
        }
        simulate_game_collision(Logs);
    }

    // simulate slimes
    {
        for (auto &slime : Slimes) {
            slime.simulate(delta_time);
        }

        simulate_game_collision(Slimes);
    }

    // simulate bats
    {
        for (auto &bat : Bats) {
            bat.simulate(delta_time);
        }

        simulate_game_collision(Bats);
    }

    // simulate fireplaces
    {
//        for (auto &fireplace : Fireplaces) {
//            fireplace.simulate(delta_time);
//        }
    }

    // simulate effects
    {
        for (unsigned int i = 0; i < Effects.size(); i++) {
            if (Effects[i].simulate(delta_time)) {
                Effects.erase(Effects.begin() + i);
                i--;
            }
        }
    }
}

void render_game() {
    clear_screen(0xff000000);

    draw_texture(
        Dot(-world_half_size.x, world_half_size.y), 32, 32, 0.5,
        SP_GRASS_BACKGROUND
    );

    // draw players, slimes, bats,
    // bushes, trees, fireplaces,
    // logs
    {
        // top sort

        struct top_sort_object {
            enum type_object {
                TO_UNDEFIND,

                TO_PLAYER,
                TO_BUSH,
                TO_SLIME,
                TO_TREE,
                TO_LOG,
                TO_BAT,
                TO_FIREPLACE,
            };

            type_object type;
            const void *ptr;

            top_sort_object() {
                type = TO_UNDEFIND;
                ptr = nullptr;
            }

            explicit top_sort_object(const Player &player) {
                type = TO_PLAYER;
                ptr = reinterpret_cast<const void *>(&player);
            }

            explicit top_sort_object(const Fireplace &fireplace) {
                type = TO_FIREPLACE;
                ptr = reinterpret_cast<const void *>(&fireplace);
            }

            explicit top_sort_object(const Bat &player) {
                type = TO_BAT;
                ptr = reinterpret_cast<const void *>(&player);
            }

            explicit top_sort_object(const Log &log) {
                type = TO_LOG;
                ptr = reinterpret_cast<const void *>(&log);
            }

            explicit top_sort_object(const Bush &bush) {
                type = TO_BUSH;
                ptr = reinterpret_cast<const void *>(&bush);
            }

            explicit top_sort_object(const Slime &slime) {
                type = TO_SLIME;
                ptr = reinterpret_cast<const void *>(&slime);
            }

            explicit top_sort_object(const Tree &tree) {
                type = TO_TREE;
                ptr = reinterpret_cast<const void *>(&tree);
            }

            [[nodiscard]] efloat get_y() const {
                switch (type) {
                    case TO_PLAYER: {
                        return reinterpret_cast<const Player *>(ptr)->pos.y;
                    }
                    case TO_BUSH: {
                        return reinterpret_cast<const Bush *>(ptr)->pos.y;
                    }
                    case TO_SLIME: {
                        return reinterpret_cast<const Slime *>(ptr)->pos.y;
                    }
                    case TO_TREE: {
                        return reinterpret_cast<const Tree *>(ptr)->pos.y;
                    }
                    case TO_LOG: {
                        return reinterpret_cast<const Log *>(ptr)->pos.y;
                    }
                    case TO_BAT: {
                        return reinterpret_cast<const Bat *>(ptr)->pos.y;
                    }
                    case TO_FIREPLACE: {
                        return reinterpret_cast<const Fireplace *>(ptr)->pos.y;
                    }
                    default:
                        ASSERT(false, "undefind object type");
                }
                return 0;
            }

            void draw() const {
                switch (type) {
                    case TO_PLAYER: {
                        reinterpret_cast<const Player *>(ptr)->draw();
                    } break;
                    case TO_BUSH: {
                        reinterpret_cast<const Bush *>(ptr)->draw();
                    } break;
                    case TO_SLIME: {
                        reinterpret_cast<const Slime *>(ptr)->draw();
                    } break;
                    case TO_TREE: {
                        reinterpret_cast<const Tree *>(ptr)->draw();
                    } break;
                    case TO_LOG: {
                        reinterpret_cast<const Log *>(ptr)->draw();
                    } break;
                    case TO_BAT: {
                        reinterpret_cast<const Bat *>(ptr)->draw();
                    } break;
                    case TO_FIREPLACE: {
                        reinterpret_cast<const Fireplace *>(ptr)->draw();
                    } break;
                    case TO_UNDEFIND: {
                        ASSERT(false, "undefind object type");
                    }
                }
            }

            bool operator<(const top_sort_object &Rhs) const {
                return get_y() > Rhs.get_y();
            }
        };

        std::vector<top_sort_object> Objects;
        Objects.emplace_back(player);
        for (auto &fireplace : Fireplaces) {
            Objects.emplace_back(fireplace);
        }
        for (auto &slime : Slimes) {
            Objects.emplace_back(slime);
        }
        for (auto &bush : Bushes) {
            Objects.emplace_back(bush);
        }
        for (auto &tree : Trees) {
            Objects.emplace_back(tree);
        }
        for (auto &log : Logs) {
            Objects.emplace_back(log);
        }
        for (auto &bat : Bats) {
            Objects.emplace_back(bat);
        }

        std::stable_sort(Objects.begin(), Objects.end());

        for (auto &obj : Objects) {
            obj.draw();
        }
    }

    // draw effects
    {
        for (auto &Effect : Effects) {
            Effect.draw();
        }
    }

    // hp
    /*ui_state(
        Dot(5 - global_variables::arena_half_size.x,
    global_variables::arena_half_size.y - 5), Dot(25 -
    global_variables::arena_half_size.x, global_variables::arena_half_size.y
    - 7.5)
    )
        .draw(player.hp, player.max_hp, GREY, RED);

    // exp
    ui_state(
        Dot(5 - global_variables::arena_half_size.x,
    global_variables::arena_half_size.y - 10), Dot(25 -
    global_variables::arena_half_size.x, global_variables::arena_half_size.y
    - 12.5)
    )
        .draw(player.exp, int(10), GREY, YELLOW);

    // damage
    draw_object(
        player.damage, Dot(5 - global_variables::arena_half_size.x,
    global_variables::arena_half_size.y - 15), 0.6, BLUE
    );

    draw_object(
        player.lvl, Dot(5 - global_variables::arena_half_size.x,
    global_variables::arena_half_size.y - 20), 0.6, PURPLE
    );*/

    cursor.draw();
}

template <typename func_t>
void simulate_input(const Input &input, func_t &&window_mode_callback) {
    if (pressed(BUTTON_ESC)) {
        global_variables::running = false;
        return;
    }

    if (pressed(BUTTON_ENTER)) {
        window_mode_callback();
    }

    if (pressed(BUTTON_TAB)) {
        global_variables::debug_mode = !global_variables::debug_mode;
    }

    if (pressed(BUTTON_K)) {
        global_variables::show_locator = !global_variables::show_locator;
    }

    if (pressed(BUTTON_F)) {
        global_variables::show_fps = !global_variables::show_fps;
    }

    // update render_scale
    {
        if (is_down(BUTTON_UP)) {
            increase_window_scaling(cursor.pos);
        }

        if (is_down(BUTTON_DOWN)) {
            decrease_window_scaling(cursor.pos);
        }
    }

    cursor.simulate(input);
}

template <typename func_t>
void simulate_game(
    const Input &input,
    efloat delta_time,
    func_t &&window_mode_callback
) {
    simulate_input(input, window_mode_callback);
    if (!global_variables::running) {
        return;
    }

    global_variables::camera.simulate(player.pos, delta_time);

    // simulate players
    {
        simulate_player(input, delta_time);

        // simulate_game_collision(player);
    }

    simulate_physics(delta_time);

    cursor.simulate(input);

    // clear_screen(BLACK);
    // draw_sprite(Dot(), 0.1, SP_KEK);

    for (auto &bush : Bushes) {
        bush.pos = player.get_collision().bubble(bush.get_collision());
        // player.pos = bush.get_collision().bubble(player.get_collision());
    }

    render_game();

    // bullet!
    // TODO: нужно это все потом перенести в simulate_player, render и т.п.
    {
        static std::vector<Bullet> Bullets;

//        draw_sprite(player.pos, 0.1, SP_TREE);
//        draw_sprite(mouse.pos + camera.pos, 0.1, SP_TREE);
        static efloat last_hit_time = 3;
        if (pressed(BUTTON_MOUSE_L) && last_hit_time >= PLAYER_ATTACK_COOLDOWN ) {
            // new bullet!
            if (-1 > player.random_shot+player.random_shot_multiplyer || player.random_shot+player.random_shot_multiplyer > 1) {
                player.random_shot_multiplyer*=-1;
            }
            player.random_shot += player.random_shot_multiplyer;
//            std::cout << player.ind << ' ' << player.bullet_delta << ' ';
//            std::cout << player.ind << ' ' << player.bullet_delta << std::endl;
            last_hit_time = 0;
            Bullets.emplace_back(
                // player.pos,
                player.get_collision()
                    .circle.pos + BulletDeltas[(player.ind+player.random_shot+24)%24],  // центрированная позиция относительно спрайта
                cursor.pos + global_variables::camera.pos, 1000000000, 1000
            );
        }
        last_hit_time += delta_time;


        for (auto &bullet : Bullets) {
            bullet.simulate(delta_time);
        }

        for (auto &bullet : Bullets) {
            bullet.draw();
        }

    for (int i = 0; i < Bullets.size(); i++) {
            bool attack1 = Bullets[i].simulate_attack(player, Bats);
            bool attack2 = Bullets[i].simulate_attack(player, Slimes);
            if (attack1 || attack2) {
                Bullets.erase(Bullets.begin()+i);
                i--;
            }
        }
    }
    for (auto& loot: Loot_objects) {
        loot->draw();
    }

    for (int i = 0; i < Loot_objects.size(); i++) {
        auto& object = Loot_objects[i];
        object->simulate(delta_time);
        if (object->simulate_collection()) {
            Loot_objects.erase(Loot_objects.begin()+i);
            i--;
        }
    }
    /*player.hp = 300;
    player.pos = Dot();
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            draw_sprite_static(Dot(j, i) * 10 -
    global_variables::arena_half_size, 0.5, SP_TREE);
        }
    }*/

    // p = pos
    // dp = pos'
    // ddp = pos''

    // static Dot pos;
    // static Dot dp;
    // move_to2d(pos, Dot(100, 100), dp, Dot(20, 100), delta_time);

    /*if (is_down(BUTTON_LEFT)) {
        simulate_move2d(pos, dp, Dot(-1, 0) * 1000, delta_time);
    }
    if (is_down(BUTTON_RIGHT)) {
        simulate_move2d(pos, dp, Dot(1, 0) * 1000, delta_time);
    }
    if (is_down(BUTTON_UP)) {
        simulate_move2d(pos, dp, Dot(0, 1) * 1000, delta_time);
    }
    if (is_down(BUTTON_DOWN)) {
        simulate_move2d(pos, dp, Dot(0, -1) * 1000, delta_time);
    }*/

    // draw_rect(pos, Dot(1, 1), RED);

    // draw_spritesheet(Dot(), 1, SS_SLIME, 3);

    // static animation anim(SS_SLIME, 0, 10, 0.1);
    // anim.frame_update(delta_time);
    // anim.draw(Dot(), 1);

    // clear_screen(BLACK);

    // draw_sprite(Dot(), 1, SP_TREE);
    // draw_sprite_static(Dot(), 1, SP_TREE);

    // draw_text("hello world", Dot() - camera.pos, 1, WHITE);

    // draw_rect_in_pixels(0, 0, 100, 100, 0xffffffff);

    // draw_rect2(Dot(), Dot(10, -20), BLACK);

    // alpha 100/255
    // draw_circle(Circle(Dot(), 3), Color(0xff0000, 100));

    // draw_text("hello world", Dot(), 1, WHITE);

    // draw_object("hello world", Dot(), 0.3, 0xffff0000);

    // draw_object(player.is_jumped, Dot(), 1, BLACK);

    /*draw_text_align("1234567890+-!?.,|()", Dot(0, 0), 0.6, 0xffffffffff);
    draw_text_align(
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n\
abcdefghijklmnopqrstuvwxyz",
        Dot(0, -20), 0.6, 0xffffffffff
    );*/
}
