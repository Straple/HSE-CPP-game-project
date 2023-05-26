#ifndef MULTIPLAYER_GAME_MESSAGE_HPP
#define MULTIPLAYER_GAME_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../assert.hpp"

class GameMessage {
public:
    enum { header_length = 4 };

    enum { max_body_length = 8 * 1024 };

    GameMessage() : m_body_length(0) {
    }

    GameMessage(const std::string &string) {
        body_length(string.size());
        encode_header();
        std::memcpy(body(), string.data(), body_length());
    }

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

    void body_length(std::size_t new_length) {
        ASSERT(new_length < max_body_length, "out of memory");
        m_body_length = new_length;
    }

    bool decode_header() {
        char header[header_length + 1] = "";
        std::strncat(header, m_data, header_length);
        [[maybe_unused]] char *ptr_end = nullptr;
        m_body_length = std::strtol(header, &ptr_end, 10);  // получили заголовок из data
        ASSERT(m_body_length < max_body_length, "out of memory");
        return true;
    }

    void encode_header() {
        char header[header_length + 1] = "";
        std::sprintf(header, "%4d", m_body_length);
        std::memcpy(m_data, header, header_length);
    }

private:
    char m_data[header_length + max_body_length]{};

    int m_body_length;
};

#endif  // MULTIPLAYER_GAME_MESSAGE_HPP