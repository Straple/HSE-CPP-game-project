// boost
#include <boost/asio.hpp>
#include <boost/bind.hpp>
//
#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
//
#include "../main.cpp"
//
#include "game_message.hpp"

//----------------------------------------------------------------------

void set_game_state(GameMessage &message) {
    std::string data;
    data.resize(message.length());
    std::memcpy(data.data(), message.body(), message.length());
    std::istringstream iss(data);

    {
        Players = serialization_traits<std::vector<Player>>::deserialize(iss);
        Slimes = serialization_traits<std::vector<Slime>>::deserialize(iss);
        Bats = serialization_traits<std::vector<Bat>>::deserialize(iss);
        Trees = serialization_traits<std::vector<Tree>>::deserialize(iss);
        Bushes = serialization_traits<std::vector<Bush>>::deserialize(iss);
        Logs = serialization_traits<std::vector<Log>>::deserialize(iss);
        Effects = serialization_traits<std::vector<effect>>::deserialize(iss);
        Bullets = serialization_traits<std::vector<Bullet>>::deserialize(iss);
    }
}

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

// Клиент. Это мы
class Client {
public:
    Client(boost::asio::io_context &io_context, const tcp::resolver::results_type &endpoints, WindowHandler &window_handler)
        : io_context(io_context),
          socket(io_context),
          timer(io_context, boost::posix_time::milliseconds(5)),
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
        int index = find_player_index(client_id);
        // не за чем отправлять свой инпут, когда мы не получили первое игровое состояние
        if(index != -1) {
            write_message_frame_id = frame_id;
            sending_chain_is_run = true;

            GameMessage message;
            message.body_length(sizeof(ButtonsState) + sizeof(Dot));
            message.encode_header();
            std::memcpy(message.body(), reinterpret_cast<char *>(&window_handler.input), sizeof(ButtonsState));

            Dot cursor_pos = window_handler.cursor.pos + global_variables::camera.pos - Players[index].pos;
            std::memcpy(message.body() + sizeof(ButtonsState), &cursor_pos, sizeof(Dot));
            write(message);
        }
    }

    void simulate_game_frame() {
        frame_id++;

        efloat delta_time;
        // update time
        {
            u64 cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev_frame) / performance_frequency;
            time_tick_prev_frame = cur_time_tick;
        }

        frame_time_accum += delta_time;
        if (frame_time_accum > 1) {
            frame_read_count_snapshot = frame_read_count_accum;
            frame_read_count_accum = 0;
            frame_time_accum = 0;
        }

        window_handler.simulate_frame(delta_time);

        int index = find_player_index(client_id);
        // если в игровом состоянии есть наш персонаж
        if (index != -1) {
            Players[index].cursor_dir = window_handler.cursor.pos + global_variables::camera.pos - Players[index].pos;
            global_variables::camera.simulate(Players[index].pos, delta_time);
        }

        draw_object(frame_read_count_snapshot, Dot(), 0.5, RED);

        window_handler.release_frame();

        if (!sending_chain_is_run) {
            // восстановим цепочку отправок сообщений инпута на сервер
            send_input_to_server();
        }

        if (!global_variables::running) {
            close();
        }

        // продлеваем таймер следующего кадра
        timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(5));
        timer.async_wait(boost::bind(&Client::simulate_game_frame, this));
    }

private:
    void do_connect(const tcp::resolver::results_type &endpoints) {
        boost::asio::async_connect(socket, endpoints, [this](boost::system::error_code ec, tcp::endpoint) {
            if (!ec) {
                do_read_header();  // мы должны получить наш client_id от сервера
            } else {
                std::cout << "connecting failed\n";
                std::cout << "message: " << ec << std::endl;
                io_context.stop();
            }
        });
    }

    //----------------------------------------------------------------

    void do_read_header() {
        boost::asio::async_read(socket, boost::asio::buffer(read_message.data(), GameMessage::header_length), [this](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec && read_message.decode_header()) {
                do_read_body();
            } else {
                close();
            }
        });
    }

    void do_read_body() {
        boost::asio::async_read(socket, boost::asio::buffer(read_message.body(), read_message.body_length()), [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                if (client_id == -1) {
                    // сервер прислал нам client_id
                    ASSERT(read_message.body_length() == sizeof(int), "is not client_id?");
                    std::memcpy(&client_id, read_message.body(), sizeof(int));

                    // теперь мы можем начать играть
                    simulate_game_frame();
                } else {
                    frame_read_count_accum++;
                    set_game_state(read_message);
                }

                do_read_header();  // продолжим читать у сервера
            } else {
                close();
            }
        });
    }

    //----------------------------------------------------------------

    void do_write() {
        boost::asio::async_write(socket, boost::asio::buffer(write_message.data(), write_message.length()), [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                sending_chain_is_run = false;

                if (write_message_frame_id != frame_id) {
                    send_input_to_server();
                } else {
                    // мы прервали цепочку отправок инпута на сервер
                    // мы должны ее восстановить, когда обработаем новый игровой кадр
                }
                // TODO: это возможно нужно удалить
                // write_message_frame_id = frame_id;
            } else {
                close();
            }
        });
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

    boost::asio::deadline_timer timer;  // таймер для рендера кадров

    //----------------------------------------------------------------

    u64 time_tick_prev_frame;  // время последнего кадра

    efloat frame_time_accum = 0;  // накопленное время кадров

    int frame_read_count_accum = 0;

    int frame_read_count_snapshot = 0;
};

//----------------------------------------------------------------------

int main() {
    // initialize
    {
        std::cout << "performance_frequency: " << performance_frequency << std::endl;

        // ShowWindow(GetConsoleWindow(), global_variables::show_console ? SW_SHOW : SW_HIDE);
        // ShowCursor(global_variables::show_cursor);

        read_sprites();
        read_spritesheets();

        test_room.read("level.txt");
    }

    WindowHandler window_handler;

    try {
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "4");

        Client client(io_context, endpoints, window_handler);

        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
