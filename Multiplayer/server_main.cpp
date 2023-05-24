#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
//-------------------------
#include "../main.cpp"
//-------------------------
#include "game_message.hpp"
//----------------------------------------------------------------------

#define BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

//----------------------------------------------------------------------

// получить все игровое состояние
GameMessage get_game_state() {
    std::ostringstream oss(std::ios::binary);
    {
        serialization_traits<std::vector<Player>>::serialize(oss, Players);
        serialization_traits<std::vector<Slime>>::serialize(oss, Slimes);
        serialization_traits<std::vector<Bat>>::serialize(oss, Bats);
        serialization_traits<std::vector<Tree>>::serialize(oss, Trees);
        serialization_traits<std::vector<Bush>>::serialize(oss, Bushes);
        serialization_traits<std::vector<Log>>::serialize(oss, Logs);
        serialization_traits<std::vector<effect>>::serialize(oss, Effects);
    }

    GameMessage result;
    {
        ASSERT(oss.str().size() < result.max_body_length, "oh no, memory limit");

        result.body_length(oss.str().size());
        std::memcpy(result.body(), oss.str().data(), result.body_length());
        result.encode_header();
    }
    return result;
}

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

// Нужен для гениальной (или не очень) связки GameSession с ClientHandler
class AbstractClient {
public:
    virtual ~AbstractClient() = default;

    virtual void deliver(const GameMessage &message) = 0;

    explicit AbstractClient(tcp::socket socket)
        : socket(std::move(socket)) {
    }

    void simulate(efloat delta_time) {
        simulate_player(input, delta_time);
        input.previous = input.current;
    }

    // проинформировать меня о готовом кадре
    virtual void inform_about_new_frame() = 0;

    tcp::socket socket;  // сокет клиента

    Input input;  // инпут игрока

    int cnt_peek_message_from_client = 0;
};

using AbstractClientPtr = std::shared_ptr<AbstractClient>;

//----------------------------------------------------------------------

// игровая сессия
// периодически симулирует игру
// при необходимости удаляет или добавляет клиентов в игровую сессию
class GameSession {
public:
    explicit GameSession(boost::asio::io_context &io_context)
        : timer(io_context, boost::posix_time::milliseconds(5)),
          time_tick_prev_frame(get_ticks()) {
        timer.async_wait(boost::bind(&GameSession::simulate_game_frame, this));
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

        // print debug info
        {
            frame_accum++;
            frame_time_accum += delta_time;
            if (frame_time_accum > 1) {
                std::cout << "count of simulation fps: " << frame_accum << '\n';
                std::cout << "frame id: " << frame_id << '\n';
                for (auto &client_ptr : Clients) {
                    std::cout << "count peek message from client: " << client_ptr->cnt_peek_message_from_client << '\n';
                    client_ptr->cnt_peek_message_from_client = 0;  // сбросим информацию
                }
                std::cout << '\n';
                std::cout.flush();
                frame_time_accum = 0;
                frame_accum = 0;
            }
        }

        // симулируем игроков
        for (const auto &client_ptr : Clients) {
            client_ptr->simulate(delta_time);
        }

        // симулируем игру
        simulate_game(delta_time);

        // берем снапшот состояния игры
        game_state_snapshot = get_game_state();

        // сообщаем о нового кадре клиентам
        for (const auto &client_ptr : Clients) {
            client_ptr->inform_about_new_frame();
        }

        // продлеваем таймер следующей симуляции
        timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(5));
        timer.async_wait(boost::bind(&GameSession::simulate_game_frame, this));
    }

    //----------------------------------------------------------------------

    void join(const AbstractClientPtr &client_ptr) {
        Clients.insert(client_ptr);                // НИКАКИХ STD::MOVE!!!

        auto remote_endpoint = client_ptr->socket.remote_endpoint();
        std::cout << "Client <" << remote_endpoint << "> joined in session\n\n";
        std::cout.flush();
    }

    void leave(const AbstractClientPtr &client_ptr) {
        // если мы его еще не удалили
        // (из-за асинхронности куча, ожидающих выполнения действий, придут сюда из ClientHandler, чтобы уйти из сессии)
        if (Clients.find(client_ptr) != Clients.end()) {
            Clients.erase(client_ptr);

            auto remote_endpoint = client_ptr->socket.remote_endpoint();
            std::cout << "Client <" << remote_endpoint << "> leaved from session\n\n";
            std::cout.flush();
        }
    }

    //----------------------------------------------------------------------

    GameMessage game_state_snapshot;  // сохраненное глобальное состояние игры

    int frame_id = 0;  // id игрового кадра

