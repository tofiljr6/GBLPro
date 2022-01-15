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
        void put_array(std::string name, long long array_start, long long array_end);
        void put_array_cell(std::string name, long long offset);
        void put_addr_cell(std::string name, long long offset);
        symbol* get_symbol(string name);

        void init_constant(string name, long long value);
        
        void print_symbols();
};
