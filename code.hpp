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
        
        
        // VALUE
        symbol* get_num(long long num);
        
        // ASSEMBLER COMMANDS
        void HALT();
  
};