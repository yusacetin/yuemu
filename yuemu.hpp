#include <cstdint>
#include <string>
#include <map>

class Yuemu {
    public:
        Yuemu(std::string fpath);
    
    private:
        const int DEBUG_LEVEL = 10;

        unsigned int read_instr_count = 0;
        unsigned int pc = 0;
        uint32_t regs[256];
        std::map<uint32_t, uint32_t> mem;

        bool skip_auto_pc_incr = false;

        void read_file_to_memory(std::string fpath);
        void run();

        static std::string get_instr_as_hex(uint32_t instr_int);
        static uint32_t sign_extend(uint32_t val, uint32_t no_of_bits);
        static int32_t to_signed(uint32_t val);
};
