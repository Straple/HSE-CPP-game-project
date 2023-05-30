// boost
#include <boost/asio.hpp>
#include <boost/bind.hpp>
using boost::asio::ip::tcp;
//
#include "../window_handler.hpp"
//
#include <iostream>
#include "game_message.hpp"

//----------------------------------------------------------------------

void set_game_state(const std::string &game_state) {
    std::istringstream iss(game_state);

    Players = serialization_traits<std::vector<Player>>::deserialize(iss);
    Slimes = serialization_traits<std::vector<Slime>>::deserialize(iss);
    Bats = serialization_traits<std::vector<Bat>>::deserialize(iss);
    Trees = serialization_traits<std::vector<Tree>>::deserialize(iss);
    Bushes = serialization_traits<std::vector<Bush>>::deserialize(iss);
    Logs = serialization_traits<std::vector<Log>>::deserialize(iss);
    Effects = serialization_traits<std::vector<Effect>>::deserialize(iss);
    Bullets = serialization_traits<std::vector<Bullet>>::deserialize(iss);
    Loot_coins = serialization_traits<std::vector<Coin>>::deserialize(iss);
    Loot_hearts = serialization_traits<std::vector<Heart>>::deserialize(iss);
}

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

    void send_input_to_server() {
        write_message_frame_id = frame_id;
        sending_chain_is_run = true;

        GameMessage message;
        message.body_length(sizeof(ButtonsState) + sizeof(Dot));
        message.encode_header();

        // запишем ButtonsState
        std::memcpy(message.body(), &window_handler.input.current, sizeof(ButtonsState));

        // запишем cursor_dir (нужен для пушки и выстрелов от игрока)
        int index = find_player_index(client_id);
        std::memcpy(message.body() + sizeof(ButtonsState), &Players[index].cursor_dir, sizeof(Dot));

        // отправим на сервер
        write(message);
    }

    void simulate_game_frame() {
        frame_id++;

        efloat delta_time;
        // update time
        {
            uint64_t cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev_frame) / performance_frequency;
            time_tick_prev_frame = cur_time_tick;
        }

        // simulate frame
        {
            window_handler.update();

            int index = find_player_index(client_id);
            Players[index].cursor_dir = window_handler.cursor.pos + global_variables::camera.pos - Players[index].pos;
            global_variables::camera.simulate(Players[index].pos, delta_time);
        }

        if (!this_frame_from_server) {
            // этот фрейм уже не от сервера
            // мы должны симулировать свою версию игры
            //
            // то есть мы пару кадров будем жить в своем собственном мире,
            // а когда получим от сервера его версию игры, то переключимся на нее
            // таким образом игра становиться более плавной и менее зависящей от сервера
            // если долго не будут приходить пакеты от сервера, то клиент еще сможет играть

            // симулируем игроков
            for (auto &player : Players) {
                simulate_player(delta_time, player.client_id);
            }

            // симулируем игру
            simulate_game(delta_time);
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

        // draw frame
        {
            window_handler.draw_frame(delta_time, client_id);

            if (global_variables::show_fps) {
                draw_object(
                    count_of_non_server_frames_snapshot, Dot(5, 20) - global_variables::arena_half_size, 0.5, GREEN
                );
            }

            window_handler.release_frame();
        }

        if (!sending_chain_is_run) {
            // восстановим цепочку отправок сообщений инпута на сервер
            send_input_to_server();
        }

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
                if (!error_code && read_message.decode_header()) {
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
                    this_frame_from_server = true;
                    if (client_id == -1) {
                        // это первый пакет от сервера
                        // он прислал нам наш client_id и игровой снапшот

                        // считали client_id
                        std::memcpy(&client_id, read_message.body(), sizeof(int));
                        std::cout << "client_id: " << client_id << std::endl;

                        // считали game_state
                        std::string game_state;
                        game_state.resize(read_message.body_length() - sizeof(int));
                        std::memcpy(game_state.data(), read_message.body() + sizeof(int), game_state.size());

                        set_game_state(game_state);

                        simulate_game_frame();  // теперь мы можем начать играть
                    } else {
                        // считали game_state
                        std::string game_state;
                        game_state.resize(read_message.body_length());
                        std::memcpy(game_state.data(), read_message.body(), game_state.size());

                        set_game_state(game_state);
                    }

                    do_read_header();  // продолжим читать у сервера
                } else {
                    std::cout << "reading body failed. message: \"" << error_code.message() << "\"" << std::endl;
                    close();
                }
            }
        );
    }

    //----------------------------------------------------------------

    void do_write() {
        boost::asio::async_write(
            socket, boost::asio::buffer(write_message.data(), write_message.length()),
            [this](boost::system::error_code error_code, std::size_t) {
                if (!error_code) {
                    sending_chain_is_run = false;

                    if (write_message_frame_id != frame_id) {
                        send_input_to_server();
                    } else {
                        // мы прервали цепочку отправок инпута на сервер
                        // мы должны ее восстановить, когда обработаем новый игровой кадр
                    }
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

    //----------------------------------------------------------------

    // id нашего (не серверного) игрового кадра
    // увеличивается на 1 при нашей симуляции
    int frame_id = 0;

    // id нашего (не серверного) игрового кадра сообщения, который мы отправили
    // нужен, чтобы не слать постоянно инпут серверу, когда он даже не пересчитан
    int write_message_frame_id = 0;

    // если true, то работает цепочка отправок состояний инпута на сервер
    //
    // если false, то она не работает и это нужно исправить при первой возможности
    //
    // когда мы обработаем свой новый кадр, то должны будем ее восстановить
    bool sending_chain_is_run = false;

    //----------------------------------------------------------------

    WindowHandler &window_handler;  // обработчик окна

    boost::asio::deadline_timer timer;  // таймер для симуляции кадров

    uint64_t time_tick_prev_frame;  // время последнего кадра

    //----------------------------------------------------------------

    // для дебага

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

    // initialize
    {
        std::cout << "performance_frequency: " << performance_frequency << std::endl;

        ShowWindow(GetConsoleWindow(), global_variables::show_console ? SW_SHOW : SW_HIDE);
        ShowCursor(global_variables::show_cursor);

        read_sprites();
        read_spritesheets();

        test_room.read("level.txt");
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
