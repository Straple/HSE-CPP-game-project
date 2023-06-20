// boost
#include <boost/asio.hpp>
#include <boost/bind.hpp>
using boost::asio::ip::tcp;
//
#include "../Audio/audio.hpp"
#include "../game_mode.hpp"
//
#include <iostream>
#include "game_message.hpp"

//----------------------------------------------------------------------

void set_game_state(const std::string &game_state) {
    std::istringstream iss(game_state);

    game_variables::Players = serialization_traits<std::vector<Player>>::deserialize(iss);

    // mobs
    game_variables::Slimes = serialization_traits<std::vector<Slime>>::deserialize(iss);
    game_variables::Bats = serialization_traits<std::vector<Bat>>::deserialize(iss);
    game_variables::Bombers = serialization_traits<std::vector<Bomber>>::deserialize(iss);

    // world
    game_variables::Trees = serialization_traits<std::vector<Tree>>::deserialize(iss);
    game_variables::Bushes = serialization_traits<std::vector<Bush>>::deserialize(iss);
    game_variables::Logs = serialization_traits<std::vector<Log>>::deserialize(iss);
    game_variables::Pillars = serialization_traits<std::vector<Pillar>>::deserialize(iss);
    game_variables::NunStatues = serialization_traits<std::vector<NunStatue>>::deserialize(iss);
    game_variables::Knights = serialization_traits<std::vector<Knight>>::deserialize(iss);
    game_variables::Barrels = serialization_traits<std::vector<Barrel>>::deserialize(iss);

    // loot
    game_variables::Loot_coins = serialization_traits<std::vector<Coin>>::deserialize(iss);
    game_variables::Loot_hearts = serialization_traits<std::vector<Heart>>::deserialize(iss);

    game_variables::Effects = serialization_traits<std::vector<Effect>>::deserialize(iss);

    game_variables::Bullets = serialization_traits<std::vector<Bullet>>::deserialize(iss);
    game_variables::Weapons = serialization_traits<std::vector<Weapon>>::deserialize(iss);
}

//----------------------------------------------------------------------

Player customization_player;

//----------------------------------------------------------------------

// Клиент. Это мы
class Client {
public:
    Client(
        boost::asio::io_context &io_context,
        const tcp::resolver::results_type &endpoints,
        WindowHandler &window_handler
    )
        : io_context(io_context),
          socket(io_context),
          timer(io_context, boost::posix_time::milliseconds(1)),
          time_tick_prev_frame(get_ticks()),
          window_handler(window_handler) {
        do_connect(endpoints);
    }

    void write(const GameMessage &message) {
        write_message = message;
        do_write();
    }

    void close() {
        std::cout << "Stopping..." << std::endl;
        global_variables::running = false;
        io_context.stop();
        socket.close();
    }

    void send_chat_message_to_server(const std::string &chat_message) {
        GameMessage message;
        message.set_body_length(chat_message.size());
        message.set_message_type(GameMessage::CHAT_MESSAGE);
        message.encode_header();

        std::memcpy(message.body(), chat_message.data(), chat_message.size());

        // отправим на сервер
        write(message);
    }

    void send_input_to_server() {
        if (now_is_typing) {
            return;
        }
        GameMessage message;
        message.set_body_length(sizeof(ButtonsState) + sizeof(Dot) + 2 * sizeof(int));
        message.set_message_type(GameMessage::PLAYER_INPUT);
        message.encode_header();

        // запишем ButtonsState
        if (game_mode == GM_GAME) {
            std::memcpy(message.body(), &window_handler.input.current, sizeof(ButtonsState));

        } else if (game_mode == GM_CUSTOMIZATION) {
            // никаких нажатий нет
        } else {
            ASSERT(false, "game_mode = ?");
        }

        // запишем cursor_dir (нужен для пушки и выстрелов от игрока)
        int index = find_player_index(client_id);
        std::memcpy(message.body() + sizeof(ButtonsState), &game_variables::Players[index].cursor_dir, sizeof(Dot));

        // запишем cloack_color_id
        std::memcpy(
            message.body() + sizeof(ButtonsState) + sizeof(Dot), &customization_player.cloack_color_id, sizeof(int)
        );
        // запишем t_shirt_color_id
        std::memcpy(
            message.body() + sizeof(ButtonsState) + sizeof(Dot) + sizeof(int), &customization_player.t_shirt_color_id,
            sizeof(int)
        );

        // отправим на сервер
        write(message);
    }

