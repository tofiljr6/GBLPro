#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <memory>

#include "code.hpp"
#include "data.hpp"
#include "symbol.hpp"
#include "labels.hpp"

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

// COMMANDS

void Code::assign(symbol* var) {
    // offsetowi zmiennej, przypisujemy aktualną zawartość rejestru a
    symbol* test = this->data->get_symbol(var->name);
    test->is_init = true;
    
    // cout << var->name << ":" << var->offset << endl;
    
    
    if (var->is_iterator) {
        throw std::string(var->name + " - try change value of iterator");
    }

    if (var->is_addr_cell) {
        // cout << var->name << ":" << var->offset << endl;
        
        // this->STORE("f");
        // this->PUT();
        this->SWAP("b");
        this->generate_value_in_register(var->offset, "a");
        this->LOAD("a");
        this->SWAP("c");
        this->SWAP("b");
        this->STORE("c");
    } 
    else if(var->is_array_cell) {
        this->SWAP("f");
        this->generate_value_in_register(test->offset, "b");
        this->SWAP("f");
        this->STORE("b");
        // this->STORE(test->offset); // #5
    }
    else if (var->is_init && !var->is_const) {
        this->SWAP("f");
        this->generate_value_in_register(test->offset, "b");
        this->SWAP("f");
        this->STORE("b");
    }
    else {
        this->STORE(test->offset);
    }
}

void Code::if_block(cond_label* label) {
    cout << label->go_to << ":" << this->pc - label->go_to << endl;
    this->code[label->go_to] += std::to_string(this->pc - label->go_to);
}

cond_label* Code::if_else_first_block(cond_label* label) {
    this->JUMP();
    
    this->if_block(label);
    
    label->go_to = this->pc - 1;
    return label;
}

void Code::if_else_second_block(cond_label* label) {
    this->if_block(label);
}

void Code::while_block(cond_label* label) {
    this->JUMP(label->start - this->pc);
    this->if_block(label);
}

cond_label* Code::repeat_until_first_block() {
    return new cond_label(0, this->pc - 1);
}

void Code::repeat_until_second_block(cond_label* label, cond_label* cond) {
    this->JUMP(label->go_to  - this->pc);
    this->if_block(cond);
}

for_label* Code::for_first_block(std::string iterator_name, symbol* start, symbol* end, bool to) {
    symbol* iterator = this->data->get_symbol(iterator_name);
    this->check_init(start);
    this->check_init(end);
    
    std::string end_name = "END" + std::to_string(this->data->memory_offset);
    this->data->put_symbol(end_name, true);
    symbol* end_tmp = this->data->get_symbol(end_name);
    
    this->LOAD(end);
    this->SWAP("b");
    long long stependtmpstart = this->pc;
    this->generate_value_in_register(end_tmp->offset, "a");
    long long stependtmpend = this->pc - stependtmpstart;
    this->SWAP("b");
    this->STORE("b");
    
    // init iterator
    this->LOAD(start);
    this->SWAP("b");
    long long stepiteratorstart = this->pc;
    this->generate_value_in_register(iterator->offset, "a");
    long long stepiteratorend = this->pc - stepiteratorstart;
    this->SWAP("b");
    this->STORE("b");
    
    long long sum = stependtmpend + stepiteratorend;
    
    this->generate_value_in_register(end_tmp->offset, "a");
    this->LOAD("a");
    this->SWAP("b");
    this->generate_value_in_register(iterator->offset, "a");
    this->LOAD("a");
    
    this->SUB("b");
    // cout << "JUMPING 2:" << this->pc << ":" << sum << endl;
    cond_label* label = new cond_label(this->pc + sum + 4, this->pc );
    
    this->generate_value_in_register(end_tmp->offset, "a"); // -----|
    this->LOAD("a");  // ładowanie wartości, koniec zakresu         |
    this->SWAP("b");                                        //      |
    this->generate_value_in_register(iterator->offset, "a");// -----|---> to się wykona sum razy, musimy to wiedzieć, żeby wiedzieć ile potrzebujemy SHIFT'ów i INC/DEC aby osoągnąc offset
    this->LOAD("a"); // ładowanie wartości, iterator

    this->SUB("b"); // odejmowanie i sprawdzenie dopóki będzie spełniać warunek
    
    if (to) {
        this->JPOS();
    } else {
        this->JNEG();
    }
    
    return new for_label(iterator, start, end_tmp, label, false);
}

