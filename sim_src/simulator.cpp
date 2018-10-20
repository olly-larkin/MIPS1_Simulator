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

int32_t Simulator::sgnExt16(int32_t val){
    if ((val >> 15) & 1) {
        val = val | 0xFFFF0000;
    } else {
        val = val & 0xFFFF;
    }
    return val;
}

// ****************** INSTRUCTIONS ******************

void Simulator::add(char rs, char rt, char rd, char sa) {
    int32_t in1 = registers[rs];
    int32_t in2 = registers[rt];
    int32_t out = in1 + in2;
    if (sgn(in1) == sgn(in2) && sgn(in1) != sgn(out)) {
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
    registers.write(rt, imm + registers[rs]);
}

void Simulator::addu(char rs, char rt, char rd, char sa) {
    registers.write(rd, registers[rs] + registers[rt]);
}

void Simulator::and_instr(char rs, char rt, char rd, char sa) {
    registers.write(rd, registers[rs] & registers[rt]);
}

void Simulator::andi(char rs, char rt, int32_t imm) {
    registers.write(rt, registers[rs] & (imm & 0xFFFF));
}

void Simulator::beq(char rs, char rt, int32_t imm) {
    if ((imm >> 15) & 1) {
        imm = (imm << 2) | 0xFFFC0000;
    } else {
        imm = (imm << 2) & 0x3FFFF;
    }
    if (registers[rs] == registers[rt])
        pc += imm;
}

void Simulator::branches(char rs, char rt, int32_t imm) {
    switch(rt) {
        case 1: {
            bgez(rs, imm);
            break;
        }
        case 17: {
            bgezal(rs, imm);
            break;
        }
        case 0: {
            bltz(rs, imm);
            break;
        }
        case 16: {
            bltzal(rs, imm);
            break;
        }
        default: {
            std::cerr << "Invalid branch instruction." << std::endl;
            std::exit(-12);
        }
    }
}

void Simulator::bgez(char rs, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] >= 0)
        pc += imm;
}

void Simulator::bgezal(char rs, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] >= 0) {
        registers.write(31, pc);    // Maybe +4? Ask about branch delay slots
        pc += imm;
    }
}

void Simulator::bgtz(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] > 0)
        pc += imm;
}

void Simulator::blez(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] <= 0){
        pc += imm;
    }

}

void Simulator::bltz(char rs, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] < 0){
        pc += imm;
    } 
}

void Simulator::bltzal(char rs, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] < 0){
        registers.write(31, pc);
        pc += imm;
    } 
}

void Simulator::bne(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] != registers[rt]){
        pc += imm;
    }
}

void Simulator::div_instr(char rs, char rt, char rd, char sa) {
    if(registers[rt] == 0){
        std::cerr << "Attempted to divide by 0." << std::endl << std::endl;
        std::exit(-10);
    }
    LO = registers[rs] / registers[rt];
    HI = registers[rs] % registers[rt];
    
}

void Simulator::divu(char rs, char rt, char rd, char sa){
    if(registers[rt] == 0){
        std::cerr << "Attempted to divide by 0." << std::endl << std::endl;
        std::exit(-10);
    }
    LO = (uint32_t)registers[rs] / (uint32_t)registers[rt];
    HI = (uint32_t)registers[rs] % (uint32_t)registers[rt];
}

void Simulator::j(int addr) {
    addr = (addr << 2) | (pc & 0xFF000000);
    pc = addr;
}

void Simulator::jalr(char rs, char rt, char rd, char sa) {
    registers.write(31, pc);
    pc = registers[rs];
}

void Simulator::jal(int addr) {
    addr = (addr << 2) | (pc & 0xFF000000);
    registers.write(31, pc);
    pc = addr;
}

void Simulator::jr(char rs, char rt, char rd, char sa) {
    pc = registers[rs];
}

void Simulator::sll(char rs, char rt, char rd, char sa) {
    registers.write(rd, (registers[rt] << sa));
}
