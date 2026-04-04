#ifndef YUEMU_H
#define YUEMU_H

#include <cstdint>
#include <string>
#include <map>
#include <stack>

using uint32_t = std::uint32_t;

class Yuemu {
public:
    Yuemu(std::string fpath);

private:
    inline static constexpr int DEBUG_LEVEL = 10;
    inline static constexpr uint32_t DEFAULT_PRINT_MEMORY_FROM = 0x8000;

    unsigned int read_instr_count = 0;
    unsigned int pc = 0;
    uint32_t regs[15] = {0};
    std::map<uint32_t, uint32_t> mem;
    std::stack<uint32_t> ret_stack;

    bool skip_auto_pc_incr = false;

    bool read_file_to_memory(std::string fpath);
    void run();
    void print_memory(uint32_t from_addr, uint32_t to_addr = 0xFFFFFFFF);

    static std::string get_instr_as_hex(uint32_t instr_int);
    static uint32_t sign_extend(uint32_t val, uint32_t no_of_bits);
    static int32_t to_signed(uint32_t val);
};

#endif