void Code::for_second_block(for_label* label, bool to) {
    // cout << ":" << label->iterator->offset << endl;
    this->generate_value_in_register(label->iterator->offset, "a");
    this->LOAD("a");

    if (to) {
        this->INC("a");
    } else {
        this->DEC("a");
    }
    
    this->SWAP("b");
    this->generate_value_in_register(label->iterator->offset, "c");
    this->SWAP("b");
    this->STORE("c");

    this->JUMP(- (this->pc - label->jump_label->go_to));
    this->code[label->jump_label->start] += to_string(this->pc - label->jump_label->start);
}

void Code::write(symbol* sym) {
    // czytanie różni się od tego jak symbol chcemy wydrukować, musimy załadować
    // offset offsetu aby dostać się do wartości
    if (sym->is_addr_cell) {
        this->check_init(sym);
        this->generate_value_in_register(sym->offset, "b");
        this->LOAD("a");
        this->LOAD("a");
        this->PUT();
    } else if(sym->is_array_cell) {
        this->check_init(sym);
        this->generate_value_in_register(sym->offset, "a");
        this->LOAD("a");
        this->PUT();
    // } else if (sym->is_iterator) {
    //     cout << "NAME:" << sym->name << endl;
    //     this->generate_value_in_register(sym->offset, "a");
    //     this->LOAD("a");
    //     this->PUT();
    //     // this->HALT();
    } else {
        this->check_init(sym);
        this->LOAD(sym->offset);
        this->PUT();
    }
}

void Code::read(symbol* sym) {
    this->GET();

    if (!sym->is_addr_cell) {
        // zmienna
        this->SWAP("b");
        this->generate_value_in_register(sym->offset, "c");
        this->SWAP("b");
        this->STORE("c");
        sym->is_init = true;
    } else {
        this->SWAP("c");
        this->generate_value_in_register(sym->offset, "a");
        this->LOAD("a");
        this->SWAP("c");
        this->STORE("c");
    } 
}

// EXPRESSIONS

void Code::load_value(symbol* sym) {
    // metoda załadowuje wartość w zależności o rodzaju symbolu
    // różnica jest wtedy gdy chcemy przypisać zmiennej arr[i] zmienna arr[j]
    // wtedy musimy załadować wskazanie offset'u
    this->check_init(sym);
    if (sym->is_addr_cell) {
        cout << "\t" << sym->name << ":" << sym->offset << endl;
        this->LOAD(sym->offset);
        this->LOAD("a");
    } 
    else if(sym->is_array_cell) {
        this->LOAD(sym->offset);
    }
    else { // sym->is_constant
        this->LOAD(sym->offset);
    }
}

void Code::plus(symbol* a, symbol* b) {
    this->check_init(a);
    this->check_init(b);
    
    if (a->is_addr_cell && b->is_addr_cell) {
        // ładujemy z pamięci offset zmiennej a, następnie load jej offset i mmay jej wartość
        // powtarzamy czynność dla symbolu b  oraz wyniki
        // zachowujemy w rejestrze 'a' - ze względy na to że potem nastąpi ASSIGN i 
        // on wymaga tego, żeby wartość którą chcemy zapisać była w rejestrze 'a'
        this->LOAD(a->offset);
        this->LOAD("a");
        this->SWAP("f");
        this->LOAD(b->offset);
        this->LOAD("a");
        this->ADD("f");
    } else if (!a->is_addr_cell && b->is_addr_cell) {
        this->LOAD(b->offset);
        this->LOAD("a");
        this->SWAP("f");
        this->LOAD(a->offset);
        this->ADD("f");
    } else if (a->is_addr_cell && !b->is_addr_cell) {
        this->LOAD(a->offset);
        this->LOAD("a");
        this->SWAP("f");
        this->LOAD(b->offset);
        this->ADD("f");
    } else {
        // cout << a->value << "+" << b->value << endl;
        this->LOAD(a->offset);
        this->SWAP("b");
        this->LOAD(b->offset);
        this->ADD("b");
    }
}

