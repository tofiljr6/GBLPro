#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <memory>

#include "code.hpp"
#include "data.hpp"
#include "symbol.hpp"

Code::Code(shared_ptr<Data> data) {
    this->pc = 0;
    this->data = data;
}

vector<string> Code::get_code() {
    return this->code;
}

void Code::end_code() {
    this->HALT();
}

// VALUES

symbol* Code::get_num(long long num) {
    // get constant if its declared
    if (this->data->check_context(std::to_string(num))) {
        return this->data->get_symbol(std::to_string(num));
    } else {  // declare const
        this->data->put_symbol(std::to_string(num), true);
        return this->data->get_symbol(std::to_string(num));
    }
}

// ASEMBLER COMMANDS

void Code::HALT() {
    this->code.push_back("HALT");
    this->pc++;
}