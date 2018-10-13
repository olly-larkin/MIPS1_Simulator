#include "MIPS_instructions.hpp"
#include <iostream>
#include <map>

std::map<char, rTypeFunc> R_FUNC = {
    {33, addu}
};
std::map<char, iTypeFunc> I_FUNC = {
    
};
std::map<char, jTypeFunc> J_FUNC = {
    
};

char memBlock[0xFFFFFFFF];
int32_t registers[32];
int32_t programCounter = ADDR_INSTR;

void executeR(char fn, char s1, char s2, char dest, char shAmt){
    if (R_FUNC.find(fn) == R_FUNC.end()) {
        std::cerr << "Invalid instruction.\n";
        std::exit(-12);
    }
    R_FUNC[fn](s1,s2,dest,shAmt);
}

void executeI(char op, char s1, char dest, int16_t data){
    if (I_FUNC.find(op) == I_FUNC.end()) {
        std::cerr << "Invalid instruction.\n";
        std::exit(-12);
    }
    I_FUNC[op](s1,dest,data);
}

void executeJ(char op, int32_t addr){
    if (J_FUNC.find(op) == J_FUNC.end()) {
        std::cerr << "Invalid instruction.\n";
        std::exit(-12);
    }
    J_FUNC[op](addr);
}

char* executeMem() {
    return &memBlock[ADDR_INSTR];
}

int32_t pcLocation() {
    return programCounter;
}

int32_t getNextInstr() {
    int32_t returnVal = get32(memBlock, programCounter);
    programCounter += 4;
    return returnVal;
}

int32_t get32(const char* arr, int addr) {
    return ((unsigned char)arr[addr] << 24) | ((unsigned char)arr[addr + 1] << 16) | ((unsigned char)arr[addr + 2] << 8) | (unsigned char)arr[addr + 3];
}

char returnCode() {
    return (char)(registers[2] & 0xFF);
}

void exitError(std::string msg, int errCode) {
    std::cerr << msg << std::endl << std::endl;
    std::exit(errCode);
}

//************************** MIPS INSTRUCTIONS **************************

void addu(char s1, char s2, char dest, char shAmt) {
    if (shAmt != 0)
        std::cerr << "Shift amount not required for this operation. Value disregarded.\n";
    
    registers[dest] = registers[s1] + registers[s2];
}
 