    void simulate_game_frame() {
        efloat delta_time;
        // update time
        {
            uint64_t cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev_frame) / get_performance_frequency();
            time_tick_prev_frame = cur_time_tick;
        }

        // update debug info
        {
            if (this_frame_from_server) {
                this_frame_from_server = false;
            } else {
                count_of_non_server_frames_accum++;
            }
            accum_time_measurement += delta_time;
            if (accum_time_measurement >= 1) {
                accum_time_measurement = 0;
                count_of_non_server_frames_snapshot = std::exchange(count_of_non_server_frames_accum, 0);
            }
        }

        int index = find_player_index(client_id);
        ASSERT(index != -1, "where this player?");
        auto &player = game_variables::Players[index];
        const auto &input = window_handler.input;

        // костыль
        bool old_is_typing = now_is_typing;
        std::string typer_text = typer.text;

        simulate_game_mode(delta_time, player, customization_player, window_handler, typer);

        // мы сбросили сообщение чата и оно не было пусто
        if (old_is_typing != now_is_typing && old_is_typing && !typer_text.empty()) {
            send_chat_message_to_server(typer_text);
        }

        audio_variables::SoundsQueue.clear();

        draw_game_mode(delta_time, client_id, customization_player, window_handler, typer);

        send_input_to_server();

        if (!global_variables::running) {
            close();
        }

        // продлеваем таймер следующей симуляции
        timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(1));
        timer.async_wait(boost::bind(&Client::simulate_game_frame, this));
    }

