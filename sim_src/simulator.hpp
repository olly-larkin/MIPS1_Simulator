#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <iostream>
#include "memoryMap.hpp"
#include "registers.hpp"
#include <fstream>
#include <map>

enum OP_TYPE {R, I, J};

class Simulator {
public:
    void loadInstr(std::ifstream& binFile);
    char execute();
    Simulator();
    Simulator(std::ifstream& binFile);

    typedef void (Simulator::*R_FUNC)(char rs, char rt, char rd, char sa);
    typedef void (Simulator::*I_FUNC)(char rs, char rt, int32_t imm);
    typedef void (Simulator::*J_FUNC)(int addr);
private:
    MemoryMap memory;
    Registers registers;
    Registers regBuff1;
    Registers regBuff2;
    unsigned int pc = ADDR_INSTR_P;
    int32_t HI = 0;
    int32_t LO = 0;
    OP_TYPE opcode(uint32_t instr);
    void branchExecute();
    void executeR(uint32_t instr);
    void executeI(uint32_t instr);
    void executeJ(uint32_t instr);
    char sgn(int num);
    int32_t sgnExt16(int32_t val);
    int32_t sgnExt8(int32_t val);
    void regWrite(char addr, int32_t data);

    //---- Maps ----
    std::map<char, R_FUNC> R_MAP = {
        {32, &Simulator::add},
        {33, &Simulator::addu},
        {36, &Simulator::and_instr},
        {26, &Simulator::div_instr},
        {27, &Simulator::divu},
        {9, &Simulator::jalr},
        {8, &Simulator::jr},
        {16, &Simulator::mfhi},
        {18, &Simulator::mflo},
        {17, &Simulator::mthi},
        {19, &Simulator::mtlo},
        {24, &Simulator::mult},
        {25, &Simulator::multu},
        {37, &Simulator::or_instr},
        {0, &Simulator::sll},
        {4, &Simulator::sllv},
        {42, &Simulator::slt},
        {43, &Simulator::sltu},
        {3, &Simulator::sra},
        {7, &Simulator::srav},
        {2, &Simulator::srl},
        {6, &Simulator::srlv},
        {34, &Simulator::sub},
        {35, &Simulator::subu},
        {38, &Simulator::xor_instr}
    };
    std::map<char, I_FUNC> I_MAP = {
        {8, &Simulator::addi},
        {9, &Simulator::addiu},
        {12, &Simulator::andi},
        {4, &Simulator::beq},
        {1, &Simulator::branches},
        {7, &Simulator::bgtz},
        {6, &Simulator::blez},
        {5, &Simulator::bne},
        {32, &Simulator::lb},
        {36, &Simulator::lbu},
        {33, &Simulator::lh},
        {37, &Simulator::lhu},
        {15, &Simulator::lui},
        {35, &Simulator::lw},
        {34, &Simulator::lwl},
        {38, &Simulator::lwr},
        {13, &Simulator::ori},
        {40, &Simulator::sb},
        {41, &Simulator::sh},
        {10, &Simulator::slti},
        {11, &Simulator::sltiu},
        {43, &Simulator::sw},
        {14, &Simulator::xori}
    };
    std::map<char, J_FUNC> J_MAP = {
        {2, &Simulator::j},
        {3, &Simulator::jal}
    };

    //---- Instructions ----
    void add(char rs, char rt, char rd, char sa);
    void addi(char rs, char rt, int32_t imm);
    void addiu(char rs, char rt, int32_t imm);
    void addu(char rs, char rt, char rd, char sa);
    void and_instr(char rs, char rt, char rd, char sa);
    void andi(char rs, char rt, int32_t imm);
    void beq(char rs, char rt, int32_t imm);
    void branches(char rs, char rt, int32_t imm);
    void bgez(char rs, int32_t imm);
    void bgezal(char rs, int32_t imm);
    void bgtz(char rs, char rt, int32_t imm);
    void blez(char rs, char rt, int32_t imm);
    void bltz(char rs, int32_t imm);
    void bltzal(char rs, int32_t imm);
    void bne(char rs, char rt, int32_t imm);
    void div_instr(char rs, char rt, char rd, char sa);
    void divu(char rs, char rt, char rd, char sa);
    void j(int addr);
    void jalr(char rs, char rt, char rd, char sa);
    void jal(int addr);
    void jr(char rs, char rt, char rd, char sa);
    void lb(char rs, char rt, int32_t imm);
    void lbu(char rs, char rt, int32_t imm);
    void lh(char rs, char rt, int32_t imm);
    void lhu(char rs, char rt, int32_t imm);
    void lui(char rs, char rt, int32_t imm);
    void lw(char rs, char rt, int32_t imm);
    void lwl(char rs, char rt, int32_t imm);
    void lwr(char rs, char rt, int32_t imm);
    void mfhi(char rs, char rt, char rd, char sa);
    void mflo(char rs, char rt, char rd, char sa);
    void mthi(char rs, char rt, char rd, char sa);
    void mtlo(char rs, char rt, char rd, char sa);
    void mult(char rs, char rt, char rd, char sa);
    void multu(char rs, char rt, char rd, char sa);
    void or_instr(char rs, char rt, char rd, char sa);
    void ori(char rs, char rt, int32_t imm);
    void sb(char rs, char rt, int32_t imm);
    void sh(char rs, char rt, int32_t imm);
    void sll(char rs, char rt, char rd, char sa);
    void sllv(char rs, char rt, char rd, char sa);
    void slt(char rs, char rt, char rd, char sa);
    void sltu(char rs, char rt, char rd, char sa);
    void slti(char rs, char rt, int32_t imm);
    void sltiu(char rs, char rt, int32_t imm);
    void sra(char rs, char rt, char rd, char sa);
    void srav(char rs, char rt, char rd, char sa);
    void srl(char rs, char rt, char rd, char sa);
    void srlv(char rs, char rt, char rd, char sa);
    void sub(char rs, char rt, char rd, char sa);
    void subu(char rs, char rt, char rd, char sa);
    void sw(char rs, char rt, int32_t imm);
    void xor_instr(char rs, char rt, char rd, char sa);
    void xori(char rs, char rt, int32_t imm);
};

#endif //SIMULATOR_HPP