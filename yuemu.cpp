#include <iostream>
#include <fstream>
#include <iomanip>

#include "yuemu.hpp"

Yuemu::Yuemu(std::string fpath) {
    read_file_to_memory(fpath);
    run();
}

void Yuemu::run() {
    std::cout << "Running program\n";
    while (pc != read_instr_count) {
        unsigned int instr = mem[pc]; // fetch
        unsigned int opcode_category = instr >> 28 & 0xF;
        unsigned int opcode_id = instr >> 24 & 0xF;

        switch (opcode_category) {
            case 0x00: { // load store
                switch (opcode_id) {
                    case 0x00: { // load immediate
                        unsigned int rd = instr >> 16 & 0xFF; // 8 bits
                        unsigned int val = instr & 0xFFFF; // 16 bits
                        regs[rd] = val;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "loadimm: rd=" << rd << ", val=" << val << "\n";
                        }
                        break;
                    }

                    case 0x01: { // load register indirect
                        unsigned int rd = instr >> 16 & 0xFF; // 8 bits
                        unsigned int raddr = instr >> 8 & 0xFF; // 8 bits
                        regs[rd] = mem[regs[raddr]];

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "loadrin: rd=" << rd << ", raddr=" << raddr << "\n";
                            std::cout << "+> value_of_addr=" << regs[raddr] << ", value_at_addr=" << mem[regs[raddr]] << "\n";
                        }
                        break;
                    }

                    case 0x02: { // store
                        unsigned int raddr = instr >> 16 & 0xFF; // 8 bits
                        unsigned int rs = instr >> 8 & 0xFF; // 8 bits
                        mem[regs[raddr]] = regs[rs];

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "store: raddr=" << raddr << ", rs=" << rs << "\n";
                            std::cout << "+> value_at_rs=" << regs[rs] << "\n";
                        }
                        break;
                    }
                }
                break;
            }

            case 0x01: { // arithmetic
                switch (opcode_id) {
                    case 0x00: { // add
                        unsigned int rd = instr >> 16 & 0xFF;
                        unsigned int rs1 = instr >> 8 & 0xFF;
                        unsigned int rs2 = instr & 0xFF;

                        int val1 = regs[rs1];
                        int val2 = regs[rs2];
                        int res = val1 + val2;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "add: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << val1 << ", val2=" << val2 << ", res=" << res << "\n";
                        }
                        break;
                    }

                    case 0x01: { // subtract
                        unsigned int rd = instr >> 16 & 0xFF;
                        unsigned int rs1 = instr >> 8 & 0xFF;
                        unsigned int rs2 = instr & 0xFF;

                        int val1 = regs[rs1];
                        int val2 = regs[rs2];
                        int res = val1 - val2;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "sub: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << val1 << ", val2=" << val2 << ", res=" << res << "\n";
                        }
                        break;
                    }

                    case 0x02: { // less than
                        unsigned int rd = instr >> 16 & 0xFF;
                        unsigned int rs1 = instr >> 8 & 0xFF;
                        unsigned int rs2 = instr & 0xFF;

                        int val1 = regs[rs1];
                        int val2 = regs[rs2];
                        bool res_bool = val1 < val2;
                        int res = res_bool ? 1 : 0;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "lt: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << val1 << ", val2=" << val2 << ", res=" << res << "\n";
                        }
                        break;
                    }

                    case 0x03: { // less than or equal to
                        unsigned int rd = instr >> 16 & 0xFF;
                        unsigned int rs1 = instr >> 8 & 0xFF;
                        unsigned int rs2 = instr & 0xFF;

                        int val1 = regs[rs1];
                        int val2 = regs[rs2];
                        bool res_bool = val1 <= val2;
                        int res = res_bool ? 1 : 0;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "lte: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << val1 << ", val2=" << val2 << ", res=" << res << "\n";
                        }
                        break;
                    }

                    case 0x04: { // greater than
                        unsigned int rd = instr >> 16 & 0xFF;
                        unsigned int rs1 = instr >> 8 & 0xFF;
                        unsigned int rs2 = instr & 0xFF;

                        int val1 = regs[rs1];
                        int val2 = regs[rs2];
                        bool res_bool = val1 > val2;
                        int res = res_bool ? 1 : 0;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "gt: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << val1 << ", val2=" << val2 << ", res=" << res << "\n";
                        }
                        break;
                    }

                    case 0x05: { // greater than or equal to
                        unsigned int rd = instr >> 16 & 0xFF;
                        unsigned int rs1 = instr >> 8 & 0xFF;
                        unsigned int rs2 = instr & 0xFF;

                        int val1 = regs[rs1];
                        int val2 = regs[rs2];
                        bool res_bool = val1 >= val2;
                        int res = res_bool ? 1 : 0;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "gte: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << val1 << ", val2=" << val2 << ", res=" << res << "\n";
                        }
                        break;
                    }

                    case 0x06: { // equal to
                        unsigned int rd = instr >> 16 & 0xFF;
                        unsigned int rs1 = instr >> 8 & 0xFF;
                        unsigned int rs2 = instr & 0xFF;

                        int val1 = regs[rs1];
                        int val2 = regs[rs2];
                        bool res_bool = val1 == val2;
                        int res = res_bool ? 1 : 0;
                        regs[rd] = res;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "eq: rd=" << rd << ", rs1=" << rs1 << ", rs2=" << rs2 << "\n";
                            std::cout << "+> val1=" << val1 << ", val2=" << val2 << ", res=" << res << "\n";
                        }
                        break;
                    }
                    
                }
                break;
            }

            case 0x02: { // control
                switch (opcode_id) {
                    case 0x00: { // jump unconditionally immediate
                        unsigned int val = instr & 0xFFFFF; // 24 bits address value

                        pc = val;
                        skip_auto_pc_incr = true;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "jump: val=" << val << "\n";
                        }
                        break;
                    }

                    case 0x01: { // jump unconditionally direct
                        unsigned int rs = instr >> 16 & 0xFF;

                        pc = regs[rs];
                        skip_auto_pc_incr = true;

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "jumpdir: rs=" << rs << "\n";
                            std::cout << "+> value_at_rs=" << regs[rs] << "\n";
                        }
                        break;
                    }

                    case 0x02: { // jump if immediate 0010_0010_16-bits-val_8-bits-rcond
                        unsigned int val = instr >> 8 & 0xFFFF;
                        unsigned int rcond = instr & 0xFF;

                        int cond = regs[rcond];
                        bool cond_bool = (cond != 0) ? true : false;

                        if (cond_bool) {
                            pc = val;
                            skip_auto_pc_incr = true;
                        }

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "jumpif: val=" << val << ", rcond=" << rcond << "\n";
                            std::cout << "+> cond=" << cond << "\n";
                        }
                        break;
                    }

                    case 0x03: { // jump if direct 0010_0011_8-bits-rs_8-bits-skip_8-bits-rcond
                        unsigned int rs = instr >> 16 & 0xFF;
                        unsigned int rcond = instr & 0xFF;

                        int cond = regs[rcond];
                        bool cond_bool = (cond != 0) ? true : false;

                        if (cond_bool) {
                            pc = regs[rs];
                            skip_auto_pc_incr = true;
                        }

                        if (DEBUG_LEVEL >= 10) {
                            std::cout << "jumpif: rs=" << rs << ", rcond=" << rcond << "\n";
                            std::cout << "+> value_at_rs=" << regs[rs] << ", cond=" << cond << "\n";
                        }
                        break;
                    }

                    case 0x04: { // end
                        std::cout << "End of execution\n";
                        return;
                        // break; // not necessary
                    }
                }
                break;
            }
        }

        unsigned int pc_debug = pc;
        if (!skip_auto_pc_incr) {
            pc += 4;
        }
        skip_auto_pc_incr = false;

        if (DEBUG_LEVEL >= 11) {
            std::cout << "########\n";
            std::cout << "PC: " << pc_debug << ", Instruction Count: " << read_instr_count << "\n";
            std::cout << "First 8 registers:\n";
            std::cout << "[0]: " << regs[0] << "\n";
            std::cout << "[1]: " << regs[1] << "\n";
            std::cout << "[2]: " << regs[2] << "\n";
            std::cout << "[3]: " << regs[3] << "\n";
            std::cout << "[4]: " << regs[4] << "\n";
            std::cout << "[5]: " << regs[5] << "\n";
            std::cout << "[6]: " << regs[6] << "\n";
            std::cout << "[7]: " << regs[7] << "\n";
            std::cout << "########\n";
        }
        
    }

    std::cout << "Finished running program\n";

    if (DEBUG_LEVEL >= 10) {
        std::cout << "\n\nMemory map after 0x0100\n----------------\n";
        for (auto it=mem.begin(); it!=mem.end(); ++it) {
            if (it->first >= 0x100) {
                std::cout << "Address: " << it->first << ", Value: " << it->second << "\n";
            }
        }
    }
}

void Yuemu::read_file_to_memory(std::string fpath) {
    std::cout << "Reading program: " << fpath << "\n";
    std::ifstream bin_file(fpath);

    for (int i=0; !bin_file.eof(); i+=4) {
        char b3, b2, b1, b0;
        bin_file.get(b3);
        bin_file.get(b2);
        bin_file.get(b1);
        bin_file.get(b0);

        unsigned int instr = 0;
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

std::string Yuemu::get_instr_as_hex(unsigned int instr_int) {
    unsigned char instr_bytes[4];
    instr_bytes[0] = (instr_int) & 0xFF;
    instr_bytes[1] = (instr_int >> 8) & 0xFF;
    instr_bytes[2] = (instr_int >> 16) & 0xFF;
    instr_bytes[3] = (instr_int >> 24) & 0xFF;

    std::stringstream ss;

    ss << "0x" << std::uppercase << std::hex
    << std::setw(2) << std::setfill('0') << (int)instr_bytes[3] 
    << std::setw(2) << std::setfill('0') << (int)instr_bytes[2]
    << std::setw(2) << std::setfill('0') << (int)instr_bytes[1]
    << std::setw(2) << std::setfill('0') << (int)instr_bytes[0];

    return ss.str();
}
