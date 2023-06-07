#ifndef GAME_TYPER_HPP
#define GAME_TYPER_HPP
#include <string>
#include "../../objects.hpp"
#include "../../render.hpp"
#include "../../sprites.hpp"
#include "game_objects.hpp"

class Typer {
    std::string text = "";
    /*bool shift_is_down = false;
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
    efloat time_limit = 0.25;*/

public:
    const std::string &get_text() const {
        return text;
    }

    void clear(){
        text.clear();
    }

    void simulate(const Input &input, efloat delta_time) {
        bool shift_is_down = IS_DOWN(BUTTON_SHIFT);

#define BUTTON_HANDLER(button, with_shift, without_shift)     \
    if (PRESSED(button)) {                                    \
        text += (shift_is_down) ? with_shift : without_shift; \
    }

        BUTTON_HANDLER(BUTTON_A, 'A', 'a');
        BUTTON_HANDLER(BUTTON_B, 'B', 'b');
        BUTTON_HANDLER(BUTTON_C, 'C', 'c');
        BUTTON_HANDLER(BUTTON_D, 'D', 'd');
        BUTTON_HANDLER(BUTTON_E, 'E', 'e');
        BUTTON_HANDLER(BUTTON_F, 'F', 'f');
        BUTTON_HANDLER(BUTTON_G, 'G', 'g');
        BUTTON_HANDLER(BUTTON_H, 'H', 'h');
        BUTTON_HANDLER(BUTTON_I, 'I', 'i');
        BUTTON_HANDLER(BUTTON_J, 'J', 'j');
        BUTTON_HANDLER(BUTTON_K, 'K', 'k');
        BUTTON_HANDLER(BUTTON_L, 'L', 'l');
        BUTTON_HANDLER(BUTTON_M, 'M', 'm');
        BUTTON_HANDLER(BUTTON_N, 'N', 'n');
        BUTTON_HANDLER(BUTTON_O, 'O', 'o');
        BUTTON_HANDLER(BUTTON_P, 'P', 'p');
        BUTTON_HANDLER(BUTTON_Q, 'Q', 'q');
        BUTTON_HANDLER(BUTTON_R, 'R', 'r');
        BUTTON_HANDLER(BUTTON_S, 'S', 's');
        BUTTON_HANDLER(BUTTON_T, 'T', 't');
        BUTTON_HANDLER(BUTTON_U, 'U', 'u');
        BUTTON_HANDLER(BUTTON_V, 'V', 'v');
        BUTTON_HANDLER(BUTTON_W, 'W', 'w');
        BUTTON_HANDLER(BUTTON_X, 'X', 'x');
        BUTTON_HANDLER(BUTTON_Y, 'Y', 'y');
        BUTTON_HANDLER(BUTTON_Z, 'Z', 'z');

        BUTTON_HANDLER(BUTTON_1, "!", "1");
        BUTTON_HANDLER(BUTTON_2, "", "2");
        BUTTON_HANDLER(BUTTON_3, "", "3");
        BUTTON_HANDLER(BUTTON_4, "", "4");
        BUTTON_HANDLER(BUTTON_5, "", "5");
        BUTTON_HANDLER(BUTTON_6, "", "6");
        BUTTON_HANDLER(BUTTON_7, "", "7");
        BUTTON_HANDLER(BUTTON_8, "", "8");
        BUTTON_HANDLER(BUTTON_9, "(", "9");
        BUTTON_HANDLER(BUTTON_0, ")", "0");

        BUTTON_HANDLER(BUTTON_NUMPAD1, "1", "!");
        BUTTON_HANDLER(BUTTON_NUMPAD2, "2", "");
        BUTTON_HANDLER(BUTTON_NUMPAD3, "3", "");
        BUTTON_HANDLER(BUTTON_NUMPAD4, "4", "");
        BUTTON_HANDLER(BUTTON_NUMPAD5, "5", "");
        BUTTON_HANDLER(BUTTON_NUMPAD6, "6", "");
        BUTTON_HANDLER(BUTTON_NUMPAD7, "7", "");
        BUTTON_HANDLER(BUTTON_NUMPAD8, "8", "");
        BUTTON_HANDLER(BUTTON_NUMPAD9, "9", "(");
        BUTTON_HANDLER(BUTTON_NUMPAD0, "0", ")");

        BUTTON_HANDLER(BUTTON_DOT, "", ".");
        BUTTON_HANDLER(BUTTON_MINUS, "_", "-");
        BUTTON_HANDLER(BUTTON_PLUS, "+", "");
        BUTTON_HANDLER(BUTTON_COMMA, ",", "");
        BUTTON_HANDLER(BUTTON_QUESTION_MARK, "?", "");
        BUTTON_HANDLER(BUTTON_SPACE, " ", " ");

        if (PRESSED(BUTTON_BACKSPACE) && !text.empty()) {
            text.pop_back();
        }

        /*if (IS_DOWN(BUTTON_A)) {
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
            if (time_backspace_pressed * 3 >= time_limit) {
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
        draw();*/
    }
};

#endif  // GAME_TYPER_HPP