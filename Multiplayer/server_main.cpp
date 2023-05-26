#include <cstdlib>
#include <iostream>
#include <memory>
#include <set>
#include <utility>
//----------------------------------------------------------------------
#define BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG
//#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
//----------------------------------------------------------------------
#include "../main_header.hpp"
//----------------------------------------------------------------------
#include "game_message.hpp"

//----------------------------------------------------------------------

// получить игровое состояние
std::string get_game_state() {
    std::ostringstream oss(std::ios::binary);

    serialization_traits<std::vector<Player>>::serialize(oss, Players);
    serialization_traits<std::vector<Slime>>::serialize(oss, Slimes);
    serialization_traits<std::vector<Bat>>::serialize(oss, Bats);
    serialization_traits<std::vector<Tree>>::serialize(oss, Trees);
    serialization_traits<std::vector<Bush>>::serialize(oss, Bushes);
    serialization_traits<std::vector<Log>>::serialize(oss, Logs);
    serialization_traits<std::vector<effect>>::serialize(oss, Effects);
    serialization_traits<std::vector<Bullet>>::serialize(oss, Bullets);
    return oss.str();
}

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

// Нужен для гениальной (или не очень) связки GameSession с ClientHandler
class AbstractClient {
public:
    virtual ~AbstractClient() = default;

    // проинформировать меня о готовом кадре
    virtual void inform_me_about_new_frame() = 0;

    //----------------------------------------------------------------------

    AbstractClient(tcp::socket socket, const int client_id) : socket(std::move(socket)), client_id(client_id) {
    }

    void simulate(efloat delta_time) {
        simulate_player(input, delta_time, client_id);
        // текущий кадр инпута так и оставим, а вот предыдущий смениться текущим
        // таким образом, мы будем считать, что игрок все еще нажимает на кнопку до тех пор,
        // пока не получим информацию от него об обратном
        input.previous = input.current;
    }

    //---------------------------------------------------------------------

    tcp::socket socket;  // сокет клиента

    const int client_id;  // уникальный id клиента

    //----------------------------------------------------------------------

    Input input;  // инпут игрока

    //----------------------------------------------------------------------

    // для серверной статистики и дебага

    int count_of_write_messages = 0;

    int count_of_read_messages = 0;
};

using AbstractClientPtr = std::shared_ptr<AbstractClient>;

//----------------------------------------------------------------------

// игровая сессия
// периодически симулирует игру
// при необходимости удаляет или добавляет клиентов в игровую сессию
class GameSession {
public:
    explicit GameSession(boost::asio::io_context &io_context)
        : timer_for_simulate_game(io_context, boost::posix_time::milliseconds(0)),
          timer_for_print_debug_info(io_context, boost::posix_time::milliseconds(0)),
          time_tick_prev_frame(get_ticks()) {
        print_debug_info();
        simulate_game_frame();
    }

    void print_debug_info() {
        std::cout << "server fps: " << frame_count_accum << ' ' << "frame_id: " << frame_id << '\n';
        if (!Clients.empty()) {
            std::cout << "write | reade\n";
        }
        for (auto &client_ptr : Clients) {
            std::cout << client_ptr->count_of_write_messages << ' ' << client_ptr->count_of_read_messages << '\n';
            client_ptr->count_of_write_messages = 0;
            client_ptr->count_of_read_messages = 0;
        }
        std::cout << '\n';
        std::cout.flush();

        frame_count_accum = 0;

        // продлеваем таймер
        timer_for_print_debug_info.expires_at(timer_for_print_debug_info.expires_at() + boost::posix_time::seconds(1));
        timer_for_print_debug_info.async_wait(boost::bind(&GameSession::print_debug_info, this));
    }

    void simulate_game_frame() {
        frame_id++;
        frame_count_accum++;

        efloat delta_time;
        // update time
        {
            u64 cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev_frame) / performance_frequency;
            time_tick_prev_frame = cur_time_tick;
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
            client_ptr->inform_me_about_new_frame();
        }

        // продлеваем таймер следующей симуляции
        timer_for_simulate_game.expires_at(timer_for_simulate_game.expires_at() + boost::posix_time::milliseconds(5));
        timer_for_simulate_game.async_wait(boost::bind(&GameSession::simulate_game_frame, this));
    }

    //----------------------------------------------------------------------

    // добавляет клиента в игровую сессию
    void join(const AbstractClientPtr &client_ptr) {
        Clients.insert(client_ptr);  // НИКАКИХ STD::MOVE!!!

        auto remote_endpoint = client_ptr->socket.remote_endpoint();
        std::cout << "Client <" << remote_endpoint << "> joined in session\n" << std::endl;

        // добавим персонажа в игру
        Players.push_back({});
        Players.back().client_id = client_ptr->client_id;

        // обновим игровой снапшот, так как мы получили нового игрока
        // иначе будет UB: после join() ClientHandler отправит get_game_state() клиенту,
        // а потом мы не обновив game_state_snapshot отправим его уже без этого игрока
        game_state_snapshot = get_game_state();
    }

    // удаляет клиента из игровой сессии
    void leave(const AbstractClientPtr &client_ptr) {
        // если мы его еще не удалили
        // (из-за асинхронности куча, ожидающих выполнения действий, придут сюда из ClientHandler, чтобы уйти из сессии)
        if (Clients.find(client_ptr) != Clients.end()) {
            Clients.erase(client_ptr);

            auto remote_endpoint = client_ptr->socket.remote_endpoint();
            std::cout << "Client <" << remote_endpoint << "> leaved from session\n\n";
            std::cout.flush();

            // также удалим персонажа из игры
            int index = find_player_index(client_ptr->client_id);
            Players.erase(Players.begin() + index);

            // TODO: возможно тут стоит тоже сразу же пересчитать игровой мир
            // так как например, если слайм ел игрока, а он вышел из игры, то кого он в итоге ест?
        }
    }

    //----------------------------------------------------------------------

    GameMessage game_state_snapshot;  // сохраненное глобальное состояние игры

    int frame_id = 0;  // id игрового кадра