void Code::minus(symbol* a, symbol* b) {
    this->check_init(a);
    this->check_init(b);
    
    if (a->is_addr_cell && b->is_addr_cell) {
        // ładujemy z pamięci offset zmiennej a, następnie load jej offset i mmay jej wartość
        // powtarzamy czynność dla symbolu b  oraz wyniki
        // zachowujemy w rejestrze 'a' - ze względy na to że potem nastąpi ASSIGN i 
        // on wymaga tego, żeby wartość którą chcemy zapisać była w rejestrze 'a'
        this->SWAP("g");
        this->RESET("a");
        this->DEC("a");
        this->SWAP("g");

        this->LOAD(a->offset);
        this->LOAD("a");
        this->SWAP("b");
        
        this->LOAD(b->offset);
        this->LOAD("a");
        this->SWAP("b");
        this->SUB("b");
    } else if (!a->is_addr_cell && b->is_addr_cell) {
        this->LOAD(a->offset);
        this->SWAP("f");
        this->RESET("a");
        this->LOAD(b->offset);
        this->LOAD("a");
        this->SWAP("f");
        this->SUB("f");
    } else if (a->is_addr_cell && !b->is_addr_cell) {
        this->LOAD(a->offset);
        this->LOAD("a");
        this->SWAP("f");
    
        // NOTE
        // this->generate_value_in_register(b->value, "a");
        // tak nie możemy napisać, bo 
        //  1* b->value jest tylko i wyłącznie dla stałych
        //  2* nawet jeżli powyższa wartość była by spełniona
        //      to musimu liczbyć nowe wartości, a jak robimy skoki, to byśmy
        //      musieli wiedzieć o ile skakać, niewykonalne
        //      : (patrz. podobnie jak w FOR)
        
        this->LOAD(b->offset);
        this->SWAP("f");
        this->SUB("f");
    } else {
        this->LOAD(a->offset);
        this->SWAP("b");
        this->LOAD(b->offset);
        this->SWAP("b");
        this->SUB("b");
    }
}

