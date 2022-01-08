#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "code.hpp"

Code::Code() {
    this->pc = 0;
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