#ifndef MAP_H
#define MAP_H

#include <string>
#include <cstddef>

class Map {
public:
    struct Pair {
        std::string key;
        std::string value;
        
        Pair() {}
        Pair(const std::string& k, const std::string& v) : key(k), value(v) {}
    };
    
private:
    Pair* data;
    size_t capacity;
    size_t length;
    
    void resize();
    
public:
    Map();
    Map(const Map& other);
    ~Map();
    
    Map& operator=(const Map& other);
    
    void insert(const std::string& key, const std::string& value);
    bool find(const std::string& key) const;
    std::string& operator[](const std::string& key);
    const std::string& at(const std::string& key) const;
    
    bool count(const std::string& key) const;
    size_t size() const;
    bool empty() const;
    void clear();
    
    class Iterator {
    private:
        Pair* ptr;
    public:
        Iterator(Pair* p) : ptr(p) {}
        
        Pair& operator*() { return *ptr; }
        const Pair& operator*() const { return *ptr; }
        Pair* operator->() { return ptr; }
        const Pair* operator->() const { return ptr; }
        Iterator& operator++() { ++ptr; return *this; }
        bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
    };
    
    Iterator begin();
    Iterator end();
    Iterator begin() const;
    Iterator end() const;
};

#endif