void Code::times(symbol* a, symbol* b) {
    this->check_init(a);
    this->check_init(b);
    
    // small optymalisation
    if (b->is_const && b->value == 2) {
        // cout << "1" << endl;
        this->LOAD(a);
        this->RESET("b");
        this->INC("b");
        this->SHIFT("b");
        return;
    }
    // small optymalisation
    if (a->is_const && a->value == 2) {
        // cout << "2" << endl;
        this->LOAD(b);
        this->RESET("b");
        this->INC("b");
        this->SHIFT("b");
        return;
    }
    
    if (a->is_addr_cell && b->is_addr_cell) {
        this->LOAD(a->offset);
        this->LOAD("a");
        this->SWAP("b");
        this->LOAD(b->offset);
        this->LOAD("a");
        this->SWAP("b");
    } else if (!a->is_addr_cell && b->is_addr_cell) {
        this->LOAD(b->offset);
        this->LOAD("a");
        this->SWAP("b");
        this->LOAD(a->offset);
        this->SWAP("b");
    } else if (a->is_addr_cell && !b->is_addr_cell) {
        this->LOAD(a->offset);
        this->LOAD("a");
        this->SWAP("b");
        this->LOAD(b->offset);
        this->SWAP("b");
    } else {
        this->LOAD(a->offset);
        this->SWAP("b");
        this->LOAD(b->offset);
        this->SWAP("b");
    }
    
    
    this->RESET("c");
    this->RESET("d");
    this->RESET("e");
    this->RESET("f");
    this->RESET("g");
    this->RESET("h");
    
    this->INC("c");
    this->DEC("d");

    // check a
    this->JZERO(27);
    this->JPOS(4);
    this->DEC("h");
    this->SWAP("f");
    this->SUB("f");
    this->SWAP("b");
    this->SWAP("g");
    this->SWAP("b");
    
    // check b
    this->JZERO(20);
    this->SWAP("b");
    this->RESET("f");
    this->SWAP("f");
    
    // parzystposc B
    this->ADD("b");
    this->SHIFT("d");
    this->SHIFT("c");
    this->SUB("b");
    this->JZERO(4);
    this->SWAP("f");
    this->ADD("g");
    this->JUMP(2);
    this->SWAP("f");
    this->SWAP("g");
    this->SHIFT("c");
    this->SWAP("g");
    this->SWAP("b");
    this->SHIFT("d");
    this->SWAP("b");
    this->JUMP(-20);
    this->SWAP("h");
    this->JZERO(4);
    this->RESET("a");
    this->SUB("b");
    this->JUMP(2);
    this->SWAP("b");
    
    // this->printregister();
}

void Code::mod(symbol* a, symbol *b) {
    this->check_init(a);
    this->check_init(b);
    
    // przygotowanie zależne od zmiennych
    // tak aby w rejestrze
    // r_b znajdowała się wartość symbolu a
    // r_c znajdowała się wartość symbolu b
    // w r_a przechowywać będziemy ile już razy przemnożyliśmy
    if (a->is_addr_cell && b->is_addr_cell) {
        this->LOAD(a->offset);
        this->LOAD("a");
        this->SWAP("b");
        this->LOAD(b->offset);
        this->LOAD("a");
        // this->SWAP("a");
    } else if (!a->is_addr_cell && b->is_addr_cell) {
        this->LOAD(b->offset);
        this->LOAD("a");
        this->SWAP("c");
        this->LOAD(a->offset);
        // this->SWAP("a");
    } else if (a->is_addr_cell && !b->is_addr_cell) {
        this->LOAD(a->offset);
        this->LOAD("a");
        this->SWAP("b");
        this->LOAD(b->offset);
        // this->SWAP("a");
    } else {
        this->LOAD(a->offset);
        this->SWAP("b");
        this->LOAD(b->offset);
        // this->SWAP("a");
    }
    
    this->RESET("g");
    this->RESET("e");
    this->RESET("h");
    this->RESET("f");
    this->RESET("c");
    this->INC("c");
    this->RESET("d");
    this->DEC("d");
    
    this->RESET("g");
    this->RESET("e");
    this->RESET("h");
    this->RESET("f");
    this->RESET("c");
    
    this->SWAP("c");
    this->ADD("c");
    this->SWAP("c");
    this->JZERO(74);
    this->JPOS(6);
    this->INC("e");
    this->INC("e");
    this->SWAP("f");
    this->SUB("f");
    this->RESET("f");
    this->SWAP("b");
    this->JZERO(66);
    this->JPOS(5);
    this->INC("e");
    this->SWAP("f");
    this->SUB("f");
    this->RESET("f");
    // h=dlugosc bitowa a
    this->SWAP("g");
    this->ADD("g");
    this->JZERO(4);
    this->SHIFT("d");
    this->INC("h");
    this->JUMP(-3);
    this->SWAP("g");
    // g dlugosc a wartosc
    this->SWAP("b");
    // g=dlugosc bitowa b
    this->SWAP("f");
    this->ADD("f");
    this->JZERO(4);
    this->SHIFT("d");
    this->INC("g");
    this->JUMP(-3);
    this->SWAP("f");
    this->SWAP("b");
    this->SWAP("h");
    // roznica dlugosci
    this->SUB("g");
    this->JNEG(44);
    this->RESET("f");
    this->SWAP("f");
    this->ADD("f");
    this->SWAP("f");
    this->INC("f");
    // a roznica h g
    // wyrownanie do lewej
    this->JZERO(10);
    this->SWAP("b");
    this->INC("d");
    this->INC("d");
    this->SHIFT("d");
    this->DEC("d");
    this->DEC("d");
    this->DEC("b");
    this->SWAP("b");
    this->JUMP(-9);
    this->SWAP("h");
    // a i wyrownane b
    this->RESET("h");
    this->SWAP("f");
    // petla
    this->JZERO(23);
    this->RESET("g");
    this->SWAP("g");
    this->ADD("f");
    this->SUB("b");
    this->SWAP("h");
    this->INC("d");
    this->INC("d");
    this->SHIFT("d");
    this->DEC("d");
    this->DEC("d");
    this->SWAP("h");
    this->JNEG(5);
    this->INC("h");
    this->SWAP("f");
    this->SUB("b");
    this->SWAP("f");
    this->SWAP("b");
    this->SHIFT("d");
    this->SWAP("b");
    this->SWAP("g");
    this->DEC("a");
    this->JUMP(-22);
    this->SWAP("f");
    this->JUMP(2);
    this->SWAP("h");
    // spr
    this->SWAP("e");
    this->JZERO(21);
    this->DEC("a");
    this->JZERO(13);
    this->DEC("a");
    this->JZERO(7);
    // dwa minusy
    this->RESET("g");
    this->SWAP("e");
    this->SWAP("g");
    this->SUB("g");
    this->SWAP("e");
    this->JUMP(11);
    // minus w B
    this->SWAP("e");
    this->ADD("c");
    this->SWAP("e");
    this->JUMP(7);
    // minus w A
    this->RESET("g");
    this->SWAP("e");
    this->SWAP("g");
    this->SUB("g");
    this->ADD("c");
    this->SWAP("e");
    this->SWAP("e");
    this->RESET("c");
    this->INC("c");
}

