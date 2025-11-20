#include "../include/Map.h"

using namespace std;

Map::Map() : data(nullptr), capacity(0), length(0) {}

Map::Map(const Map& other) : capacity(other.capacity), length(other.length) {
    data = new Pair[capacity];
    for (size_t i = 0; i < length; i++) {
        data[i] = other.data[i];
    }
}

Map::~Map() {
    delete[] data;
}

Map& Map::operator=(const Map& other) {
    if (this != &other) {
        delete[] data;
        capacity = other.capacity;
        length = other.length;
        data = new Pair[capacity];
        for (size_t i = 0; i < length; i++) {
            data[i] = other.data[i];
        }
    }
    return *this;
}

void Map::resize() {
    capacity = (capacity == 0) ? 1 : capacity * 2;
    Pair* newData = new Pair[capacity];
    for (size_t i = 0; i < length; i++) {
        newData[i] = data[i];
    }
    delete[] data;
    data = newData;
}

void Map::insert(const string& key, const string& value) {
    for (size_t i = 0; i < length; i++) {
        if (data[i].key == key) {
            data[i].value = value;
            return;
        }
    }
    
    if (length >= capacity) {
        resize();
    }
    
    data[length++] = Pair(key, value);
}

bool Map::find(const string& key) const {
    for (size_t i = 0; i < length; i++) {
        if (data[i].key == key) {
            return true;
        }
    }
    return false;
}

string& Map::operator[](const string& key) {
    for (size_t i = 0; i < length; i++) {
        if (data[i].key == key) {
            return data[i].value;
        }
    }
    
    if (length >= capacity) {
        resize();
    }
    
    data[length++] = Pair(key, "");
    return data[length - 1].value;
}

const string& Map::at(const string& key) const {
    for (size_t i = 0; i < length; i++) {
        if (data[i].key == key) {
            return data[i].value;
        }
    }
    throw "Key not found";
}

bool Map::count(const string& key) const {
    return find(key);
}

size_t Map::size() const {
    return length;
}

bool Map::empty() const {
    return length == 0;
}

void Map::clear() {
    length = 0;
}

Map::Iterator Map::begin() {
    return Iterator(data);
}

Map::Iterator Map::end() {
    return Iterator(data + length);
}

Map::Iterator Map::begin() const {
    return Iterator(const_cast<Pair*>(data));
}

Map::Iterator Map::end() const {
    return Iterator(const_cast<Pair*>(data + length));
}
