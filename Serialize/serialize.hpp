#ifndef GAME_ENGINE_SERIALIZATION
#define GAME_ENGINE_SERIALIZATION

#include <fstream>
#include <vector>
#include "container_traits.hpp"

using serialization_traits_container_size_t = uint32_t;

//----------------------------------------------------------------------

// standard serialization_traits
template <typename T, typename enable_if_type = void>
struct serialization_traits {
    static void serialize(std::ostream &output_stream, const T &value) {
        value.serialize(output_stream);
    }

    static T deserialize(std::istream &input_stream) {
        T result;
        result.deserialize(input_stream);
        return result;
    }
};

//----------------------------------------------------------------------

template <typename T>
void byte_serialization(std::ostream &output_stream, const T &value) {
    output_stream.write(reinterpret_cast<const char *>(&value), sizeof(T));
}

template <typename T>
T byte_deserialization(std::istream &input_stream) {
    T result;
    input_stream.read(reinterpret_cast<char *>(&result), sizeof(T));
    return result;
}

// macro for bytes serialization in structures
#define ADD_BYTE_SERIALIZATION()                                                                  \
    inline void serialize(std::ostream &output_stream) const {                                    \
        byte_serialization(output_stream, *this);                                                 \
    }                                                                                             \
                                                                                                  \
    inline void deserialize(std::istream &input_stream) {                                         \
        *this = byte_deserialization<std::remove_reference<decltype(*this)>::type>(input_stream); \
    }

//----------------------------------------------------------------------

// serialization_traits for arithmetic: char, int, long long, double ...
template <typename T>
struct serialization_traits<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> {
    static void serialize(std::ostream &output_stream, const T &value) {
        byte_serialization(output_stream, value);
    }

    static T deserialize(std::istream &input_stream) {
        return byte_deserialization<T>(input_stream);
    }
};

//----------------------------------------------------------------------

// serialization_traits for container
template <typename container_t>
struct serialization_traits<container_t, typename std::enable_if<is_container<container_t>::value>::type> {
    static void serialize(std::ostream &output_stream, const container_t &container) {
        byte_serialization<serialization_traits_container_size_t>(output_stream, container.size());
        for (const auto &value : container) {
            serialization_traits<typename container_t::value_type>::serialize(output_stream, value);
        }
    }

    static container_t deserialize(std::istream &input_stream) {
        serialization_traits_container_size_t size = byte_deserialization<decltype(size)>(input_stream);
        std::vector<typename container_t::value_type> temp_data(size);
        for (auto &value : temp_data) {
            value = serialization_traits<typename container_t::value_type>::deserialize(input_stream);
        }
        return container_t(temp_data.begin(), temp_data.end());
    }
};

#endif  // GAME_ENGINE_SERIALIZATION