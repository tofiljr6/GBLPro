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

// ASEMBLER COMMANDS

void Code::HALT() {
    this->code.push_back("HALT");
    this->pc++;
}