void Code::div(symbol* a, symbol* b) {
    this->check_init(a);
    this->check_init(b);
    
    if (b->is_const && b->value == 2) {
        this->LOAD(a->offset);
        this->RESET("b");
        this->DEC("b");
        this->SHIFT("b");
        return;
    }

    if (a->is_addr_cell && b->is_addr_cell) {
        this->LOAD(a->offset);
        this->LOAD("a");
        this->SWAP("b");
        this->LOAD(b->offset);
        this->LOAD("a");
        // this->SWAP("a");
    } else if (!a->is_addr_cell && b->is_addr_cell) {
        this->LOAD(b->offset);
        this->LOAD("a");
        this->SWAP("b");
        this->LOAD(a->offset);
        // this->SWAP("a");
    } else if (a->is_addr_cell && !b->is_addr_cell) {
        this->LOAD(a->offset);
        this->LOAD("a");
        this->SWAP("b");
        this->LOAD(b->offset);
        // this->SWAP("a");
    } else {
        this->LOAD(a->offset);
        this->SWAP("b");
        this->LOAD(b->offset);
        // this->SWAP("a");
    }

    this->RESET("c");
    this->RESET("d");
    this->RESET("e");
    this->RESET("h");
    this->RESET("f");

    this->INC("c");
    this->DEC("d");

    this->JZERO(66);
    this->JPOS(6);
    this->INC("e");
    this->INC("e");
    this->SWAP("f");
    this->SUB("f");
    this->RESET("f");
    this->SWAP("b");
    this->JZERO(58);
    this->JPOS(5);
    this->INC("e");
    this->SWAP("f");
    this->SUB("f");
    this->RESET("f");
    // h=dlugosc bitowa a
    this->SWAP("g");
    this->ADD("g");
    this->JZERO(4);
    this->SHIFT("d");
    this->INC("h");
    this->JUMP(-3);
    this->SWAP("g");
    // g dlugosc a wartosc
    this->SWAP("b");
    // g=dlugosc bitowa b
    this->SWAP("f");
    this->ADD("f");
    this->JZERO(4);
    this->SHIFT("d");
    this->INC("g");
    this->JUMP(-3);
    this->SWAP("f");
    this->SWAP("b");
    this->SWAP("h");
    // roznica dlugosci
    this->SUB("g");
    this->JNEG(35);
    this->RESET("f");
    this->SWAP("f");
    this->ADD("f");
    this->SWAP("f");
    this->INC("f");
    // a roznica h g
    // wyrownanie do lewej
    this->JZERO(6);
    this->SWAP("b");
    this->SHIFT("c");
    this->DEC("b");
    this->SWAP("b");
    this->JUMP(-5);
    this->SWAP("h");
    // a i wyrownane b
    this->RESET("h");
    this->SWAP("f");
    // petla
    this->JZERO(19);
    this->RESET("g");
    this->SWAP("g");
    this->ADD("f");
    this->SUB("b");
    this->SWAP("h");
    this->SHIFT("c");
    this->SWAP("h");
    this->JNEG(5);
    this->INC("h");
    this->SWAP("f");
    this->SUB("b");
    this->SWAP("f");
    this->SWAP("b");
    this->SHIFT("d");
    this->SWAP("b");
    this->SWAP("g");
    this->DEC("a");
    this->JUMP(-18);
    this->JUMP(6);
    this->SWAP("h");
    this->SWAP("f");
    this->ADD("f");
    this->SWAP("f");
    this->RESET("h");
    this->SWAP("h");

    // sprawdzenie
    this->SWAP("e");
    this->JZERO(16);
    this->DEC("a");
    this->JZERO(4);
    this->DEC("a");
    this->JZERO(2);
    // dwa minusy
    this->JUMP(11);
    // jeden minus
    this->RESET("g");
    this->SWAP("e");
    this->SWAP("g");
    this->SUB("g");
    this->SWAP("e");
    this->SWAP("f");
    this->JZERO(4);
    this->SWAP("e");
    this->SUB("c");
    this->SWAP("e");
    this->SWAP("e");
}

