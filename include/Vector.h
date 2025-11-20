#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef>
#include <string>

template <typename T>
class Vector {
private:
    T* data;
    size_t capacity;
    size_t length;

    void resize() {
        capacity = (capacity == 0) ? 1 : capacity * 2;
        T* newData = new T[capacity];
        for (size_t i = 0; i < length; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

public:
    Vector() : data(nullptr), capacity(0), length(0) {}

    Vector(const Vector& other) : capacity(other.capacity), length(other.length) {
        data = new T[capacity];
        for (size_t i = 0; i < length; i++) {
            data[i] = other.data[i];
        }
    }

    ~Vector() {
        delete[] data;
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            length = other.length;
            data = new T[capacity];
            for (size_t i = 0; i < length; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    void push_back(const T& value) {
        if (length >= capacity) {
            resize();
        }
        data[length++] = value;
    }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }
    
    size_t size() const {
        return length;
    }
    
    bool empty() const {
        return length == 0;
    }
};
using VectorStr = Vector<std::string>;

#endif
