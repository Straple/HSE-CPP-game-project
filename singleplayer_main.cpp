// boost
#include <boost/asio.hpp>
#include <boost/bind.hpp>
using boost::asio::ip::tcp;

#include "Audio/audio.hpp"
#include "game_mode.hpp"
//
#include "Multiplayer/client.cpp"

int main() {
    // setlocale(LC_ALL, "ru-RU");
    SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
#ifdef AUDIERE
    audiere::AudioDevicePtr device = audiere::OpenDevice();
#endif

    // initialize
    {
        std::cout << "performance_frequency: " << get_performance_frequency() << std::endl;

        ShowWindow(GetConsoleWindow(), global_variables::show_console ? SW_SHOW : SW_HIDE);
        ShowCursor(global_variables::show_cursor);

        read_sprites();
        read_spritesheets();
#ifdef AUDIERE
        Audio::init_audio(device);
#endif

        // создадим персонажа
        //game_variables::Players.emplace_back(Dot(25, -100));
        //game_variables::Players.back().client_id = 0;

        // test_room.read("0-lobby-level.txt");
        // test_room.read("1-forest-level.txt");
    }
    Audio::play_music(Audio::MT_gameplay);
    WindowHandler window_handler;

    uint64_t time_tick_global_start = get_ticks();
    uint64_t time_tick_prev = time_tick_global_start;

    efloat delta_time = 0;

    Player customization_player;

    Typer typer;

    while (global_variables::running) {
        auto &player = game_variables::Players[0];

        audio_variables::SoundsQueue.clear();  // почистим очередь звуков

        simulate_game_mode(delta_time, player, customization_player, window_handler, typer);

        // проиграем очередь звуков
        for (auto type : audio_variables::SoundsQueue) {
            audio_variables::Sounds[type].play();
        }

        draw_game_mode(delta_time, 0, customization_player, window_handler, typer);

        if (game_mode == GM_MULTIPLAYER) {
            game_mode = GM_GAME;

            test_room.read("0-lobby-level.txt");
            // test_room.read("1-forest-level.txt");

            std::cout << "connecting to server" << std::endl;
            run_client(window_handler, server_address);

            game_mode = GM_MAIN_MENU;
        }

        if (test_room.wave_number == 2) {
            std::cout << "New level!" << std::endl;
            // нужно перенестись в новую комнату
            if (test_room.room_name == "0-lobby-level") {
                test_room.read("1-forest-level.txt");
            } else if (test_room.room_name == "1-forest-level") {
                test_room.read("2-dungeon-level.txt");
            } else {
            }
        }

        // update time
        {
            uint64_t cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev) / get_performance_frequency();
            time_tick_prev = cur_time_tick;
        }
    }
}