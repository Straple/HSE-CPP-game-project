#ifndef MULTIPLAYER_GAME_MESSAGE_HPP
#define MULTIPLAYER_GAME_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../assert.hpp"

class GameMessage {
public:
    enum message_t : uint8_t {
        NONE,
        GAME_STATE,    // игровое состояние (снапшот)
        CHAT_MESSAGE,  // сообщение в чате
        SOUNDS,        // звуки (выстрел, взрыв и т.п.)
        PLAYER_INPUT,  // инпут игрока
        CLIENT_ID,  // client id, чтобы клиент узнал о его id
        NEW_LEVEL,  // нужно считать новый уровень
    };

    enum { header_length = 4, max_body_length = 32 * 1024 };

    // сначала записывается 3 байта body_length
    // потом 1 байт для message_t

    GameMessage() = default;

    /*GameMessage(const std::string &string) {
        set_body_length(string.size());
        encode_header();
        std::memcpy(body(), string.data(), body_length());
    }*/

    [[nodiscard]] const char *data() const {
        return m_data;
    }

    [[nodiscard]] char *data() {
        return m_data;
    }

    [[nodiscard]] std::size_t length() const {
        return header_length + m_body_length;
    }

    [[nodiscard]] const char *body() const {
        return m_data + header_length;
    }

    [[nodiscard]] char *body() {
        return m_data + header_length;
    }

    [[nodiscard]] std::size_t body_length() const {
        return m_body_length;
    }

    void set_body_length(std::size_t new_body_length) {
        ASSERT(new_body_length < max_body_length, "out of memory");
        m_body_length = new_body_length;
    }

    void set_message_type(message_t new_message_type) {
        m_message_type = new_message_type;
    }

    message_t message_type() const {
        return m_message_type;
    }

    void decode_header() {
        m_body_length = 0;
        std::memcpy(&m_body_length, m_data, 3);
        std::memcpy(&m_message_type, m_data + 3, 1);
        ASSERT(m_body_length < max_body_length, "out of memory");
    }

    void encode_header() {
        std::memcpy(m_data, &m_body_length, 3);
        std::memcpy(m_data + 3, &m_message_type, 1);
    }

private:
    int m_body_length = 0;

    message_t m_message_type = NONE;

    char m_data[header_length + max_body_length]{};
};

#endif  // MULTIPLAYER_GAME_MESSAGE_HPP