private:
    std::set<AbstractClientPtr> Clients;  // клиенты

    boost::asio::deadline_timer timer;  // таймер для симуляций кадров

    //----------------------------------------------------------------------

    int frame_accum = 0;  // накопленное количество кадров

    efloat frame_time_accum = 0;  // накопленное время кадров

    u64 time_tick_prev_frame;  // время последнего кадра
};

//----------------------------------------------------------------------

// обработчик клиента (взаимодействует с ним)
class ClientHandler : public AbstractClient, public std::enable_shared_from_this<ClientHandler> {
public:
    ClientHandler(tcp::socket socket, GameSession &session)
        : AbstractClient(std::move(socket)), session(session) {
    }

    // начинает обработку и добавляет в игровую сессию
    void start() {
        session.join(shared_from_this());
        do_read_header();
    }

    // отправляет клиенту сообщение
    void deliver(const GameMessage &message) override {
        write_message = message;
        do_write();
    }

    void inform_about_new_frame() override {
        // если мы не отправляем клиенту сообщения о состоянии игры
        if (!sending_chain_is_run) {
            sending_chain_is_run = true;
            write_message_frame_id = session.frame_id;
            deliver(session.game_state_snapshot);
        }
    }

private:
    void do_read_header() {
        auto self(shared_from_this());
        boost::asio::async_read(socket, boost::asio::buffer(read_message.data(), GameMessage::header_length), [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec && read_message.decode_header()) {
                // считали заголовок с размером
                // теперь можем считать и тело
                do_read_body();
            } else {
                session.leave(shared_from_this());
            }
        });
    }

    void do_read_body() {
        auto self(shared_from_this());
        boost::asio::async_read(socket, boost::asio::buffer(read_message.body(), read_message.body_length()), [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                // обработаем полученное сообщение
                cnt_peek_message_from_client++;
                ASSERT(read_message.body_length() == sizeof(ButtonsState), "wtf???");
                std::memcpy(&input.current, read_message.body(), sizeof(ButtonsState));

                do_read_header();  // продолжим цепочку чтения сообщений
            } else {
                session.leave(shared_from_this());
            }
        });
    }

    void do_write() {
        auto self(shared_from_this());
        boost::asio::async_write(socket, boost::asio::buffer(write_message.data(), write_message.length()), [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                // возьмем последний кадр и отправим его игроку

                // если мы очень медленны и не создали новый кадр, то сейчас отсылать нет смысла

                // оставим это дело на GameSession, чтобы она после создания нового кадра сказала нам об этом,
                // чтобы мы отправили новый кадр

                sending_chain_is_run = false;

                // готов ли новый кадр
                if (write_message_frame_id < session.frame_id) {
                    // сообщим себе об этом и продолжим цепочку сообщений
                    inform_about_new_frame();
                }
            } else {
                session.leave(shared_from_this());
            }
        });
    }

    //----------------------------------------------------------------------

    GameSession &session;  // ссылка на игровую сессию, в которой мы находимся

    GameMessage read_message;  // нужно получить это сообщение

    GameMessage write_message;  // нужно отправить это сообщение

    //----------------------------------------------------------------------

    // если true, то работает цепочка отправок состояний игры клиенту
    //
    // если false, то она не работает и это нужно исправить при первой возможности
    //
    // GameSession после создания нового игрового кадра должна сказать нам об этом,
    // чтобы мы взяли этот кадр и продолжили цепочку отправок клиенту
    bool sending_chain_is_run = false;

    // id игрового кадра, который мы отправили
    int write_message_frame_id = 0;
};

//----------------------------------------------------------------------

// сервер
// принятие новых клиентов и добавление их в игровую сессию
class Server {
public:
    Server(boost::asio::io_context &io_context, const tcp::endpoint &endpoint)
        : acceptor(io_context, endpoint), session(io_context) {
        do_accept();
    }

private:
    void do_accept() {
        // асинхронно принимаю соединение от клиента
        acceptor.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                {
                    auto remote_endpoint = socket.remote_endpoint();
                    auto local_endpoint = socket.local_endpoint();

                    std::cout << "Connected " << remote_endpoint << " --> " << local_endpoint << '\n';
                    std::cout << "message: " << ec.message() << "\n\n";
                    std::cout.flush();
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

    //----------------------------------------------------------------------

    tcp::acceptor acceptor;  // принимальщик соединений

    GameSession session;  // игровая сессия
};

//----------------------------------------------------------------------

int main() {
    setlocale(LC_ALL, "ru-RU");

    test_room.read("level.txt");

    try {
        boost::asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), std::atoi("4"));
        Server server(io_context, endpoint);
        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
