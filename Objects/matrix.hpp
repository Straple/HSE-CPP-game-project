#ifndef GAME_ENGINE_MATRIX_HPP
#define GAME_ENGINE_MATRIX_HPP

#include "../utils.hpp"

/*
 *     width
 *  h +-----+
 *  e |#####|
 *  i |#####|
 *  g |#####|
 *  h |#####|
 *  t +-----+
 */
template <typename T>
class Matrix {
    unsigned int m_height = 0;  // колво строк
    unsigned int m_width = 0;   // колво столбцов
    T *m_memory = nullptr;

    // обнуление
    void zeroing() {
        m_height = m_width = 0;
        m_memory = nullptr;
    }

    void copy_matrix(const Matrix &copy_object) {
        m_height = copy_object.m_height;
        m_width = copy_object.m_width;
        unsigned int n = m_height * m_width;
        m_memory = new T[n];
        for (unsigned int i = 0; i < n; i++) {
            m_memory[i] = copy_object.m_memory[i];
        }
    }

    void move_matrix(Matrix &move_object) {
        m_memory = move_object.m_memory;
        m_height = move_object.m_height;
        m_width = move_object.m_width;
        move_object.zeroing();
    }

public:
    Matrix() = default;

    Matrix(unsigned int height, unsigned int width)
        : m_height(height), m_width(width) {
        unsigned int n = height * width;
        m_memory = new T[n];
        for (int i = 0; i < n; i++) {
            m_memory[i] = T();
        }
    }

    Matrix(std::initializer_list<std::initializer_list<T>> lists) {
        m_height = lists.size();
        if (m_height != 0) {
            m_width = (*lists.begin()).size();
        }

        m_memory = new T[m_height * m_width];

        // move items from lists
        T *temp = m_memory;
        for (auto &it : lists) {
            for (auto &jt : it) {
                *temp = std::move(jt);
                temp++;
            }
        }
    }

    Matrix(const Matrix &copy_object) {
        copy_matrix(copy_object);
    }

    Matrix(Matrix &&move_object) noexcept {
        move_matrix(move_object);
    }

    ~Matrix() {
        delete[] m_memory;
    }

    [[nodiscard]] unsigned int height() const {
        return m_height;
    }

    [[nodiscard]] unsigned int width() const {
        return m_width;
    }

    void clear() {
        delete[] m_memory;
        zeroing();
    }

    Matrix &operator=(const Matrix &copy_object) {
        if (this != &copy_object) {
            delete[] m_memory;
            copy_matrix(copy_object);
        }
        return *this;
    }

    Matrix &operator=(Matrix &&move_object) noexcept {
        if (this != &move_object) {
            delete[] m_memory;
            move_matrix(move_object);
        }
        return *this;
    }

    T *operator[](unsigned int row) const {
        ASSERT(row < m_height, "wrong row");
        return m_memory + row * m_width;
    }
};

#endif  // GAME_ENGINE_MATRIX_HPP