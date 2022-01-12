#pragma once

#include <string>

using namespace std;

typedef struct symbol {
    string name;
    long long offset;
    long long value;
    bool is_init;
    bool is_const;
    bool is_array;
    long long array_start;
    long long array_end;
    long long array_size;

    
    symbol(string name, long long offset,  bool is_const) {
        this->name = name;
        this->offset = offset;
        this->is_init = false;
        this->is_const = is_const;
        this->is_array = false;
    }
    
    // arrays
    symbol(std::string name, long long offset, long long array_start, long long array_end) {
        this->name = name;
        this->offset = offset;
        this->is_init = false;
        this->is_const = false;
        this->is_array = true;
        this->array_start = array_start;
        this->array_end = array_end;
        this->array_size = array_end - array_start + 1;
    }
} symbol;