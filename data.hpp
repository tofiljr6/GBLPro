#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "symbol.hpp"

class Data {
    private:
        unordered_map<string, shared_ptr<symbol>> sym_map;
    public:
        long long memory_offset;

        Data();

        bool check_context(string name);
        void put_symbol(string name, bool is_const);
        symbol* get_symbol(string name);

        void init_constant(string name, long long value);
        
        void print_symbols();
};