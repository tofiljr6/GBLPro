#include <string>
#include <vector>
#include <stdlib.h>


using namespace std;

class Code {
    private:
        long long pc;   // program counter
        vector<std::string> code;
    public:
        Code();
        
        vector<string> get_code();
        void end_code();
        
        // ASSEMBLER COMMANDS
        void HALT();
  
};