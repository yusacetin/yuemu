#include <string>
#include <map>

class Yuemu {
    public:
        Yuemu(std::string fpath);
    
    private:
        const int DEBUG_LEVEL = 10;

        int read_instr_count = 0;
        int pc = 0;
        int regs[256];
        std::map<unsigned int, int> mem;

        bool skip_auto_pc_incr = false;

        void read_file_to_memory(std::string fpath);
        void run();

        static std::string get_instr_as_hex(unsigned int instr_int);
};
