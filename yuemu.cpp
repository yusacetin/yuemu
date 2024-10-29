#include <cstdint>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "yuemu.hpp"

Yuemu::Yuemu(std::string fpath) {
    read_file_to_memory(fpath);
    run();
}

void Yuemu::run() {
    // TODO unsigned and signed types are mixed up for register array and maybe memory map
    std::cout << "Running program\n";
    while (pc != read_instr_count) {
        uint32_t instr = mem[pc]; // fetch
        uint32_t opcode_category = instr >> 28 & 0xF;
        uint32_t opcode_id = instr >> 24 & 0xF;

        switch (opcode_category) {
            case 0x0: { // memory
                switch (opcode_id) {
                    case 0x0: { // load immediate
                        uint32_t rd = instr >> 16 & 0xFF; // 8 bits
                        uint32_t val = instr & 0xFFFF; // 16 bits

                        // sign extend
                        if ((val >> 15 & 0b1) == 1) { // if negative
                            val = sign_extend(val, 16);
                        }

                        regs[rd] = val;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "loadm: rd=" << rd << ", val=" << to_signed(val) << "\n";
                        }
                        break;
                    }

                    case 0x1: { // load register indirect
                        uint32_t rd = instr >> 16 & 0xFF; // 8 bits
                        uint32_t raddr = instr >> 8 & 0xFF; // 8 bits
                        regs[rd] = mem[regs[raddr]];

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "loadr: rd=" << rd << ", raddr=" << raddr << "\n";
                            std::cout << "+> value_of_addr=" << regs[raddr] << ", value_at_addr=" << mem[regs[raddr]] << "\n";
                        }
                        break;
                    }

                    case 0x2: { // storen
                        uint32_t raddr = instr >> 16 & 0xFF; // 8 bits
                        uint32_t rs = instr >> 8 & 0xFF; // 8 bits
                        mem[regs[raddr]] = regs[rs];

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "storen: raddr=" << raddr << ", rs=" << rs << "\n";
                            std::cout << "+> value_at_rs=" << to_signed(regs[rs]) << "\n";
                        }
                        break;
                    }

                    case 0x3: { // stored 0000_0011_16-bits-addr_8-bits-rs
                        uint32_t addr = instr >> 8 & 0xFFFF; // 16 bits
                        uint32_t rs = instr & 0xFF; // 8 bits
                        mem[addr] = regs[rs];

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "stored: addr=" << addr << ", rs=" << rs << "\n";
                            std::cout << "+> value_at_rs=" << to_signed(regs[rs]) << "\n";
                        }
                        break;
                    }

                    case 0x4: { // load direct 0000_0100_8-bits-rd_16-bits-addr
                        uint32_t rd = instr >> 16 & 0xFF; // 8 bits
                        uint32_t addr = instr & 0xFFFF; // 16 bits
                        regs[rd] = mem[addr];

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "loadd: rd=" << rd << ", raddr=" << addr << "\n";
                            std::cout << "+> value_at_addr=" << mem[addr] << "\n";
                        }
                        break;
                    }

                    default: {
                        std::cerr << "Error: invalid instruction: 0x" << get_instr_as_hex(instr) << "\n";
                        return;
                    }
                }
                break;
            }

            case 0x1: { // arithmetic
                switch (opcode_id) {
                    case 0x0: { // add
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        uint32_t val1 = regs[rs1];
                        uint32_t val2 = regs[rs2];
                        uint32_t res = val1 + val2;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "add: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x1: { // subtract
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        int32_t val1 = regs[rs1];
                        int32_t val2 = regs[rs2];
                        int32_t res = val1 - val2;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "sub: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x2: { // mul
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        uint32_t val1 = regs[rs1];
                        uint32_t val2 = regs[rs2];
                        uint32_t res = val1 * val2;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "mul: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x3: { // div
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        uint32_t val1 = regs[rs1];
                        uint32_t val2 = regs[rs2];
                        uint32_t res = val1 / val2;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "div: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    default: {
                        std::cerr << "Error: invalid instruction: 0x" << get_instr_as_hex(instr) << "\n";
                        return;
                    }
                    
                }
                break;
            }

            case 0x2: { // control
                switch (opcode_id) {
                    case 0x0: { // jump unconditionally immediate // TODO not tested
                        int32_t val = instr & 0xFFFFFF; // 24 bits address offset

                        // sign extend
                        if ((val >> 15 & 0b1) == 1) { // if negative
                            val = sign_extend(val, 16);
                        }

                        pc += val;
                        skip_auto_pc_incr = true;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "jump: val=" << val << "\n";
                        }
                        break;
                    }

                    case 0x1: { // jump unconditionally direct // TODO not tested
                        uint32_t rs = instr >> 16 & 0xFF;

                        pc += (int32_t) regs[rs];
                        skip_auto_pc_incr = true;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "jumpdir: rs=" << rs << "\n";
                            std::cout << "+> value_at_rs=" << to_signed(regs[rs]) << "\n";
                        }
                        break;
                    }

                    case 0x2: { // jump if immediate 0010_0010_16-bits-val_8-bits-rcond
                        int32_t val = instr >> 8 & 0xFFFF;
                        uint32_t rcond = instr & 0xFF;

                        // sign extend
                        if ((val >> 15 & 0b1) == 1) { // if negative
                            val = sign_extend(val, 16);
                        }

                        int32_t cond = regs[rcond];
                        bool cond_bool = (cond != 0) ? true : false;

                        if (cond_bool) {
                            pc += val;
                            skip_auto_pc_incr = true;
                        }

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "jumpif: val=" << val << ", rcond=" << rcond << "\n";
                            std::cout << "+> cond=" << cond << "\n";
                        }
                        break;
                    }

                    case 0x3: { // jump if direct 0010_0011_8-bits-rs_8-bits-skip_8-bits-rcond // TODO not tested
                        uint32_t rs = instr >> 16 & 0xFF;
                        uint32_t rcond = instr & 0xFF;

                        int32_t cond = regs[rcond];
                        bool cond_bool = (cond != 0) ? true : false;

                        if (cond_bool) {
                            pc += (int32_t) regs[rs];
                            skip_auto_pc_incr = true;
                        }

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "jumpif: rs=" << rs << ", rcond=" << rcond << "\n";
                            std::cout << "+> value_at_rs=" << to_signed(regs[rs]) << ", cond=" << cond << "\n";
                        }
                        break;
                    }

                    case 0x4: { // return
                        if (ret_stack.empty()) {
                            if (DEBUG_LEVEL >= 10) {
                                std::cout << "ret\n";
                                std::cout << "+> return stack empty, ignoring\n"; 
                            }
                        } else {
                            uint32_t ret_addr = ret_stack.top();
                            pc = ret_addr;
                            skip_auto_pc_incr = true;
                            ret_stack.pop();

                            if (DEBUG_LEVEL >= 10) {
                                std::cout << "ret\n";
                                std::cout << "+> pc = " << ret_addr << "\n";
                            }
                        }
                        break;
                    }

                    case 0x5: { // end
                        std::cout << "End of program\n";

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "\nMemory map after 0x0100\n----------------\n";
                            for (auto it=mem.begin(); it!=mem.end(); ++it) {
                                if (it->first >= 0x100) {
                                    std::cout << "Address: " << it->first << ", Value: " << to_signed(it->second) << "\n";
                                }
                            }
                        }

                        return;
                        // break; // not necessary
                    }

                    case 0x6: { // branch unconditionally immediate
                        int32_t val = instr & 0xFFFFFF; // 24 bits address offset

                        // sign extend
                        if ((val >> 23 & 0b1) == 1) { // if negative
                            val = sign_extend(val, 24);
                        }

                        ret_stack.push(pc + 4);
                        pc += val;
                        skip_auto_pc_incr = true;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "br: val=" << val << "\n";
                        }
                        break;
                    }

                    case 0x7: { // branch if immediate
                        int32_t val = instr >> 8 & 0xFFFF;
                        uint32_t rcond = instr & 0xFF;

                        // sign extend
                        if ((val >> 15 & 0b1) == 1) { // if negative
                            val = sign_extend(val, 16);
                        }

                        int32_t cond = regs[rcond];
                        bool cond_bool = (cond != 0) ? true : false;

                        if (cond_bool) {
                            ret_stack.push(pc + 4);
                            pc += val;
                            skip_auto_pc_incr = true;
                        }

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "brif: val=" << val << ", rcond=" << rcond << "\n";
                            std::cout << "+> cond=" << cond << "\n";
                        }
                        break;
                    }

                    default: {
                        std::cerr << "Error: invalid instruction: 0x" << get_instr_as_hex(instr) << "\n";
                        return;
                    }
                }
                break;
            }

            case 0x3: { // logical
                switch (opcode_id) {
                    case 0x0: { // and
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        uint32_t val1 = regs[rs1];
                        uint32_t val2 = regs[rs2];
                        uint32_t res = val1 & val2;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "and: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x1: { // or
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        uint32_t val1 = regs[rs1];
                        uint32_t val2 = regs[rs2];
                        uint32_t res = val1 | val2;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "or: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x2: { // nand
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        uint32_t val1 = regs[rs1];
                        uint32_t val2 = regs[rs2];
                        uint32_t res = ~(val1 & val2);
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "nand: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x3: { // nor
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        uint32_t val1 = regs[rs1];
                        uint32_t val2 = regs[rs2];
                        uint32_t res = ~(val1 | val2);
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "nor: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x4: { // xor
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        uint32_t val1 = regs[rs1];
                        uint32_t val2 = regs[rs2];
                        uint32_t res = val1 ^ val2;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "xor: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    default: {
                        std::cerr << "Error: invalid instruction: 0x" << get_instr_as_hex(instr) << "\n";
                        return;
                    }
                }
                break;
            }

            case 0x4: { // shift
                switch (opcode_id) {
                    case 0x0: { // lshift
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        uint32_t val1 = regs[rs1];
                        uint32_t val2 = regs[rs2];
                        uint32_t res = val1 << val2;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "lshift: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x1: { // rshift
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        uint32_t val1 = regs[rs1];
                        uint32_t val2 = regs[rs2];
                        uint32_t res = val1 >> val2;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "rshift: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }
                }
                break;
            }

            case 0x5: { // comparison
                switch (opcode_id) {
                    case 0x0: { // less than
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        int32_t val1 = regs[rs1];
                        int32_t val2 = regs[rs2];
                        bool res_bool = val1 < val2;
                        int32_t res = res_bool ? 1 : 0;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "lt: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x1: { // less than or equal to
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        int32_t val1 = regs[rs1];
                        int32_t val2 = regs[rs2];
                        bool res_bool = val1 <= val2;
                        int32_t res = res_bool ? 1 : 0;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "lte: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x2: { // greater than
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        int32_t val1 = regs[rs1];
                        int32_t val2 = regs[rs2];
                        bool res_bool = val1 > val2;
                        int32_t res = res_bool ? 1 : 0;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "gt: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x3: { // greater than or equal to
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        int32_t val1 = regs[rs1];
                        int32_t val2 = regs[rs2];
                        bool res_bool = val1 >= val2;
                        int32_t res = res_bool ? 1 : 0;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "gte: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }

                    case 0x4: { // equal to
                        uint32_t rd = instr >> 16 & 0xFF;
                        uint32_t rs1 = instr >> 8 & 0xFF;
                        uint32_t rs2 = instr & 0xFF;

                        int32_t val1 = regs[rs1];
                        int32_t val2 = regs[rs2];
                        bool res_bool = val1 == val2;
                        int32_t res = res_bool ? 1 : 0;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "eq: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << to_signed(val1) << ", val2=" << to_signed(val2) << ", res=" << to_signed(res) << "\n";
                        }
                        break;
                    }
                }
                break;            
            }

            default: {
                std::cerr << "Error: invalid instruction: 0x" << get_instr_as_hex(instr) << "\n";
                return;
            }
        }

        uint32_t pc_debug = pc;
        if (!skip_auto_pc_incr) {
            pc += 4;
        }
        skip_auto_pc_incr = false;

        if (DEBUG_LEVEL >= 11) {
            std::cout << "########\n";
            std::cout << "PC: " << pc_debug << ", Instruction Count: " << read_instr_count << "\n";
            std::cout << "First 8 registers:\n";
            std::cout << "[0]: " << to_signed(regs[0]) << "\n";
            std::cout << "[1]: " << to_signed(regs[1]) << "\n";
            std::cout << "[2]: " << to_signed(regs[2]) << "\n";
            std::cout << "[3]: " << to_signed(regs[3]) << "\n";
            std::cout << "[4]: " << to_signed(regs[4]) << "\n";
            std::cout << "[5]: " << to_signed(regs[5]) << "\n";
            std::cout << "[6]: " << to_signed(regs[6]) << "\n";
            std::cout << "[7]: " << to_signed(regs[7]) << "\n";
            std::cout << "########\n";
        }
        
    }

    std::cout << "Finished running program\n";

    if (DEBUG_LEVEL >= 10) {
        std::cout << "\nMemory map after 0x0100\n----------------\n";
        for (auto it=mem.begin(); it!=mem.end(); ++it) {
            if (it->first >= 0x100) {
                std::cout << "Address: " << it->first << ", Value: " << to_signed(it->second) << "\n";
            }
        }
    }
}