cond_label* Code::eq(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    this->JZERO(2);
    long long ADDr = this->pc;
    this->JUMP();
    
    return new cond_label(start, ADDr);
}

cond_label* Code::neq(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    this->JZERO();
    long long ADDr = this->pc - 1;
    
    return new cond_label(start, ADDr);
}

cond_label* Code::le(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    this->JNEG(2);
    long long ADDr = this->pc;
    this->JUMP();
    
    return new cond_label(start, ADDr);
}

cond_label* Code::ge(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    // this->PUT();
    this->JPOS(2);
    long long ADDr = this->pc;
    this->JUMP();
    
    return new cond_label(start, ADDr);
}

cond_label* Code::leq(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    this->JPOS();
    long long ADDr = this->pc - 1;
    
    return new cond_label(start, ADDr);
}

cond_label* Code::geq(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    this->JNEG();
    long long ADDr = this->pc - 1;

    cond_label* label = new cond_label(start, ADDr);
    return label;
}

void Code::printregister() {
    this->PUT();
    this->SWAP("b");
    this->PUT();
    this->SWAP("b");
    this->SWAP("c");
    this->PUT();
    this->SWAP("c");
    this->SWAP("d");
    this->PUT();
    this->SWAP("d");
    this->SWAP("e");
    this->PUT();
    this->SWAP("e");
    this->SWAP("f");
    this->PUT();
    this->SWAP("f");
    this->SWAP("g");
    this->PUT();
    this->SWAP("g");
}

