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

// EXPRESSIONS

void Code::load_value(symbol* sym) {
    this->check_init(sym);
    this->LOAD(sym->offset);
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

// MISC

void Code::generate_constant(long long value, long long offset) {
    // rejestr b - wartość symbolu **value**
    
    symbol* one = this->data->get_symbol("1");
    if (!one->is_init) {
        this->RESET("a");
        this->INC("a");
        // to co jest w rejestrze 'a', czyli 1, zapisz do pamięci o offset 1,
        // zakładamy, że 1 ma zawsze offset rowny 1 !!
        this->STORE(one->offset);
        one->is_init = true;
        one->value = 1;
    }
    
    this->generate_value_in_register(value, "b");
    this->generate_value_in_register(offset, "d");
    this->SWAP("b");
    this->code.push_back("STORE d");
    
}

void Code::generate_value_in_register(long long value, string r) {
    // generuje wartość **value** w rejestrze r
    long long digits[64];
    bool nonnegative = (value >= 0);
    
    // mała optymalizacja, gdy wartość którą chcemy zapisać jest mniejsza od 10
    // to zapisujemy ją INC lub odpowiednio DEC we wskazanym rejestrze
    if (llabs(value) < 10) {
        this->RESET(r);
        if (value > 0) {
            for (long long i = 0; i < value; i++) {
                this->INC(r);
            }
        } else {
            for (long long i = 0; i < -value; i++) {
                this->DEC(r);
            }
        }
    } else {
        // wartość którą chcemy zapisać na początku przechowujemy w rejestrze a
        // ze względu na to, że chcemy wykonywać SHIFT'a. Po wszystkich operacjach,
        // robimy SWAP'a to wskazanego przez nasz rejestru i tam przechowujemy wartość
        long long i = 0;
        while (value != 0) {
            digits[i] = llabs(value % 2);
            value /= 2;
            i++;
        }
        i--;
        this->RESET("a");
        
        symbol* one = this->data->get_symbol("1");
        this->generate_value_in_register(one->offset, "h");
        
        for (; i > 0; i--) {
            if (digits[i] == 1) {
                if (nonnegative) {
                    this->INC("a");
                } else {
                    this->DEC("a");
                }
            }
            // to shiftuje to rejestru a
            this->SHIFT("h");
        }

        if (digits[i] == 1) {
            if (nonnegative) {
                this->INC("a");
            } else {
                this->DEC("a");
            }
        }
        // obecna wartość **value** jest przechowywana w rejestrze 'a'
        this->SWAP(r);
    }
}

void Code::init_const(symbol* sym) {
    this->data->init_constant(sym->name, std::stoll(sym->name));
    this->generate_constant(std::stoll(sym->name), sym->offset);
}

void Code::check_init(symbol* sym) {
    // CONDITION:
    if (!sym->is_init) {
        if (sym->is_const) {
            this->init_const(sym);
        }
    }
}

// ASEMBLER COMMANDS

void Code::HALT() {
    this->code.push_back("HALT");
    this->pc++;
}

void Code::INC(string r) {
    this->code.push_back("INC " + r);
    this->pc++;
}

void Code::DEC(string r) {
    this->code.push_back("DEC " + r);
    this->pc++;
}

void Code::RESET(string r) {
    this->code.push_back("RESET " + r);
    this->pc++;
}

void Code::SHIFT(string r) {
    this->code.push_back("SHIFT " + r);
    this->pc++;
}

void Code::STORE(long long offset) {
    // zapisuje wartość, która jest w rejestrze a, w pamięci o offset'cie k†óry podajemy
    // przetrzymujemy offset w rejestrze g
    this->generate_value_in_register(offset, "g");
    this->code.push_back("STORE g");
    this->pc++;
}

void Code::LOAD(long long offset) {
    // wczytaj wartośc z pamięci o offset'cie to rejestru a
    this->generate_value_in_register(offset, "g");
    this->code.push_back("LOAD g");
    this->pc++;
}

void Code::SWAP(string r) {
    this->code.push_back("SWAP " + r);
    this->pc++;
}

void Code::PUT() {
    this->code.push_back("PUT ");
    this->pc++;
}


