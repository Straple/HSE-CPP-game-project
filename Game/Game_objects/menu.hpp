#ifndef GAME_MENU_HPP
#define GAME_MENU_HPP
enum game_mode_t {
    GM_GAME,
    GM_CUSTOMIZATION,

    GM_MAIN_MENU,
    GM_MAIN_MENU_SETTINGS,

    GM_PAUSE_MENU,
    GM_SETTINGS_MENU,

    GM_MULTIPLAYER,
    GM_MULTIPLAYER_MENU,
};

game_mode_t game_mode = GM_MAIN_MENU;
bool now_is_typing = false;
#include "../../objects.hpp"
#include "../../render.hpp"
#include "../../sprites.hpp"
#include "game_objects.hpp"
#include <functional>

struct Panel {
    Dot pos;
    efloat size;
    std::string text;
    bool is_interactive;
    Color color=Color(0xff2C2C2C);
    Color focus_color = Color(0xFFFFFFFF);
    std::function<void()> function;
    bool focus = false;
    CollisionBox collision;
    Panel(Dot pos_, efloat size_, std::string text_, bool is_interactive_, Color color_, std::function<void()> function_ = []() {return; }) :pos(pos_), size(size_), text(std::move(text_)), is_interactive(is_interactive_), color(color_), function(function_){
        int len = text_len_in_pixels(text.c_str());
        collision.p0 = Dot(pos_.x + (0 ? -(size * len) * .5f : 0), pos_.y);
        collision.p1.x = pos_.x + size * len - size + (0 ? -(size * len) * .5f : 0);
        collision.p1.y = pos_.y - size * 6 - size;
    }
    void draw(Cursor& cursor) {
        update_focus(cursor);
        draw_text(text.c_str(), pos, size, (focus) ? focus_color : color);
        if (/*�� ���� ��� debug ����� ���*/false) {
            draw_rect(collision.p0, Dot(0.5, 0.5), Color(0xff00ff00));
            draw_rect(collision.p1, Dot(0.5, 0.5), Color(0xff00ff00));
        }
    }
    void update_focus(Cursor& cursor) {
        if (!is_interactive) {
            return;
        }
        if (collision.trigger(cursor.pos)) {
            cursor.focus = true;
            focus = true;
        }
        else {
            focus = false;
        }
    }
    void simulate_button(Cursor& cursor,const Input& input) {
        update_focus(cursor);
        if (focus && (PRESSED(BUTTON_MOUSE_L))) {
            function();
        }
    }
};

