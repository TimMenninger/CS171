#ifndef HASHER_HPP
#define HASHER_HPP

#include <iostream>
#include <stdlib.h>
#include <functional>

inline void int_hash_combine(size_t &h, const int i) {
    h ^= std::hash<int>()(i) + 0x9e3779b9 + (h << 6) + (h >> 2);
}

inline void char_hash_combine(size_t &h, const int c) {
    h ^= std::hash<char>()(c) + 0x9e3779b9 + (h << 6) + (h >> 2);
}

template<class T>
struct CustomHasher {
    size_t operator() (const T &t) const {
        size_t hash = 0;
        size_t T_size = sizeof(T);
        size_t int_count = T_size / sizeof(int);
        size_t trailing_char_count = (T_size % sizeof(int)) / sizeof(char);
        int *sub_hash_start = (int*) &t;
        for (uint i = 0; i < int_count; i++) {
            int_hash_combine(hash, *sub_hash_start);
            sub_hash_start++;
        }
        char *trailing_sub_hash_start = (char*) sub_hash_start;
        for (uint i = 0; i < trailing_char_count; i++) {
            char_hash_combine(hash, *trailing_sub_hash_start);
            trailing_sub_hash_start++;
        }
        return hash;
    }
};

#endif