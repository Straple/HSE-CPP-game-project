#define BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG
// #define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
//----------------------------------------------------------------------
#include "../main_header.hpp"
//----------------------------------------------------------------------
#include "game_message.hpp"
//----------------------------------------------------------------------
#include <cstdlib>
#include <iostream>
#include <memory>
#include <set>
#include <utility>

//----------------------------------------------------------------------

// получить игровое состояние
std::string get_game_state() {
    std::ostringstream oss(std::ios::binary);

    serialization_traits<std::vector<Player>>::serialize(oss, game_variables::Players);

    // mobs
    serialization_traits<std::vector<Slime>>::serialize(oss, game_variables::Slimes);
    serialization_traits<std::vector<Bat>>::serialize(oss, game_variables::Bats);
    serialization_traits<std::vector<Bomber>>::serialize(oss, game_variables::Bombers);

    // world
    serialization_traits<std::vector<Tree>>::serialize(oss, game_variables::Trees);
    serialization_traits<std::vector<Bush>>::serialize(oss, game_variables::Bushes);
    serialization_traits<std::vector<Log>>::serialize(oss, game_variables::Logs);

    // objects
    serialization_traits<std::vector<Effect>>::serialize(oss, game_variables::Effects);
    serialization_traits<std::vector<Bullet>>::serialize(oss, game_variables::Bullets);
    serialization_traits<std::vector<Coin>>::serialize(oss, game_variables::Loot_coins);
    serialization_traits<std::vector<Heart>>::serialize(oss, game_variables::Loot_hearts);

    return oss.str();
}

GameMessage build_client_id_message(int client_id) {
    GameMessage message;
    message.set_message_type(GameMessage::CLIENT_ID);
    message.set_body_length(sizeof(int));
    message.encode_header();
    std::memcpy(message.body(), &client_id, sizeof(int));
    return message;
}

GameMessage build_game_state_message(int game_frame_id) {
    std::string game_state_str = get_game_state();
    GameMessage message;
    message.set_message_type(GameMessage::GAME_STATE);
    message.set_body_length(sizeof(int) + game_state_str.size());
    message.encode_header();
    std::memcpy(message.body(), &game_frame_id, sizeof(int));
    std::memcpy(message.body() + sizeof(int), game_state_str.data(), game_state_str.size());
    return message;
}

using boost::asio::ip::tcp;

// using boost::asio::ip::udp;

//----------------------------------------------------------------------

// Нужен для гениальной (или не очень) связки GameSession с ClientHandler
class AbstractClient {
public:
    virtual ~AbstractClient() = default;

    AbstractClient(tcp::socket socket, const int client_id) : socket(std::move(socket)), client_id(client_id) {
    }

    void simulate(efloat delta_time) {
        simulate_player(delta_time, client_id);
    }

    virtual void deliver(const GameMessage &message) = 0;

    //---------------------------------------------------------------------

    tcp::socket socket;  // сокет клиента

