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

void Simulator::branchExecute() {
    uint32_t instr = memory.read(pc, 4);
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

int32_t Simulator::sgnExt16(int32_t val) {
    if ((val >> 15) & 1) {
        val = val | 0xFFFF0000;
    } else {
        val = val & 0xFFFF;
    }
    return val;
}

int32_t Simulator::sgnExt8(int32_t val) {
    if ((val >> 7) & 1) {
        val = val | 0xFFFFFF00;
    } else {
        val = val & 0xFF;
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
    if (registers[rs] == registers[rt]) {
        branchExecute();
        pc += imm;
    }
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
    if (registers[rs] >= 0) {
        branchExecute();
        pc += imm;
    }
}

void Simulator::bgezal(char rs, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] >= 0) {
        registers.write(31, pc + 4);  
        branchExecute();
        pc += imm;
    }
}

void Simulator::bgtz(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] > 0) {
        branchExecute();
        pc += imm;
    }
}

void Simulator::blez(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] <= 0){
        branchExecute();
        pc += imm;
    }
}

void Simulator::bltz(char rs, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] < 0){
        branchExecute();
        pc += imm;
    } 
}

void Simulator::bltzal(char rs, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] < 0){
        registers.write(31, pc + 4);
        branchExecute();
        pc += imm;
    } 
}

void Simulator::bne(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm) << 2;
    if (registers[rs] != registers[rt]){
        branchExecute();
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
    branchExecute();
    pc = addr;
}

void Simulator::jalr(char rs, char rt, char rd, char sa) {
    registers.write(31, pc + 4);
    branchExecute();
    pc = registers[rs];
}

void Simulator::jal(int addr) {
    addr = (addr << 2) | (pc & 0xFF000000);
    registers.write(31, pc + 4);
    branchExecute();
    pc = addr;
}

void Simulator::jr(char rs, char rt, char rd, char sa) {
    branchExecute();
    pc = registers[rs];
}

void Simulator::lb(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm);
    int32_t val = memory.read(imm + registers[rs], 1);
    registers.write(rt, sgnExt8(val));
}

void Simulator::lbu(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm);
    int32_t val = memory.read(imm + registers[rs], 1) & 0xFF;
    registers.write(rt, val);
}

void Simulator::lh(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm);
    int32_t val = memory.read(imm + registers[rs], 2);
    registers.write(rt, sgnExt16(val));
}

void Simulator::lhu(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm);
    int32_t val = memory.read(imm + registers[rs], 2) & 0xFFFF;
    registers.write(rt, val);
}

void Simulator::lw(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm);
    int32_t val = memory.read(imm + registers[rs], 4);
    registers.write(rt, val);
}

void Simulator::lwl(char rs, char rt, int32_t imm) {
    uint32_t addr = registers[rs] + imm;
    char count = 4 - (addr % 4);
    int32_t regVal = 0;
    for(int i = 0; i < count; ++i) {
        uint32_t val = memory.read(addr + i, 1);
        val = val << ((3-i)*8);
        regVal = regVal | val;
    }
    registers.write(rt, regVal);
}

void Simulator::lwr(char rs, char rt, int32_t imm) {
    uint32_t addr = registers[rs] + imm;
    char count = addr % 4 + 1;
    int32_t regVal = 0;
    for(int i = 0; i < count; ++i) {
        uint32_t val = memory.read(addr - i, 1);
        val = val << (i*8);
        regVal = regVal | val;
    }
    registers.write(rt, regVal);
}

void Simulator::mfhi(char rs, char rt, char rd, char sa) {
    registers.write(rd, HI);
}

void Simulator::mflo(char rs, char rt, char rd, char sa) {
    registers.write(rd, LO);
}

void Simulator::mthi(char rs, char rt, char rd, char sa) {
    HI = registers[rs];
}

void Simulator::mtlo(char rs, char rt, char rd, char sa) {
    LO = registers[rs];
}

void Simulator::mult(char rs, char rt, char rd, char sa) {
    int64_t val = registers[rs] * registers[rt];
    LO = val & 0xFFFFFFF;
    HI = (val >> 32) & 0xFFFFFFFF;
}