struct Menu {
    std::vector<Panel> panels;
    Menu(std::vector<Panel> panels_):panels(std::move(panels_)){}
    void draw(Cursor& cursor) {
        for (auto panel : panels) {
            panel.draw(cursor);
        }
    }
    void simulate(Cursor& cursor,const Input&input) {
        for (auto panel : panels) {
            panel.simulate_button(cursor,input);
        }
    }
};
namespace menu_functions {
    //Функции так вынес, если вдруг потребуется в них что то большее чем поменять режим, да и так вроде удобнее.
    void pause_exit_button_function() {
        game_mode = GM_MAIN_MENU;
        //TODO
        //Нужно чтобы все чистилось,сохранялось и тд, но думаю это нам пока что не нужно.
    }
    void set_settings_gm_function() {
        game_mode = GM_SETTINGS_MENU;
    }
    void main_menu_single_game_button_function() {
        game_mode = GM_GAME;
    }
    void main_menu_exit_button_function() {
        global_variables::running = false;
    }
    void settings_exit_button_function() {
        game_mode = GM_PAUSE_MENU;
    }
    void continue_button_function() {
        game_mode = GM_GAME;
    }
    void to_multiplayer_menu_gm() {
        game_mode = GM_MULTIPLAYER_MENU;
    }
    void multi_menu_exit_button_function() {
        game_mode = GM_MAIN_MENU;
    }
    void settings_in_main_menu() {
        game_mode = GM_MAIN_MENU_SETTINGS;
    }
}
static inline Menu in_game_menu(
    std::vector{
        Panel(Dot(-10.7,59.3),1.0,"Pause",false,Color(0xff2c2c2c)),
        Panel(Dot(-10,60),1.0,"Pause",false,Color(0xffbcbcbc)),
        Panel(Dot(-120.5,-0.5),0.6,"Continue",false,Color(0xff2c2c2c)),
        Panel(Dot(-120,0),0.6,"Continue",true,Color(0xffbcbcbc),menu_functions::continue_button_function),
        Panel(Dot(-120.5,-10),0.6,"Settings",false,Color(0xff2c2c2c)),
        Panel(Dot(-120,-9.5),0.6,"Settings",true,Color(0xffbcbcbc),menu_functions::set_settings_gm_function),
        Panel(Dot(-120.5,-20),0.6,"Save game",false,Color(0xff2c2c2c)),
        Panel(Dot(-120,-19.5),0.6,"Save game",true,Color(0xffbcbcbc))/*TODO:*/,
        Panel(Dot(-120.5,-30),0.5,"Exit",false,Color(0xff2c2c2c)),
        Panel(Dot(-120,-29.5),0.5,"Exit",true,Color(0xffbcbcbc),menu_functions::pause_exit_button_function)
    }
);
static inline Menu main_menu(
    std::vector{
        Panel(Dot(-26.7,59.3),1.0,"Bulletproof",false,Color(0xff2c2c2c)),
        Panel(Dot(-26,60),1.0,"Bulletproof",false,Color(0xffbcbcbc)),
        Panel(Dot(-120.5,-10),0.6,"Singleplayer",false,Color(0xff2c2c2c)),
        Panel(Dot(-120,-9.5),0.6,"Singleplayer",true,Color(0xffbcbcbc),menu_functions::main_menu_single_game_button_function),
        Panel(Dot(-120.5,-20),0.6,"Multiplayer",false,Color(0xff2c2c2c)),
        Panel(Dot(-120,-19.5),0.6,"Multiplayer",true,Color(0xffbcbcbc),menu_functions::to_multiplayer_menu_gm),
        Panel(Dot(-120.5,-30),0.6,"Settings",false,Color(0xff2c2c2c)),
        Panel(Dot(-120,-29.5),0.6,"Settings",true,Color(0xffbcbcbc),menu_functions::settings_in_main_menu),
        Panel(Dot(-120.5,-40),0.5,"Exit",false,Color(0xff2c2c2c)),
        Panel(Dot(-120,-39.5),0.5,"Exit",true,Color(0xffbcbcbc),menu_functions::main_menu_exit_button_function)
    }


);
static inline Menu settings_menu(
    std::vector{
        Panel(Dot(-26.8,59.8),1.0,"Settings",false,Color(0xff2c2c2c)),
        Panel(Dot(-26,60),1.0,"Settings",false,Color(0xffbcbcbc)),
        Panel(Dot(-14.5,19.5),0.6,"Volume",false,Color(0xff2c2c2c)),
        Panel(Dot(-14,20),0.6,"Volume",false,Color(0xffbcbcbc)),
        Panel(Dot(-14.5,9.5),0.6,"Tut dolzhen bit polzunik",false,Color(0xff2c2c2c)),
        Panel(Dot(-14,10),0.6,"Tut dolzhen bit polzunik",false,Color(0xffbcbcbc)),
    });
static inline Menu settings_menu_in_main(
    std::vector{
        Panel(Dot(-26.8,59.8),1.0,"Settings",false,Color(0xff2c2c2c)),
        Panel(Dot(-26,60),1.0,"Settings",false,Color(0xffbcbcbc)),
        Panel(Dot(-14.5,19.5),0.6,"Volume",false,Color(0xff2c2c2c)),
        Panel(Dot(-14,20),0.6,"Volume",false,Color(0xffbcbcbc)),
        Panel(Dot(-14.5,9.5),0.6,"Tut dolzhen bit polzunik",false,Color(0xff2c2c2c)),
        Panel(Dot(-14,10),0.6,"Tut dolzhen bit polzunik",false,Color(0xffbcbcbc)),
        Panel(Dot(-120.5,-40),0.5,"Exit",false,Color(0xff2c2c2c)),
        Panel(Dot(-120,-39.5),0.5,"Exit",true,Color(0xffbcbcbc),menu_functions::multi_menu_exit_button_function)
    });
static inline Menu multiplayer_menu(
    std::vector{
        Panel(Dot(-26.8,59.8),1.0,"Multiplayer",false,Color(0xff2c2c2c)),
        Panel(Dot(-26,60),1.0,"Multiplayer",false,Color(0xffbcbcbc)),
        Panel(Dot(-28.5,39.5),0.6,"Press ENTER for typing",false,Color(0xff2c2c2c)),
        Panel(Dot(-28,40),0.6,"Press ENTER for typing",false,Color(0xffbcbcbc)),
        Panel(Dot(-120.5,-40),0.5,"Exit",false,Color(0xff2c2c2c)),
        Panel(Dot(-120,-39.5),0.5,"Exit",true,Color(0xffbcbcbc),menu_functions::multi_menu_exit_button_function)
    });

#endif // GAME_MENU_HPP

