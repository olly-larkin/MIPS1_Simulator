#include <iostream>
#include "simulator.hpp"
#include <map>

Simulator::Simulator(std::ifstream& binFile): HI(0), LO(0), pc(ADDR_INSTR_P) {
    loadInstr(binFile);
}

void Simulator::loadInstr(std::ifstream& binFile) {
    memory.instrDump(binFile);
}

char Simulator::execute() {
    uint32_t instr = 0;
    do {
        if (pc < ADDR_INSTR_P || pc > ADDR_INSTR_P + ADDR_INSTR_SIZE) {
            std::cerr << "Attempted to execute non-executable memory." << std::endl;
            std::exit(-11);
        }
        instr = memory.read(pc, 4);

        switch(opcode(instr)) {
            case R: {
                executeR(instr);
                break;
            }
            case I: {
                executeI(instr);
                break;
            }
            case J: {
                executeJ(instr);
                break;
            }
        }

        pc += 4;
    } while(instr != 0);

    return registers.exitCode();
}

OP_TYPE Simulator::opcode(uint32_t instr) {
    unsigned char op = (instr >> 26) & 0x3F;
    if (op == 0)
        return R;
    else if (op == 2 || op == 3)
        return J;
    else   
        return I;
}

void Simulator::executeR(uint32_t instr) {
    char rs = (instr >> 21) & 0x1F;
    char rt = (instr >> 16) & 0x1F;
    char rd = (instr >> 11) & 0x1F;
    char sa = (instr >> 6) & 0x1F;
    char fn = instr & 0x3F;

    if (R_MAP.find(fn) == R_MAP.end()) {
        std::cerr << "Invalid instruction." << std::endl;
        std::exit(-12);
    }

    (this->*R_MAP[fn])(rs, rt, rd, sa);
}

void Simulator::executeI(uint32_t instr) {
    char op = (instr >> 26) & 0x3F;
    char rs = (instr >> 21) & 0x1F;
    char rt = (instr >> 16) & 0x1F;
    int16_t imm = instr & 0xFFFF;

    if (I_MAP.find(op) == I_MAP.end()) {
        std::cerr << "Invalid instruction." << std::endl;
        std::exit(-12);
    }

    (this->*I_MAP[op])(rs, rt, imm);
}

void Simulator::executeJ(uint32_t instr) {
    char op = (instr >> 26) & 0x3F;
    int addr = instr & 0x3FFFFFF;

    if (J_MAP.find(op) == J_MAP.end()) {
        std::cerr << "Invalid instruction." << std::endl;
        std::exit(-12);
    }

    (this->*J_MAP[op])(addr);
}

// ****************** INSTRUCTIONS ******************

void Simulator::add(char rs, char rt, char rd, char sa) {
    int32_t in1 = registers.read(rs);
    int32_t in2 = registers.read(rt);
    int32_t out = in1 + in2;
    if ((((in1 >> 31) & 0x1) == ((in2 >> 31) & 0x1)) && (((out >> 31) & 0x1) != ((in1 >> 31) & 0x1))) {
        std::cerr << "Overflow detected." << std::endl;
        std::exit(-10);
    }
    registers.write(rd, out);
}
