#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "code.hpp"

Code::Code() {
    this->pc = 0;
    this->code.push_back("HALT");
}

vector<string> Code::get_code() {
    return this->code;
}