void Yuemu::read_file_to_memory(std::string fpath) {
    std::cout << "Reading program: " << fpath << "\n";
    std::ifstream bin_file(fpath);

    for (int32_t i=0; !bin_file.eof(); i+=4) {
        char cb3, cb2, cb1, cb0;
        bin_file.get(cb3);
        bin_file.get(cb2);
        bin_file.get(cb1);
        bin_file.get(cb0);
        unsigned char b3, b2, b1, b0;
        b3 = (unsigned char) cb3;
        b2 = (unsigned char) cb2;
        b1 = (unsigned char) cb1;
        b0 = (unsigned char) cb0;

        uint32_t instr = 0;
        instr += b3 << 24;
        instr += b2 << 16;
        instr += b1 << 8;
        instr += b0;

        mem[read_instr_count] = instr;
        read_instr_count += 4;

        if (DEBUG_LEVEL >= 10) {
            std::cout << "Read instruction: " << get_instr_as_hex(instr) << "\n";
        }
    }
    std::cout << "Finished reading program\n\n";

    read_instr_count -= 4;
    bin_file.close();
}

std::string Yuemu::get_instr_as_hex(uint32_t instr_int) {
    unsigned char instr_bytes[4];
    instr_bytes[0] = (instr_int) & 0xFF;
    instr_bytes[1] = (instr_int >> 8) & 0xFF;
    instr_bytes[2] = (instr_int >> 16) & 0xFF;
    instr_bytes[3] = (instr_int >> 24) & 0xFF;

    std::stringstream ss;

    ss << "0x" << std::uppercase << std::hex
    << std::setw(2) << std::setfill('0') << (int32_t)instr_bytes[3] 
    << std::setw(2) << std::setfill('0') << (int32_t)instr_bytes[2]
    << std::setw(2) << std::setfill('0') << (int32_t)instr_bytes[1]
    << std::setw(2) << std::setfill('0') << (int32_t)instr_bytes[0];

    return ss.str();
}

uint32_t Yuemu::sign_extend(uint32_t val, uint32_t no_of_bits) {
    uint32_t sign_bit = val >> (no_of_bits - 1) & 0b1;
    if (sign_bit == 1) {
        if (no_of_bits == 16) {
            val += 0xFFFF0000;
        } else if (no_of_bits == 24) {
            val += 0xFF000000;
        } else {
            std::cout << "TODO implement sign extension for all bit lengths\n";
        }
    }
    // no need to sign extend positive values
    return val; 
}

int32_t Yuemu::to_signed(uint32_t val) {
    return (int32_t) val;
}
