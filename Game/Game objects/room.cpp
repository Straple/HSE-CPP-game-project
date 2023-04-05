
struct drawing_objects {
    Dot pos;
    efloat size;
    sprite_t sprite;
    int level;

    bool operator<(const drawing_objects& right) {
        return level < right.level;
    }
};

struct Room {

    std::vector<drawing_objects> Draw_objects;
    std::vector<collision_box> Collision_boxes;
    Dot spawn_point;

    void read_room(const std::string & filename) {
        std::ifstream file(filename);
        {
            std::string str;
            file >> str;  // SPRITES

            int count;
            file >> count;
            Draw_objects.assign(count, {});
            for (auto& [pos, size, sprite,level] : Draw_objects) {
                std::string sprite_name;
                file >> sprite_name >> pos >> size>>level;
                sprite = string_to_sprite_type(sprite_name);
            }
        }
        {
            std::string str;
            file >> str >> str;  // COLLISION BOXES

            int count;
            file >> count;
            std::cout << count << std::endl;
            Collision_boxes.assign(count, {});
            for (auto& [p0, p1] : Collision_boxes) {
                file >> p0 >> p1;
            }
        }
    }
    void render_room() {
        clear_screen(GREY);
        int last_beckground_sprite;
        for (int i = 0; i < static_cast<int>(Draw_objects.size()) && Draw_objects[i].level<=0; ++i) {
            auto [pos, size, sprite,level] = Draw_objects[i];
                draw_sprite(pos, size, sprite);
                last_beckground_sprite = i;
        }

        player.draw();
        //Тут будут все сущности, мобы и тд.

        for (int i = last_beckground_sprite + 1; i < static_cast<int>(Draw_objects.size()); ++i) {
            auto [pos, size, sprite, level] = Draw_objects[i];
            draw_sprite(pos, size, sprite);
        }
    }

};
