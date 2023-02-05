#pragma once

#include "../utils.h"

/*
    matrix

     col
   +-----+
   |#####| r
   |#####| o
   |#####| w
   +-----+
*/
template<typename T>
class matrix {
    u32 colLen; // колво столбцов
    u32 rowLen; // колво строк
    T* memory;
    

    // обнуление
    void zeroing() {
        memory = 0;
        rowLen = colLen = 0;
    }

    void copy(const matrix& source) {
        colLen = source.colLen;
        rowLen = source.rowLen;
        u32 n = colLen * rowLen;
        memory = new T[n];
        for (u32 i = 0; i < n; i++) {
            memory[i] = source.memory[i];
        }
    }
    void move(matrix& source) {
        memory = source.memory;
        rowLen = source.rowLen;
        colLen = source.colLen;

        source.zeroing();
    }

public:

    u32 getRowLen() const {
        return rowLen;
    }
    u32 getColLen() const {
        return colLen;
    }

    matrix() {
        zeroing();
    }
    matrix(u32 rowLen, u32 colLen) {
        this->colLen = colLen;
        this->rowLen = rowLen;

        u32 n = colLen * rowLen;
        memory = new T[n];
        for (int i = 0; i < n; i++) {
            memory[i] = T();
        }
    }
    matrix(std::initializer_list<std::initializer_list<T>> lists) {

        rowLen = lists.size();
        if (rowLen != 0) {
            colLen = (*lists.begin()).size();
        }

        memory = new T[rowLen * colLen];

        T* temp = memory;
        for (auto& it : lists) {
            for (auto& jt : it) {
                *temp = jt;
                temp++;
            }
        }
    }
    matrix(const matrix& source) {
        copy(source);
    }
    matrix(matrix&& source) noexcept {
        move(source);
    }
    ~matrix() {
        delete[] memory;
    }

    void clear() {
        delete[] memory;
        memory = 0;
        colLen = rowLen = 0;
    }

    matrix& operator = (const matrix& source) {
        if (memory != source.memory) {
            delete[] memory;
            copy(source);
        }
        return *this;
    }
    matrix& operator = (matrix&& source) noexcept {
        if (memory != source.memory) {
            delete[] memory;
            move(source);
        }
        return *this;
    }

    T* operator [](u32 row) const {
        ASSERT(row < rowLen, "WRONG ROW");

        return memory + row * colLen;
    }
};
