#ifndef GAME_TYPER_HPP
#define GAME_TYPER_HPP
#include <string>
#include "../../objects.hpp"
#include "../../render.hpp"
#include "../../sprites.hpp"
#include "game_objects.hpp"
struct Typer {
	std::string text="bebra";
	Dot pos;
	efloat size;
	Color color;
	bool shift_is_down = false;
	bool focus = false;
	efloat time_a_pressed = 0;
	efloat time_b_pressed = 0;
	efloat time_c_pressed = 0;
	efloat time_d_pressed = 0;
	efloat time_e_pressed = 0;
	efloat time_f_pressed = 0;
	efloat time_g_pressed = 0;
	efloat time_h_pressed = 0;
	efloat time_i_pressed = 0;
	efloat time_j_pressed = 0;
	efloat time_k_pressed = 0;
	efloat time_l_pressed = 0;
	efloat time_m_pressed = 0;
	efloat time_n_pressed = 0;
	efloat time_o_pressed = 0;
	efloat time_p_pressed = 0;
	efloat time_q_pressed = 0;
	efloat time_r_pressed = 0;
	efloat time_s_pressed = 0;
	efloat time_t_pressed = 0;
	efloat time_u_pressed = 0;
	efloat time_v_pressed = 0;
	efloat time_w_pressed = 0;
	efloat time_x_pressed = 0;
	efloat time_y_pressed = 0;
	efloat time_z_pressed = 0;
	efloat time_0_pressed = 0;
	efloat time_1_pressed = 0;
	efloat time_2_pressed = 0;
	efloat time_3_pressed = 0;
	efloat time_4_pressed = 0;
	efloat time_5_pressed = 0;
	efloat time_6_pressed = 0;
	efloat time_7_pressed = 0;
	efloat time_8_pressed = 0;
	efloat time_9_pressed = 0;
	efloat time_backspace_pressed = 0;
	efloat time_dot_pressed = 0;
	efloat time_colon_pressed = 0;
	efloat time_comma_pressed = 0;
	efloat time_q_mark_pressed = 0;
	efloat time_space_pressed = 0;
	efloat time_plus_pressed = 0;
	efloat time_minus_pressed = 0;
	efloat time_limit = 0.25;
	Typer(Dot pos_, efloat size_, Color color_) :pos(pos_), size(size_), color(color_) {

	}
	void draw() {
		draw_text(text.c_str(), pos, size, color);
	}
	void simulate(Input& input,efloat delta_time) {
		if (!global_variables::is_typing||!focus) {
			return;
		}
		
		if (PRESSED(BUTTON_ESC) || PRESSED(BUTTON_ENTER)) {
			focus = false;
			global_variables::is_typing = false;
		}
		shift_is_down = false;
		if (IS_DOWN(BUTTON_SHIFT)) {
			shift_is_down = true;
		}
		if (PRESSED(BUTTON_A)) {
			text += (shift_is_down) ? 'A' : 'a';
		}
		if (PRESSED(BUTTON_B)) {
			text += (shift_is_down) ? 'B' : 'b';
		}
		if (PRESSED(BUTTON_C)) {
			text += (shift_is_down) ? 'C' : 'c';
		}
		if (PRESSED(BUTTON_D)) {
			text += (shift_is_down) ? 'D' : 'd';
		}
		if (PRESSED(BUTTON_E)) {
			text += (shift_is_down) ? 'E' : 'e';
		}
		if (PRESSED(BUTTON_F)) {
			text += (shift_is_down) ? 'F' : 'f';
		}
		if (PRESSED(BUTTON_G)) {
			text += (shift_is_down) ? 'G' : 'g';
		}
		if (PRESSED(BUTTON_H)) {
			text += (shift_is_down) ? 'H' : 'h';
		}
		if (PRESSED(BUTTON_I)) {
			text += (shift_is_down) ? 'I' : 'i';
		}
		if (PRESSED(BUTTON_J)) {
			text += (shift_is_down) ? 'J' : 'j';
		}
		if (PRESSED(BUTTON_K)) {
			text += (shift_is_down) ? 'K' : 'k';
		}
		if (PRESSED(BUTTON_L)) {
			text += (shift_is_down) ? 'L' : 'l';
		}
		if (PRESSED(BUTTON_M)) {
			text += (shift_is_down) ? 'M' : 'm';
		}
		if (PRESSED(BUTTON_N)) {
			text += (shift_is_down) ? 'N' : 'n';
		}
		if (PRESSED(BUTTON_O)) {
			text += (shift_is_down) ? 'O' : 'o';
		}
		if (PRESSED(BUTTON_P)) {
			text += (shift_is_down) ? 'P' : 'p';
		}
		if (PRESSED(BUTTON_Q)) {
			text += (shift_is_down) ? 'Q' : 'q';
		}
		if (PRESSED(BUTTON_R)) {
			text += (shift_is_down) ? 'R' : 'r';
		}
		if (PRESSED(BUTTON_S)) {
			text += (shift_is_down) ? 'S' : 's';
		}
		if (PRESSED(BUTTON_T)) {
			text += (shift_is_down) ? 'T' : 't';
		}
		if (PRESSED(BUTTON_U)) {
			text += (shift_is_down) ? 'U' : 'u';
		}
		if (PRESSED(BUTTON_V)) {
			text += (shift_is_down) ? 'V' : 'v';
		}
		if (PRESSED(BUTTON_W)) {
			text += (shift_is_down) ? 'W' : 'w';
		}
		if (PRESSED(BUTTON_X)) {
			text += (shift_is_down) ? 'X' : 'x';
		}
		if (PRESSED(BUTTON_Y)) {
			text += (shift_is_down) ? 'Y' : 'y';
		}
		if (PRESSED(BUTTON_Z)) {
			text += (shift_is_down) ? "Z" : "z";
		}
		if (PRESSED(BUTTON_1)) {
			text += (shift_is_down) ? "!" : "1";
		}
		if (PRESSED(BUTTON_2)) {
			text += (shift_is_down) ? "" : "2";
		}
		if (PRESSED(BUTTON_3)) {
			text += (shift_is_down) ? "" : "3";
		}
		if (PRESSED(BUTTON_4)) {
			text += (shift_is_down) ? "" : "4";
		}
		if (PRESSED(BUTTON_5)) {
			text += (shift_is_down) ? "" : "5";
		}
		if (PRESSED(BUTTON_6)) {
			text += (shift_is_down) ? "" : "6";
		}
		if (PRESSED(BUTTON_7)) {
			text += (shift_is_down) ? "" : "7";
		}
		if (PRESSED(BUTTON_8)) {
			text += (shift_is_down) ? "" : "8";
		}
		if (PRESSED(BUTTON_9)) {
			text += (shift_is_down) ? "(" : "9";
		}
		if (PRESSED(BUTTON_0)) {
			text += (shift_is_down) ? ")" : "0";
		}
		if (PRESSED(BUTTON_NUMPAD1)) {
			text += (shift_is_down) ? "!" : "1";
		}
		if (PRESSED(BUTTON_NUMPAD2)) {
			text += (shift_is_down) ? "" : "2";
		}
		if (PRESSED(BUTTON_NUMPAD3)) {
			text += (shift_is_down) ? "" : "3";
		}
		if (PRESSED(BUTTON_NUMPAD4)) {
			text += (shift_is_down) ? "" : "4";
		}
		if (PRESSED(BUTTON_NUMPAD5)) {
			text += (shift_is_down) ? "" : "5";
		}
		if (PRESSED(BUTTON_NUMPAD6)) {
			text += (shift_is_down) ? "" : "6";
		}
		if (PRESSED(BUTTON_NUMPAD7)) {
			text += (shift_is_down) ? "" : "7";
		}
		if (PRESSED(BUTTON_NUMPAD8)) {
			text += (shift_is_down) ? "" : "8";
		}
		if (PRESSED(BUTTON_NUMPAD9)) {
			text += (shift_is_down) ? "(" : "9";
		}
		if (PRESSED(BUTTON_NUMPAD0)) {
			text += (shift_is_down) ? ")" : "0";
		}
		if (PRESSED(BUTTON_DOT)) {
			text += (shift_is_down) ? "" : ".";
		}
		if (PRESSED(BUTTON_MINUS)) {
			text += (shift_is_down) ? "_" : "-";
		}
		if (PRESSED(BUTTON_PLUS)) {
			text += (shift_is_down) ? "" : "+";
		}
		if (PRESSED(BUTTON_COMMA)) {
			text += (shift_is_down) ? "" : ",";
		}
		if (PRESSED(BUTTON_QUESTION_MARK)) {
			text += (shift_is_down) ? "?" : "";
		}
		if (PRESSED(BUTTON_SPACE)) {
			text += (shift_is_down) ? " " : " ";
		}
		if (PRESSED(BUTTON_BACKSPACE)) {
			if(!text.empty())
			text.pop_back();
		}
		if (IS_DOWN(BUTTON_A)) {
			time_a_pressed += delta_time;
			if (time_a_pressed >= time_limit) {
				time_a_pressed = 0;
				text += (shift_is_down) ? "A" : "a";
			}
			if (RELEASED(BUTTON_A)) {
				time_a_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_B)) {
			time_b_pressed += delta_time;
			if (time_b_pressed >= time_limit) {
				time_b_pressed = 0;
				text += (shift_is_down) ? "B" : "b";
			}
			if (RELEASED(BUTTON_B)) {
				time_b_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_C)) {
		time_c_pressed += delta_time;
			if (time_c_pressed >= time_limit) {
				time_c_pressed = 0;
				text += (shift_is_down) ? "C" : "c";
			}
			if (RELEASED(BUTTON_C)) {
				time_c_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_D)) {
			time_d_pressed += delta_time;
			if (time_d_pressed >= time_limit) {
				time_d_pressed = 0;
				text += (shift_is_down) ? "D" : "d";
			}
			if (RELEASED(BUTTON_D)) {
				time_d_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_E)) {
			time_e_pressed += delta_time;
			if (time_e_pressed >= time_limit) {
				time_e_pressed = 0;
				text += (shift_is_down) ? "E" : "e";
			}
			if (RELEASED(BUTTON_E)) {
				time_e_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_F)) {
			time_f_pressed += delta_time;
			if (time_f_pressed >= time_limit) {
				time_f_pressed = 0;
				text += (shift_is_down) ? "F" : "f";
			}
			if (RELEASED(BUTTON_F)) {
				time_f_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_G)) {
			time_g_pressed += delta_time;
			if (time_g_pressed >= time_limit) {
				time_g_pressed = 0;
				text += (shift_is_down) ? "G" : "g";
			}
			if (RELEASED(BUTTON_G)) {
				time_g_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_H)) {
			time_h_pressed += delta_time;
			if (time_h_pressed >= time_limit) {
				time_h_pressed = 0;
				text += (shift_is_down) ? "H" : "h";
			}
			if (RELEASED(BUTTON_H)) {
				time_h_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_I)) {
			time_i_pressed += delta_time;
			if (time_i_pressed >= time_limit) {
				time_i_pressed = 0;
				text += (shift_is_down) ? "I" : "i";
			}
			if (RELEASED(BUTTON_I)) {
				time_i_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_J)) {
			time_j_pressed += delta_time;
			if (time_j_pressed >= time_limit) {
				time_j_pressed = 0;
				text += (shift_is_down) ? "J" : "j";
			}
			if (RELEASED(BUTTON_J)) {
				time_j_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_K)) {
			time_k_pressed += delta_time;
			if (time_k_pressed >= time_limit) {
				time_k_pressed = 0;
				text += (shift_is_down) ? "K" : "k";
			}
			if (RELEASED(BUTTON_K)) {
				time_k_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_L)) {
			time_l_pressed += delta_time;
			if (time_l_pressed >= time_limit) {
				time_l_pressed = 0;
				text += (shift_is_down) ? "L" : "l";
			}
			if (RELEASED(BUTTON_L)) {
				time_l_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_M)) {
			time_m_pressed += delta_time;
			if (time_m_pressed >= time_limit) {
				time_m_pressed = 0;
				text += (shift_is_down) ? "M" : "m";
			}
			if (RELEASED(BUTTON_M)) {
				time_m_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_N)) {
			time_n_pressed += delta_time;
			if (time_n_pressed >= time_limit) {
				time_n_pressed = 0;
				text += (shift_is_down) ? "N" : "n";
			}
			if (RELEASED(BUTTON_N)) {
				time_a_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_O)) {
			time_o_pressed += delta_time;
			if (time_o_pressed >= time_limit) {
				time_o_pressed = 0;
				text += (shift_is_down) ? "O" : "o";
			}
			if (RELEASED(BUTTON_O)) {
				time_o_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_P)) {
			time_p_pressed += delta_time;
			if (time_p_pressed >= time_limit) {
				time_p_pressed = 0;
				text += (shift_is_down) ? "P" : "p";
			}
			if (RELEASED(BUTTON_P)) {
				time_p_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_Q)) {
			time_q_pressed += delta_time;
			if (time_q_pressed >= time_limit) {
				time_q_pressed = 0;
				text += (shift_is_down) ? "Q" : "q";
			}
			if (RELEASED(BUTTON_Q)) {
				time_q_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_R)) {
			time_r_pressed += delta_time;
			if (time_r_pressed >= time_limit) {
				time_r_pressed = 0;
				text += (shift_is_down) ? "R" : "r";
			}
			if (RELEASED(BUTTON_R)) {
				time_r_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_S)) {
			time_s_pressed += delta_time;
			if (time_s_pressed >= time_limit) {
				time_s_pressed = 0;
				text += (shift_is_down) ? "S" : "s";
			}
			if (RELEASED(BUTTON_S)) {
				time_s_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_T)) {
			time_t_pressed += delta_time;
			if (time_t_pressed >= time_limit) {
				time_t_pressed = 0;
				text += (shift_is_down) ? "T" : "t";
			}
			if (RELEASED(BUTTON_T)) {
				time_t_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_U)) {
			time_u_pressed += delta_time;
			if (time_u_pressed >= time_limit) {
				time_u_pressed = 0;
				text += (shift_is_down) ? "U" : "u";
			}
			if (RELEASED(BUTTON_U)) {
				time_u_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_V)) {
			time_v_pressed += delta_time;
			if (time_v_pressed >= time_limit) {
				time_v_pressed = 0;
				text += (shift_is_down) ? "V" : "v";
			}
			if (RELEASED(BUTTON_V)) {
				time_v_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_W)) {
			time_w_pressed += delta_time;
			if (time_w_pressed >= time_limit) {
				time_w_pressed = 0;
				text += (shift_is_down) ? "W" : "w";
			}
			if (RELEASED(BUTTON_W)) {
				time_w_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_X)) {
			time_x_pressed += delta_time;
			if (time_x_pressed >= time_limit) {
				time_x_pressed = 0;
				text += (shift_is_down) ? "X" : "x";
			}
			if (RELEASED(BUTTON_X)) {
				time_a_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_Y)) {
			time_y_pressed += delta_time;
			if (time_y_pressed >= time_limit) {
				time_y_pressed = 0;
				text += (shift_is_down) ? "Y" : "y";
			}
			if (RELEASED(BUTTON_Y)) {
				time_y_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_Z)) {
			time_z_pressed += delta_time;
			if (time_z_pressed >= time_limit) {
				time_z_pressed = 0;
				text += (shift_is_down) ? "Z" : "z";
			}
			if (RELEASED(BUTTON_Z)) {
				time_z_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_0)) {
			time_0_pressed += delta_time;
			if (time_0_pressed >= time_limit) {
				time_0_pressed = 0;
				text += (shift_is_down) ? ")" : "0";
			}
			if (RELEASED(BUTTON_0)) {
				time_0_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_1)) {
			time_1_pressed += delta_time;
			if (time_1_pressed >= time_limit) {
				time_1_pressed = 0;
				text += (shift_is_down) ? "!" : "1";
			}
			if (RELEASED(BUTTON_1)) {
				time_1_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_2)) {
			time_2_pressed += delta_time;
			if (time_2_pressed >= time_limit) {
				time_2_pressed = 0;
				text += (shift_is_down) ? "" : "2";
			}
			if (RELEASED(BUTTON_2)) {
				time_2_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_3)) {
			time_3_pressed += delta_time;
			if (time_3_pressed >= time_limit) {
				time_3_pressed = 0;
				text += (shift_is_down) ? "" : "3";
			}
			if (RELEASED(BUTTON_3)) {
				time_3_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_4)) {
			time_4_pressed += delta_time;
			if (time_4_pressed >= time_limit) {
				time_4_pressed = 0;
				text += (shift_is_down) ? "" : "4";
			}
			if (RELEASED(BUTTON_4)) {
				time_4_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_5)) {
			time_5_pressed += delta_time;
			if (time_5_pressed >= time_limit) {
				time_5_pressed = 0;
				text += (shift_is_down) ? "" : "5";
			}
			if (RELEASED(BUTTON_5)) {
				time_5_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_6)) {
			time_6_pressed += delta_time;
			if (time_6_pressed >= time_limit) {
				time_6_pressed = 0;
				text += (shift_is_down) ? "" : "6";
			}
			if (RELEASED(BUTTON_6)) {
				time_a_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_7)) {
			time_7_pressed += delta_time;
			if (time_7_pressed >= time_limit) {
				time_7_pressed = 0;
				text += (shift_is_down) ? "" : "7";
			}
			if (RELEASED(BUTTON_7)) {
				time_7_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_8)) {
			time_8_pressed += delta_time;
			if (time_8_pressed >= time_limit) {
				time_8_pressed = 0;
				text += (shift_is_down) ? "" : "8";
			}
			if (RELEASED(BUTTON_8)) {
				time_8_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_9)) {
			time_9_pressed += delta_time;
			if (time_9_pressed >= time_limit) {
				time_9_pressed = 0;
				text += (shift_is_down) ? "(" : "9";
			}
			if (RELEASED(BUTTON_9)) {
				time_9_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_DOT)) {
			time_dot_pressed += delta_time;
			if (time_dot_pressed >= time_limit) {
				time_dot_pressed = 0;
				text += (shift_is_down) ? "" : ".";
			}
			if (RELEASED(BUTTON_DOT)) {
				time_dot_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_COMMA)) {
			time_comma_pressed += delta_time;
			if (time_comma_pressed >= time_limit) {
				time_comma_pressed = 0;
				text += (shift_is_down) ? "" : ",";
			}
			if (RELEASED(BUTTON_COMMA)) {
				time_comma_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_COLON)) {
			time_colon_pressed += delta_time;
			if (time_colon_pressed >= time_limit) {
				time_colon_pressed = 0;
				text += (shift_is_down) ? ":" : "";
			}
			if (RELEASED(BUTTON_COLON)) {
				time_colon_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_SPACE)) {
			time_space_pressed += delta_time;
			if (time_space_pressed >= time_limit) {
				time_space_pressed = 0;
				text += (shift_is_down) ? " " : " ";
			}
			if (RELEASED(BUTTON_SPACE)) {
				time_a_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_BACKSPACE)) {
			time_backspace_pressed += delta_time;
			if (time_backspace_pressed*3 >= time_limit) {
				time_backspace_pressed = 0;
				if (!text.empty()) {
					text.pop_back();
				}
			}
			if (RELEASED(BUTTON_BACKSPACE)) {
				time_backspace_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_PLUS)) {
			time_plus_pressed += delta_time;
			if (time_plus_pressed >= time_limit) {
				time_plus_pressed = 0;
				text += (shift_is_down) ? "+" : "";
			}
			if (RELEASED(BUTTON_SPACE)) {
				time_plus_pressed = 0;
			}
		}
		if (IS_DOWN(BUTTON_MINUS)) {
			time_minus_pressed += delta_time;
			if (time_minus_pressed >= time_limit) {
				time_minus_pressed = 0;
				text += (shift_is_down) ? "_" : "-";
			}
			if (RELEASED(BUTTON_SPACE)) {
				time_minus_pressed = 0;
			}
		}
		draw();
	}
};

#endif // GAME_TYPER_HPP