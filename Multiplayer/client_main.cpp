
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include "game_message.hpp"
//
#include "../main.cpp"

//

void set_game_state(GameMessage &message) {
    std::string data;
    // std::cout << "message length: " << message.length() << std::endl;
    data.resize(message.length());
    std::memcpy(data.data(), message.body(), message.length());
    std::istringstream iss(data);

    // std::cout << "deserializing..." << std::endl;

    Players = serialization_traits<std::vector<Player>>::deserialize(iss);
    Slimes = serialization_traits<std::vector<Slime>>::deserialize(iss);
    Bats = serialization_traits<std::vector<Bat>>::deserialize(iss);
    Trees = serialization_traits<std::vector<Tree>>::deserialize(iss);
    Bushes = serialization_traits<std::vector<Bush>>::deserialize(iss);
    Logs = serialization_traits<std::vector<Log>>::deserialize(iss);
    Effects = serialization_traits<std::vector<effect>>::deserialize(iss);
    // std::cout << "OK" << std::endl;
}

using boost::asio::ip::tcp;

typedef std::deque<GameMessage> GameMessageQueue;

class Client {
public:
    Client(boost::asio::io_context &io_context, const tcp::resolver::results_type &endpoints, engine_app &eng)
        : io_context(io_context),
          socket(io_context),
          timer(io_context, boost::posix_time::milliseconds(5)),
          time_tick_prev_frame(get_ticks()),
          eng(eng) {
        do_connect(endpoints);
    }

    void write(const GameMessage &msg) {
        write_msg = msg;
        do_write();
    }

    void close() {
        socket.close();
        global_variables::running = false;
    }

    void render_game_frame() {
        efloat delta_time;
        // update time
        {
            u64 cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev_frame) / performance_frequency;
            time_tick_prev_frame = cur_time_tick;
        }

        eng.simulate_frame(delta_time);

        if (!global_variables::running) {
            std::cout << "Stopping..." << std::endl;
            io_context.stop();
            return;
        }

        // send Input to server
        {
            GameMessage input_message;
            input_message.body_length(sizeof(Input));
            std::memcpy(input_message.body(), reinterpret_cast<char *>(&eng.input), input_message.body_length());
            input_message.encode_header();
            write(input_message);
        }

        // продлеваем таймер следующего кадра
        timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(5));
        timer.async_wait(boost::bind(&Client::render_game_frame, this));
    }

private:
    void do_connect(const tcp::resolver::results_type &endpoints) {
        boost::asio::async_connect(socket, endpoints, [this](boost::system::error_code ec, tcp::endpoint) {
            if (!ec) {
                do_read_header();
                render_game_frame();
            } else {
                std::cout << "connecting failed\n";
                std::cout << "message: " << ec << std::endl;
                io_context.stop();
            }
        });
    }

    void do_read_header() {
        boost::asio::async_read(socket, boost::asio::buffer(read_msg.data(), GameMessage::header_length), [this](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec && read_msg.decode_header()) {
                do_read_body();
            } else {
                close();
            }
        });
    }

    void do_read_body() {
        boost::asio::async_read(socket, boost::asio::buffer(read_msg.body(), read_msg.body_length()), [this](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                // std::cout.write(read_msg.body(), read_msg.body_length());
                // std::cout << std::endl;

                set_game_state(read_msg);

                do_read_header();
            } else {
                close();
            }
        });
    }

    void do_write() {
        boost::asio::async_write(socket, boost::asio::buffer(write_msg.data(), write_msg.length()), [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
            } else {
                close();
            }
        });
    }

private:
    boost::asio::io_context &io_context;

    tcp::socket socket;  // сокет клиента

    GameMessage read_msg;  // нужно получить это сообщение

    GameMessage write_msg;  // нужно отправить это сообщение

    boost::asio::deadline_timer timer;  // таймер для рендера кадров

    u64 time_tick_prev_frame;  // время последнего кадра

    engine_app &eng;
};

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

    engine_app eng;
    {
        if (global_variables::fullscreen_mode) {
            eng.set_fullscreen_mode();
        } else {
            eng.set_window_mode();
        }
    }

    try {
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "4");

        Client client(io_context, endpoints, eng);

        io_context.run();  // run game

        client.close();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}