// VALUES & PIDs

symbol* Code::get_num(long long num) {
    // get constant if its DEClared
    if (this->data->check_context(std::to_string(num))) {
        return this->data->get_symbol(std::to_string(num));
    } else {  // DEClare const
        this->data->put_symbol(std::to_string(num), true);
        return this->data->get_symbol(std::to_string(num));
    }
}

symbol* Code::pidentifier(string name) {
    symbol* sym = this->data->get_symbol(name);
    // CONDITION
    if (sym != nullptr) {
        if (!sym->is_array) {
            return sym;
        } else {
            throw std::string(name + " - wrong usage, " + name + " is an array");
        }
    } else {
        throw string(name + " - symbol does not exist");
    }
}

symbol* Code::array_num_pidentifier(string name, long long num) {
    symbol* sym = this->data->get_symbol(name);
    if (sym != nullptr) {
        if (sym->is_array) {
            if (sym->array_start <= num && sym->array_end >= num) {
                long long offset = num - sym->array_start + sym->offset;
                if (!this->data->check_context(name + to_string(num))) {
                    this->data->put_array_cell(name + to_string(num), offset);
                }
                symbol* ret_sym = this->data->get_symbol(name + to_string(num));
                return ret_sym;
            } else {
                throw std::string(name + " - index out of boundry");
            }
        } else {
            throw std::string(name + " - is not an array");
        }
    } else {
        throw std::string(name + " - array does not exist");
    }
}