void Simulator::multu(char rs, char rt, char rd, char sa) {
    uint64_t val = (uint32_t)registers[rs] * (uint32_t)registers[rt];
    LO = val & 0xFFFFFFF;
    HI = (val >> 32) & 0xFFFFFFFF;
}

void Simulator::or_instr(char rs, char rt, char rd, char sa) {
    registers.write(rd, registers[rs] | registers[rt]);
}

void Simulator::ori(char rs, char rt, int32_t imm) {
    registers.write(rt, registers[rs] | (imm & 0xFFFF));
}

void Simulator::sb(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm);
    memory.write(imm + registers[rs], registers[rt], 1);
}

void Simulator::sh(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm);
    memory.write(imm + registers[rs], registers[rt], 2);
}

void Simulator::sll(char rs, char rt, char rd, char sa) {
    registers.write(rd, (registers[rt] << (sa & 0x1F)));
}

void Simulator::sllv(char rs, char rt, char rd, char sa) {
    registers.write(rd, (registers[rt] << (registers[rs] & 0x1F)));
}

void Simulator::slt(char rs, char rt, char rd, char sa) {
    if (registers[rs] < registers[rt])
        registers.write(rd, 1);
    else
        registers.write(rd, 0);
}

void Simulator::sltu(char rs, char rt, char rd, char sa) {
    if ((uint32_t)registers[rs] < (uint32_t)registers[rt])
        registers.write(rd, 1);
    else
        registers.write(rd, 0);
}

void Simulator::slti(char rs, char rt, int32_t imm) {
    if (registers[rs] < sgnExt16(imm))
        registers.write(rt, 1);
    else
        registers.write(rt, 0);
}

void Simulator::sltiu(char rs, char rt, int32_t imm) {
    if ((uint32_t)registers[rs] < (uint32_t)(imm & 0xFFFF))
        registers.write(rt, 1);
    else
        registers.write(rt, 0);
}

void Simulator::sra(char rs, char rt, char rd, char sa) {
    int32_t mask = 0xFFFFFFFF;
    if (sgn(registers[rt]) == -1) {
        registers.write(rd, (registers[rt] >> sa) | (mask << (32 - sa)));
    } else {
        registers.write(rd, (registers[rt] >> sa));
    }
}

void Simulator::srav(char rs, char rt, char rd, char sa) {
    int32_t mask = 0xFFFFFFFF;
    if (sgn(registers[rt]) == -1) {
        registers.write(rd, (registers[rt] >> (registers[rs] & 0x1F)) | (mask << (32 - (registers[rs] & 0x1F))));
    } else {
        registers.write(rd, (registers[rt] >> (registers[rs] & 0x1F)));
    }
}

void Simulator::srl(char rs, char rt, char rd, char sa) {
    registers.write(rd, ((uint32_t)registers[rt] >> sa));
}

void Simulator::srlv(char rs, char rt, char rd, char sa) {
    registers.write(rd, ((uint32_t)registers[rt] >> (registers[rs] & 0x1F)));
}

void Simulator::sub(char rs, char rt, char rd, char sa) {
    int32_t in1 = registers[rs];
    int32_t in2 = registers[rt];
    int32_t out = in1 - in2;
    if (sgn(in1) != sgn(in2) && sgn(in2) == sgn(out)) {
        std::cerr << "Overflow detected in 'add'." << std::endl << std::endl;
        std::exit(-10);
    }
    registers.write(rd, out);
}

void Simulator::subu(char rs, char rt, char rd, char sa) {
    uint32_t in1 = registers[rs];
    uint32_t in2 = registers[rt];
    uint32_t out = in1 - in2;
    registers.write(rd, out);
}

void Simulator::sw(char rs, char rt, int32_t imm) {
    imm = sgnExt16(imm);
    memory.write(imm + registers[rs], registers[rt], 4);
}

void Simulator::xor_instr(char rs, char rt, char rd, char sa) {
    registers.write(rd, (registers[rs] ^ registers[rt]));
}

void Simulator::xori(char rs, char rt, int32_t imm) {
    registers.write(rt, (registers[rs] ^ (imm & 0xFFFF)));
}