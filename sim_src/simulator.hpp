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
    Simulator(): HI(0), LO(0), pc(ADDR_INSTR_P) {};
    Simulator(std::ifstream& binFile);

    typedef void (Simulator::*R_FUNC)(char rs, char rt, char rd, char sa);
    typedef void (Simulator::*I_FUNC)(char rs, char rt, int32_t imm);
    typedef void (Simulator::*J_FUNC)(int addr);
private:
    MemoryMap memory;
    Registers registers;
    unsigned int pc;
    int32_t HI;
    int32_t LO;
    OP_TYPE opcode(uint32_t instr);
    void executeR(uint32_t instr);
    void executeI(uint32_t instr);
    void executeJ(uint32_t instr);
    char sgn(int num);

    //---- Maps ----
    std::map<char, R_FUNC> R_MAP = {
        {32, &Simulator::add},
        {33, &Simulator::addu},
        {36, &Simulator::and_instr},

        {8, &Simulator::jr},
        {0, &Simulator::sll}
    };
    std::map<char, I_FUNC> I_MAP = {
        {8, &Simulator::addi},
        {9, &Simulator::addiu},
    };
    std::map<char, J_FUNC> J_MAP = {

    };

    //---- Instructions ----
    void add(char rs, char rt, char rd, char sa);
    void addi(char rs, char rt, int32_t imm);
    void addiu(char rs, char rt, int32_t imm);
    void addu(char rs, char rt, char rd, char sa);
    void and_instr(char rs, char rt, char rd, char sa);

    void jr(char rs, char rt, char rd, char sa);
    void sll(char rs, char rt, char rd, char sa);
};

#endif //SIMULATOR_HPP