symbol* Code::array_pid_pidentifier(std::string name, std::string pid_name) {
    symbol* array = this->data->get_symbol(name);
    symbol* var = this->data->get_symbol(pid_name);

    if (array != nullptr && var != nullptr) {
        if (array->is_array) {
            // init constants
            symbol* array_start = this->get_num(array->array_start);
            this->check_init(array_start);
            symbol* array_offset = this->get_num(array->offset);
            this->check_init(array_offset);
            
            this->generate_value_in_register(var->offset, "a");
            this->LOAD("a");
            this->SWAP("d");
            
            this->generate_value_in_register(array_start->offset, "b");
            this->LOAD("b");
            this->SWAP("b");

            this->generate_value_in_register(array_offset->offset, "c");
            this->LOAD("c");
            this->SWAP("c");
            
            this->SWAP("d");
            this->SUB("b");
            this->ADD("c");
            
            // naszą wartość, offset kommóki do którego chcemy zapisać, musimy napoczątku przechować w innym
            // rejestrze, ponieważ zaraz będzimey robić generate_value_in_register, który używa rejestru 'a',
            // i byśmy stracili
            this->SWAP("f");
            
            // save address
            this->data->put_addr_cell("TMP" + std::to_string(this->data->memory_offset), this->data->memory_offset);
            symbol* cell_address = this->data->get_symbol("TMP" + std::to_string(this->data->memory_offset));
            this->generate_value_in_register(cell_address->offset, "b");
            
            this->SWAP("f"); 
            
            this->STORE("b");
            this->data->memory_offset++;
            
            this->SWAP("f");
            return cell_address;
        } else {
            throw std::string(array->name + " - is not an array");
        }
    } else {
        if (array == nullptr) {
            throw std::string(name + " - array does not exsit");
        } else {
            throw std::string(pid_name + " - is not declared");
        }
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
    this->STORE("d");
    
}

void Code::generate_value_in_register(long long value, string r) {
    // generuje wartość **value** w rejestrze r
    long long digits[64];
    bool nonnegative = (value >= 0);
    
    // mała optymalizacja, gdy wartość którą chcemy zapisać jest mniejsza od 10
    // to zapisujemy ją INC lub odpowiednio DEC we wskazanym rejestrze
    if (llabs(value) < 10) {
        this->RESET("   " + r);
        if (value > 0) {
            for (long long i = 0; i < value; i++) {
                this->INC("   " + r);
            }
        } else {
            for (long long i = 0; i < -value; i++) {
                this->DEC("   " + r);
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
        this->RESET("   a");
        
        symbol* one = this->data->get_symbol("1");
        this->generate_value_in_register(one->offset, "   h");
        
        for (; i > 0; i--) {
            if (digits[i] == 1) {
                if (nonnegative) {
                    this->INC("   a");
                } else {
                    this->DEC("   a");
                }
            }
            // to SHIFTuje to rejestru a
            this->SHIFT("   h");
        }

        if (digits[i] == 1) {
            if (nonnegative) {
                this->INC("   a");
            } else {
                this->DEC("   a");
            }
        }
        // obecna wartość **value** jest przechowywana w rejestrze 'a'
        this->SWAP("   " + r);
    }
}

void Code::init_const(symbol* sym) {
    this->data->init_constant(sym->name, std::stoll(sym->name));
    this->generate_constant(std::stoll(sym->name), sym->offset);
}

void Code::check_init(symbol* sym) {
    if (sym->is_array_cell || sym->is_addr_cell || sym->is_iterator) {
        return;
    }
    if (!sym->is_init) {
        if (sym->is_const) {
            this->init_const(sym);
        } else {
            cout << sym->name << " - symbol may be uninitialized" << std::endl;
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

void Code::ADD(string r) {
    this->code.push_back("ADD " + r);
    this->pc++;
}

void Code::SUB(string r) {
    this->code.push_back("SUB " + r);
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
    // Używane rejestry: g, a, h 
    
    // zapisuje wartość, która jest w rejestrze a, w pamięci o offset'cie k†óry podajemy
    // przetrzymujemy offset w rejestrze g
    this->generate_value_in_register(offset, "g");
    this->code.push_back("STORE g");
    this->pc++;
}

void Code::STORE(string r) {
    // zapisz wartość z pamięci w rejestrze 'r' do rejestru a
    this->code.push_back("STORE " + r);
    this->pc++;
}

void Code::LOAD(long long offset) {
    // wczytaj wartośc z pamięci o offset'cie do rejestru a
    this->generate_value_in_register(offset, "g");
    this->code.push_back("LOAD g");
    this->pc++;
}

void Code::LOAD(string r) {
    // wczytaj wartość z pamięci w rejestrze 'r' do rejestru a
    this->code.push_back("LOAD " + r);
    this->pc++;
}

void Code::LOAD(symbol* sym) {
    if (sym->is_addr_cell) {
        this->generate_value_in_register(sym->offset, "g");
        this->code.push_back("LOAD g");
        this->LOAD("a");
        this->pc++;
    } else {
        this->generate_value_in_register(sym->offset, "g");
        this->code.push_back("LOAD g");
        this->pc++;
    }
}


void Code::SWAP(string r) {
    this->code.push_back("SWAP " + r);
    this->pc++;
}

void Code::PUT() {
    this->code.push_back("PUT ");
    this->pc++;
}

void Code::JUMP() {
    this->code.push_back("JUMP ");
    this->pc++;
}

void Code::JUMP(long long j) {
    this->code.push_back("JUMP " + to_string(j));
    this->pc++;
}

void Code::JPOS(long long j) {
    this->code.push_back("JPOS " + to_string(j));
    this->pc++;
}

void Code::JPOS() {
    this->code.push_back("JPOS ");
    this->pc++;
}

void Code::JZERO(long long j) {
    this->code.push_back("JZERO " + to_string(j));
    this->pc++;
}

void Code::JZERO() {
    this->code.push_back("JZERO ");
    this->pc++;
}

void Code::JNEG(long long j) {
    this->code.push_back("JNEG " + to_string(j));
    this->pc++;
}

void Code::JNEG() {
    this->code.push_back("JNEG ");
    this->pc++;
}

void Code::GET() {
    this->code.push_back("GET ");
    this->pc++;
}

