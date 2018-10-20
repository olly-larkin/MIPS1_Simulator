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

    while(pc != 0x0) {

        if (pc < ADDR_INSTR_P || pc > ADDR_INSTR_P + ADDR_INSTR_SIZE) {
            std::cerr << "Attempted to execute non-executable memory." << std::endl << std::endl;
            std::exit(-11);
        }
        instr = memory.read(pc, 4);

        pc += 4;

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
    }

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
        std::cerr << std::hex << "Invalid instruction: " << instr << std::endl << std::endl;
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
        std::cerr << "Invalid instruction." << std::endl << std::endl;
        std::exit(-12);
    }

    (this->*I_MAP[op])(rs, rt, imm);
}

void Simulator::executeJ(uint32_t instr) {
    char op = (instr >> 26) & 0x3F;
    int addr = instr & 0x3FFFFFF;

    if (J_MAP.find(op) == J_MAP.end()) {
        std::cerr << "Invalid instruction." << std::endl << std::endl;
        std::exit(-12);
    }

    (this->*J_MAP[op])(addr);
}

char Simulator::sgn(int num) {
    return (num > 0) - (num < 0);
}

// ****************** INSTRUCTIONS ******************

void Simulator::add(char rs, char rt, char rd, char sa) {
    int32_t in1 = registers[rs];
    int32_t in2 = registers[rt];
    int32_t out = in1 + in2;
    if ((((in1 >> 31) & 0x1) == ((in2 >> 31) & 0x1)) && (((out >> 31) & 0x1) != ((in1 >> 31) & 0x1))) {
        std::cerr << "Overflow detected in 'add'." << std::endl << std::endl;
        std::exit(-10);
    }
    registers.write(rd, out);
}

void Simulator::addi(char rs, char rt, int32_t imm) {
    int32_t in1 = registers[rs];
    int32_t out = in1 + imm;
    if (sgn(in1) == sgn(imm) && sgn(in1) != sgn(out)) {
        std::cerr << "Overflow detected in 'addi'." << std::endl << std::endl;
    }
    registers.write(rt, out);
}

void Simulator::addiu(char rs, char rt, int32_t imm) {
    uint32_t in1 = registers[rs];
    uint32_t out = in1 + imm;
    registers.write(rt, out);
}

void Simulator::addu(char rs, char rt, char rd, char sa) {
    
}

void Simulator::jr(char rs, char rt, char rd, char sa) {
    pc = registers[rs];
}

void Simulator::sll(char rs, char rt, char rd, char sa) {
    registers.write(rd, (registers[rt] << sa));
}
