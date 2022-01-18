#include <string>
#include <vector>
#include <memory>
#include <stdlib.h>

#include "data.hpp"


using namespace std;

class Code {
    private:
        long long pc;   // program counter
        vector<string> code;
        shared_ptr<Data> data;
    public:
        Code(shared_ptr<Data> data);
        
        vector<string> get_code();
        void end_code();
        
        // COMMANDS
        void assign(symbol* var);
        void write(symbol* sym);
        
        // EXPRESSIONS
        void load_value(symbol* sym);
        void plus(symbol* a, symbol* b);
        void minus(symbol* a, symbol* b);
        void times(symbol* a, symbol* b);
        void div(symbol* a, symbol* b);
        
        // VALUE & PIDs
        symbol* get_num(long long num);
        symbol* pidentifier(string name);
        symbol* array_num_pidentifier(std::string name , long long num);
        symbol* array_pid_pidentifier(std::string name, std::string pid_name);
        
        
        // MISC
        void generate_constant(long long value, long long offset);
        void generate_value_in_register(long long value, string r);
        void init_const(symbol* sym);
        /**
         *  Checks if symbol is initialized,
         *  if not and symbol is constant, it initializes it,
         *  otherwise throws exception 
         */
        void check_init(symbol* sym);
        
        // ASSEMBLER COMMANDS
        void HALT();
        void INC(string r);
        void DEC(string r);
        void ADD(string r);
        void SUB(string r);
        void RESET(string r);
        void SHIFT(string r);
        void STORE(long long offset);
        void STORE(string r);
        void LOAD(long long offset);
        void LOAD(string r);
        void LOAD(symbol* sym);
        void SWAP(string r);
        void PUT();
        void JUMP(long long j);
        void JPOS(long long j);
        void JZERO(long long j);
        void JNEG(long long j);
        
        void printregister(); // only for tests
  
};