private:
    //----------------------------------------------------------------------
    std::set<AbstractClientPtr> Clients;  // клиенты

    boost::asio::deadline_timer timer_for_simulate_game;

    boost::asio::deadline_timer timer_for_print_debug_info;

    //----------------------------------------------------------------------

    int frame_count_accum = 0;  // накопленное количество кадров

    efloat frame_time_accum = 0;  // накопленное время кадров

    u64 time_tick_prev_frame;  // время последнего кадра
};

//----------------------------------------------------------------------

// обработчик клиента (взаимодействует с ним)
class ClientHandler : public AbstractClient, public std::enable_shared_from_this<ClientHandler> {
public:
    ClientHandler(tcp::socket socket, GameSession &session, const int client_id)
        : AbstractClient(std::move(socket), client_id), session(session) {
        // тут нельзя вызывать никаких асинхронных операций,
        // потому что мы еще не построили себя => не построили shared_from_this()
        // поэтому будет error: bad_weak_ptr
    }

    // начинает обработку и добавляет в игровую сессию
    void start() {
        session.join(shared_from_this());
        do_read_header();  // начинаем читать инпут у клиента

        // нам нужно передать клиенту его client_id, а также снапшот игрового состояния
        {
            sending_chain_is_run = true;  // мы начали цепочку отправок

            std::string client_id_string;
            client_id_string.resize(sizeof(int));
            std::memcpy(client_id_string.data(), &client_id, sizeof(int));

            GameMessage message = client_id_string + get_game_state();
            deliver(message);
        }
    }

    // отправляет клиенту сообщение
    void deliver(const GameMessage &message) {
        write_message = message;
        do_write();
    }

    void inform_me_about_new_frame() override {
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
        boost::asio::async_read(
            socket, boost::asio::buffer(read_message.data(), GameMessage::header_length),
            [this, self](boost::system::error_code error_code, std::size_t) {
                if (!error_code && read_message.decode_header()) {
                    // считали заголовок с размером тела
                    // теперь можем считать и тело
                    do_read_body();
                } else {
                    std::cout << "reading header failed. message: \"" << error_code.message() << "\"" << std::endl;
                    session.leave(shared_from_this());
                }
            }
        );
    }

    void do_read_body() {
        auto self(shared_from_this());
        boost::asio::async_read(
            socket, boost::asio::buffer(read_message.body(), read_message.body_length()),
            [this, self](boost::system::error_code error_code, std::size_t) {
                count_of_read_messages++;

                if (!error_code) {
                    // обработаем полученное сообщение
                    ASSERT(read_message.body_length() == sizeof(ButtonsState) + sizeof(Dot), "wtf???");

                    std::memcpy(&input.current, read_message.body(), sizeof(ButtonsState));

                    Dot &cursor_dir = Players[find_player_index(client_id)].cursor_dir;
                    std::memcpy(&cursor_dir, read_message.body() + sizeof(ButtonsState), sizeof(Dot));

                    do_read_header();  // продолжим цепочку чтения сообщений
                } else {
                    std::cout << "reading body failed. message: \"" << error_code.message() << "\"" << std::endl;
                    session.leave(shared_from_this());
                }
            }
        );
    }

    void do_write() {
        auto self(shared_from_this());
        boost::asio::async_write(
            socket, boost::asio::buffer(write_message.data(), write_message.length()),
            [this, self](boost::system::error_code error_code, std::size_t) {
                count_of_write_messages++;

                if (!error_code) {
                    // возьмем последний кадр и отправим его игроку

                    // если мы очень медленны и не создали новый кадр, то сейчас отсылать нет смысла

                    // оставим это дело на GameSession, чтобы она после создания нового кадра сказала нам об этом,
                    // чтобы мы отправили новый кадр

                    sending_chain_is_run = false;

                    // готов ли новый кадр?
                    if (write_message_frame_id < session.frame_id) {
                        // сообщим себе об этом и продолжим цепочку сообщений
                        inform_me_about_new_frame();
                    }
                } else {
                    session.leave(shared_from_this());
                }
            }
        );
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
    // чтобы мы взяли этот кадр и начали новую цепочку отправок клиенту
    bool sending_chain_is_run = false;

    // id игрового кадра, который мы отправили
    // нужен для того, чтобы понимать, стоит ли отправлять кадр, возможно он старый
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
        acceptor.async_accept([this](boost::system::error_code error_code, tcp::socket socket) {
            {
                auto remote_endpoint = socket.remote_endpoint();
                auto local_endpoint = socket.local_endpoint();

                std::cout << "Connected " << remote_endpoint << " --> " << local_endpoint << " message: \"" << error_code.message() << "\"\n\n";
                std::cout.flush();
            }
            // если нет ошибок сокета
            if (!error_code) {
                // создать клиенту его обработчика и добавить в игровую сессию
                std::make_shared<ClientHandler>(std::move(socket), session, count_of_connects)->start();
                count_of_connects++;
            }
            do_accept();  // для дальнейших принятий соединений
        });
    }

    //----------------------------------------------------------------------

    tcp::acceptor acceptor;  // принимальщик соединений

    GameSession session;  // игровая сессия

    int count_of_connects = 0;  // количество подключений, используется для формирования уникального id клиента
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
