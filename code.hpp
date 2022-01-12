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
        
        // EXPRESSIONS
        void load_value(symbol* sym);
        
        // VALUE
        symbol* get_num(long long num);
        
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
        void RESET(string r);
        void SHIFT(string r);
        void STORE(long long offset);
        void LOAD(long long offset);
        void SWAP(string r);
        void PUT();
  
};