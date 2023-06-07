#ifndef GAME_MENU_HPP
#define GAME_MENU_HPP

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
	void draw() {
		draw_text(text.c_str(), pos, size, (focus) ? focus_color : color);
		if (/*Не знаю как debug режим вкл*/false) {
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
	void simulate_button(Cursor& cursor,Input& input) {
		update_focus(cursor);
		if (focus && (PRESSED(BUTTON_MOUSE_L))) {
			function();
		}
		draw();
	}
};

struct Menu {
	std::vector<Panel> panels;
	Menu(std::vector<Panel> panels_):panels(std::move(panels_)){}
	void draw() {
		for (auto panel : panels) {
			panel.draw();
		}
	}
	void simulate(Cursor& cursor,Input&input) {
		for (auto panel : panels) {
			panel.simulate_button(cursor,input);
		}
	}
};
void pause_exit_button_function() {
	Loot_coins.clear();
	Loot_hearts.clear();
	Bullets.clear();
	Slimes.clear();
	Bats.clear();
	game_mode = GM_MAIN_MENU;

	// TODO: save game and reset all stats like coins or hp
}
void main_menu_new_game_button_function() {
	game_mode = GM_GAME;
	//TODO: 
}
void main_menu_exit_button_function() {
	std::exit(0);
}
static inline Menu in_game_menu(
	std::vector{
		Panel(Dot(-6.5,59.5),1.0,"Pause",false,Color(0xff2c2c2c)),
		Panel(Dot(-7,60),1.0,"Pause",false,Color(0xffbcbcbc)),
		Panel(Dot(90.5,-60),0.5,"Exit",false,Color(0xff2c2c2c)),
		Panel(Dot(90,-59.5),0.5,"Exit",true,Color(0xffbcbcbc),pause_exit_button_function),
		Panel(Dot(78.5,-50),0.6,"Options",false,Color(0xff2c2c2c)),
		Panel(Dot(78,-49.5),0.6,"Options",true,Color(0xffbcbcbc)),
		Panel(Dot(72.5,-40),0.6,"Save game",false,Color(0xff2c2c2c)),
		Panel(Dot(72,-39.5),0.6,"Save game",true,Color(0xffbcbcbc))
	}
);
static inline Menu main_menu(
	std::vector{
		Panel(Dot(-24.5,59.5),1.0,"Bulletproof",false,Color(0xff2c2c2c)),
		Panel(Dot(-25,60),1.0,"Bulletproof",false,Color(0xffbcbcbc)),
		Panel(Dot(-120.5,-10),0.6,"New game",false,Color(0xff2c2c2c)),
		Panel(Dot(-120,-9.5),0.6,"New game",true,Color(0xffbcbcbc),main_menu_new_game_button_function),
		Panel(Dot(-120.5,-60),0.5,"Exit",false,Color(0xff2c2c2c)),
		Panel(Dot(-120,-59.5),0.5,"Exit",true,Color(0xffbcbcbc),main_menu_exit_button_function)
	}


);

#endif // GAME_MENU_HPP