private:
    void do_connect(const tcp::resolver::results_type &endpoints) {
        boost::asio::async_connect(socket, endpoints, [this](boost::system::error_code error_code, tcp::endpoint) {
            if (!error_code) {
                std::cout << "connecting successful. message: \"" << error_code.message() << "\"" << std::endl;
                do_read_header();  // мы должны получить наш client_id и игровой снапшот от сервера
            } else {
                std::cout << "connecting failed. message: \"" << error_code.message() << "\"" << std::endl;
                close();
            }
        });
    }

    //----------------------------------------------------------------

    void do_read_header() {
        boost::asio::async_read(
            socket, boost::asio::buffer(read_message.data(), GameMessage::header_length),
            [this](boost::system::error_code error_code, std::size_t) {
                if (!error_code) {
                    read_message.decode_header();
                    // считали заголовок с размером тела
                    // теперь можем считать и тело
                    do_read_body();
                } else {
                    std::cout << "reading header failed. message: \"" << error_code.message() << "\"" << std::endl;
                    close();
                }
            }
        );
    }

    void do_read_body() {
        boost::asio::async_read(
            socket, boost::asio::buffer(read_message.body(), read_message.body_length()),
            [this](boost::system::error_code error_code, std::size_t) {
                if (!error_code) {
                    process_message_from_server();
                    do_read_header();  // продолжим читать у сервера
                } else {
                    std::cout << "reading body failed. message: \"" << error_code.message() << "\"" << std::endl;
                    close();
                }
            }
        );
    }

    void process_message_from_server() {
        if (read_message.message_type() == GameMessage::CLIENT_ID) {
            // считали client_id
            ASSERT(client_id == -1, "why client_id already init?");
            std::memcpy(&client_id, read_message.body(), sizeof(int));
            std::cout << "client_id: " << client_id << std::endl;

        } else if (read_message.message_type() == GameMessage::GAME_STATE) {
            this_frame_from_server = true;

            int game_state_frame_id = -1;
            std::memcpy(&game_state_frame_id, read_message.body(), sizeof(int));
            if (frame_id < game_state_frame_id) {
                // мы получили более новое игровое состояние
                bool need_state_simulates_game_frame = frame_id == 0;
                frame_id = game_state_frame_id;

                std::string game_state;
                game_state.resize(read_message.body_length() - sizeof(int));
                std::memcpy(game_state.data(), read_message.body() + sizeof(int), game_state.size());

                set_game_state(game_state);

                if (need_state_simulates_game_frame) {
                    std::cout << "Starting first game frame..." << std::endl;
                    // мы еще ни разу не играли, так как сервер не отправил нам первое игровое состояние
                    simulate_game_frame();
                }
            } else {
                std::cout << "old frame :_(" << std::endl;
            }
        } else if (read_message.message_type() == GameMessage::CHAT_MESSAGE) {
            int sender_client_id = -1;
            std::memcpy(&sender_client_id, read_message.body(), sizeof(int));

            std::string chat_message;
            chat_message.resize(read_message.body_length() - sizeof(int));
            std::memcpy(chat_message.data(), read_message.body() + sizeof(int), chat_message.size());
            ChatMessages.insert(ChatMessages.begin(), {sender_client_id, chat_message, 0});

        } else if (read_message.message_type() == GameMessage::SOUNDS) {
            int size = read_message.body_length();
            for (int i = 0; i < size; i++) {
                Audio::sound_type type =
                    *reinterpret_cast<Audio::sound_type *>(read_message.body() + i * sizeof(uint8_t));
                audio_variables::Sounds[type].play();
            }
        } else if(read_message.message_type() == GameMessage::NEW_LEVEL){

            // нужно перенестись в новую комнату
            if (test_room.room_name == "0-lobby-level") {
                test_room.read("1-forest-level.txt");
            } else if (test_room.room_name == "1-forest-level") {
                test_room.read("2-dungeon-level.txt");
            } else {
            }
        }
    }

    //----------------------------------------------------------------

    void do_write() {
        boost::asio::async_write(
            socket, boost::asio::buffer(write_message.data(), write_message.length()),
            [this](boost::system::error_code error_code, std::size_t) {
                if (!error_code) {
                } else {
                    close();
                }
            }
        );
    }

    //----------------------------------------------------------------

    boost::asio::io_context &io_context;

    tcp::socket socket;  // сокет клиента

    GameMessage read_message;  // нужно получить это сообщение

    GameMessage write_message;  // нужно отправить это сообщение

    int client_id = -1;  // наш уникальный клиент id

    int frame_id = 0;  // id серверного игрового кадра

    //----------------------------------------------------------------

    WindowHandler &window_handler;  // обработчик окна

    Typer typer;

    boost::asio::deadline_timer timer;  // таймер для симуляции кадров

    uint64_t time_tick_prev_frame;  // время последнего кадра

    //----------------------------------------------------------------

    // для дебага и статистики

    double accum_time_measurement = 0;

    // количество кадров, которые мы просимулировали без участия сервера
    // (аккумулируем это значение для приятного вывода)
    int count_of_non_server_frames_accum = 0;

    int count_of_non_server_frames_snapshot = 0;

    // этот кадр от сервера? если да, то текущее состояние игры мы получили от сервера
    bool this_frame_from_server = false;
};

//----------------------------------------------------------------------

int main() {
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
#ifdef AUDIERE
        Audio::init_audio(device);
#endif
        read_spritesheets();

        test_room.read("0-lobby-level.txt");
    }

    WindowHandler window_handler;

    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        /*"194.87.237.93" мой сервер*/
        auto endpoints = resolver.resolve("127.0.0.1", "5005");
        Client client(io_context, endpoints, window_handler);
        io_context.run();
    } catch (std::exception &exception) {
        std::cerr << "Exception: " << exception.what() << "\n";
    }
}
