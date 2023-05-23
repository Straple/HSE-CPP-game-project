#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
//
#include "../main.cpp"
//
#include "game_message.hpp"
//

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<GameMessage> GameMessageQueue;

//----------------------------------------------------------------------

class AbstractClient {
public:
    virtual ~AbstractClient() = default;

    virtual void deliver(const GameMessage &msg) = 0;

    AbstractClient(tcp::socket socket)
        : socket(std::move(socket)) {
    }

    tcp::socket socket;  // сокет клиента
};

typedef std::shared_ptr<AbstractClient> AbstractClientPtr;

//----------------------------------------------------------------------

class GameSession {
public:
    void join(const AbstractClientPtr &client_ptr) {
        Clients.insert(client_ptr);  // НИКАКИХ STD::MOVE!!!

        auto remote_endpoint = client_ptr->socket.remote_endpoint();
        std::cout << "Client <" << remote_endpoint << "> joined in session" << std::endl
                  << std::endl;
    }

    void leave(const AbstractClientPtr &client_ptr) {
        Clients.erase(client_ptr);

        auto remote_endpoint = client_ptr->socket.remote_endpoint();
        std::cout << "Client <" << remote_endpoint << "> leaved from session" << std::endl
                  << std::endl;
    }

    void deliver(const GameMessage &msg) {
        for (const auto &client : Clients) {
            client->deliver(msg);
        }
    }

    std::set<AbstractClientPtr> Clients;
};

//----------------------------------------------------------------------

class ClientHandler : public AbstractClient, public std::enable_shared_from_this<ClientHandler> {
public:
    ClientHandler(tcp::socket socket, GameSession &session)
        : AbstractClient(std::move(socket)), session(session) {
    }

    void start() {
        session.join(shared_from_this());
        do_read_header();
    }

    void deliver(const GameMessage &msg) override {
        write_msg = msg;
        do_write();
    }

    void simulate(efloat delta_time) {
        simulate_player(input, delta_time);
    }

private:
    void do_read_header() {
        auto self(shared_from_this());
        boost::asio::async_read(socket, boost::asio::buffer(read_msg.data(), GameMessage::header_length), [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec && read_msg.decode_header()) {
                do_read_body();
            } else {
                session.leave(shared_from_this());
            }
        });
    }

    void do_read_body() {
        auto self(shared_from_this());
        boost::asio::async_read(socket, boost::asio::buffer(read_msg.body(), read_msg.body_length()), [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                // room_.deliver(read_msg_); // доставить сообщение всем остальным
                do_read_header();
            } else {
                session.leave(shared_from_this());
            }
        });
    }

    void do_write() {
        auto self(shared_from_this());
        boost::asio::async_write(socket, boost::asio::buffer(write_msg.body(), write_msg.length()), [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                // ok
            } else {
                session.leave(shared_from_this());
            }
        });
    }

    // TODO: возможно write_msg перемещается при = из-за чего мы неправильное отправляем. нужно тестить

    GameMessage read_msg;  // сообщение, которое мы читаем

    GameMessage write_msg;  // сообщение, которое нужно отправить

    Input input;  // инпут игрока

    GameSession &session;  // ссылка на игровую сессию, в которой мы находимся
};

//----------------------------------------------------------------------

class Server {
public:
    Server(boost::asio::io_context &io_context, const tcp::endpoint &endpoint)
        : acceptor(io_context, endpoint),
          timer(io_context, boost::posix_time::seconds(5)),
          time_tick_prev_frame(get_ticks()) {
        do_accept();
        timer.async_wait(boost::bind(&Server::simulate_game_frame, this));
    }

    void simulate_game_frame() {
        efloat delta_time;
        // update time
        {
            u64 cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev_frame) / performance_frequency;
            time_tick_prev_frame = cur_time_tick;
        }

        for (auto &client : session.Clients) {
            dynamic_cast<ClientHandler *>(client.get())->simulate(delta_time);
        }
        simulate_game(delta_time);

        GameMessage game_state;
        // TODO: записать состояние игры в game_state
        for (auto &client : session.Clients) {
            dynamic_cast<ClientHandler *>(client.get())->simulate(delta_time);
        }

        // продлеваем таймер следующей симуляции
        timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(5));
        timer.async_wait(boost::bind(&Server::simulate_game_frame, this));
    }

private:
    void do_accept() {
        // асинхронно принимаю соединение от клиента
        acceptor.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                {
                    auto remote_endpoint = socket.remote_endpoint();
                    auto local_endpoint = socket.local_endpoint();

                    std::cout << "Connected " << remote_endpoint << " --> " << local_endpoint << std::endl;
                    std::cout << "message: " << ec.message() << std::endl;
                }
                // если нет ошибок сокета
                if (!ec) {
                    // создать клиенту его обработчика и добавить в игровую сессию
                    std::make_shared<ClientHandler>(std::move(socket), session)->start();
                }
                do_accept();  // для дальнейших принятий соединений
            }
        );
    }

    tcp::acceptor acceptor;  // принимальщик соединений

    GameSession session;

    boost::asio::deadline_timer timer;  // таймер для симуляций кадров

    u64 time_tick_prev_frame;  // время последнего кадра
};

//----------------------------------------------------------------------

int main() {
    setlocale(LC_ALL, "ru-RU");

    try {
        boost::asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), std::atoi("4"));
        Server server(io_context, endpoint);
        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}