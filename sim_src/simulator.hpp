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

    //---- Maps ----
    std::map<int, R_FUNC> R_MAP = {
        {32, &Simulator::add}
    };
    std::map<int, I_FUNC> I_MAP = {

    };
    std::map<int, J_FUNC> J_MAP = {

    };

    //---- Instructions ----
    void add(char rs, char rt, char rd, char sa);
};

#endif //SIMULATOR_HPP