    const int client_id;  // уникальный id клиента

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
            uint64_t cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev_frame) / get_performance_frequency();
            time_tick_prev_frame = cur_time_tick;
        }

        // симулируем игроков
        for (const auto &client_ptr : Clients) {
            client_ptr->simulate(delta_time);
        }

        // симулируем игру
        simulate_game(delta_time);

        game_state_snapshot = build_game_state_message(frame_id);
        // отправляем новый игровой кадр клиентам
        for (const auto &client_ptr : Clients) {
            client_ptr->deliver(game_state_snapshot);
        }

        // продлеваем таймер следующей симуляции
        timer_for_simulate_game.expires_at(timer_for_simulate_game.expires_at() + boost::posix_time::milliseconds(10));
        timer_for_simulate_game.async_wait(boost::bind(&GameSession::simulate_game_frame, this));
    }

    //----------------------------------------------------------------------

    // добавляет клиента в игровую сессию
    void join(const AbstractClientPtr &client_ptr) {
        Clients.insert(client_ptr);  // НИКАКИХ STD::MOVE!!!

        auto remote_endpoint = client_ptr->socket.remote_endpoint();
        std::cout << "Client <" << remote_endpoint << "> joined in session\n" << std::endl;

        // добавим персонажа в игру
        game_variables::Players.emplace_back(Dot(25, -100));
        game_variables::Players.back().client_id = client_ptr->client_id;

        // обновим игровой снапшот, так как мы получили нового игрока
        // иначе будет UB: после join() ClientHandler отправит get_game_state() клиенту,
        // а потом мы не обновив game_state_snapshot отправим его уже без этого игрокав
        frame_id++;
        game_state_snapshot = build_game_state_message(frame_id);
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
            game_variables::Players.erase(game_variables::Players.begin() + index);

            // TODO: возможно тут стоит тоже сразу же пересчитать игровой мир
            // так как например, если слайм ел игрока, а он вышел из игры, то кого он в итоге ест?
            // но да ладно, мы скоро его все равно пересчитаем
        }
    }

    //----------------------------------------------------------------------

    GameMessage game_state_snapshot;  // сохраненное глобальное состояние игры

    int frame_id = 1;  // id игрового кадра

private:
    //----------------------------------------------------------------------
    std::set<AbstractClientPtr> Clients;

    boost::asio::deadline_timer timer_for_simulate_game;

    boost::asio::deadline_timer timer_for_print_debug_info;

    //----------------------------------------------------------------------

    int frame_count_accum = 0;  // накопленное количество кадров

    uint64_t time_tick_prev_frame;  // время последнего кадра
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

        // нам нужно передать клиенту его client_id
        deliver(build_client_id_message(client_id));
    }

    // отправляет клиенту сообщение
    void deliver(const GameMessage &message) override {
        write_message = message;
        do_write();
    }

private:
    void do_read_header() {
        auto self(shared_from_this());
        boost::asio::async_read(
            socket, boost::asio::buffer(read_message.data(), GameMessage::header_length),
            [this, self](boost::system::error_code error_code, std::size_t) {
                if (!error_code) {
                    read_message.decode_header();
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
                    set_player_input();  // обработаем полученное сообщение
                    do_read_header();    // продолжим цепочку чтения сообщений
                } else {
                    std::cout << "reading body failed. message: \"" << error_code.message() << "\"" << std::endl;
                    session.leave(shared_from_this());
                }
            }
        );
    }

    // читает input игрока из read_message и устанавливает его
    void set_player_input() {
        ASSERT(
            read_message.message_type() == GameMessage::PLAYER_INPUT &&
                read_message.body_length() == sizeof(ButtonsState) + sizeof(Dot) + 2 * sizeof(int),
            "is not input from client"
        );

        int index = find_player_index(client_id);
        auto &player = game_variables::Players[index];

        const char *read_ptr = read_message.body();

        std::memcpy(&player.input.current, read_ptr, sizeof(ButtonsState));
        read_ptr += sizeof(ButtonsState);

        std::memcpy(&player.cursor_dir, read_ptr, sizeof(Dot));
        read_ptr += sizeof(Dot);

        std::memcpy(&player.cloack_color_id, read_ptr, sizeof(int));
        read_ptr += sizeof(int);

        std::memcpy(&player.t_shirt_color_id, read_ptr, sizeof(int));
        read_ptr += sizeof(int);
    }

    void do_write() {
        auto self(shared_from_this());
        boost::asio::async_write(
            socket, boost::asio::buffer(write_message.data(), write_message.length()),
            [this, self](boost::system::error_code error_code, std::size_t) {
                count_of_write_messages++;
                if (!error_code) {
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

                std::cout << "Connected " << remote_endpoint << " --> " << local_endpoint << " message: \""
                          << error_code.message() << "\"\n\n";
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
    SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));

    test_room.read("level.txt");

    try {
        boost::asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), std::atoi("5005"));
        Server server(io_context, endpoint);
        io_context.run();
    } catch (std::exception &exception) {
        std::cerr << "Exception: " << exception.what() << "\n";
    }
}
