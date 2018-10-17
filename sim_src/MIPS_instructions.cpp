#include "MIPS_instructions.hpp"
#include <iostream>
#include <string>
#include <map>

std::map<char, rTypeFunc> R_FUNC = {
    {16, mfhi},
    {18, mflo},
    {26, div_instr},
    {27, divu},
    {33, addu},
    {36, and_instr}
};
std::map<char, iTypeFunc> I_FUNC = {
    {9, addiu},
    {12, andi},
    {32, lb},
    {33, lh},
    {36, lbu}
};
std::map<char, jTypeFunc> J_FUNC = {
    
};

char ADDR_NULL[0x4];
char ADDR_INSTR[0x10000000];
char ADDR_DATA[0x40000000];
char ADDR_GETC[0x4];
char ADDR_PUTC[0x4];
int32_t registers[32];
int32_t HI = 0, LO = 0;
int32_t programCounter = ADDR_INSTR_P;

void executeR(char fn, char s1, char s2, char dest, char shAmt){
    if (R_FUNC.find(fn) == R_FUNC.end())
        exitError("Invalid instruction.", -12);
    R_FUNC[fn](s1,s2,dest,shAmt);
}

void executeI(char op, char s1, char dest, int16_t data){
    if (I_FUNC.find(op) == I_FUNC.end()) 
        exitError("Invalid instruction.", -12);
    I_FUNC[op](s1,dest,data);
}

void executeJ(char op, int32_t addr){
    if (J_FUNC.find(op) == J_FUNC.end()) 
        exitError("Invalid instruction.", -12);
    J_FUNC[op](addr);
}

char* executeMem() {
    return ADDR_INSTR;
}

int32_t pcLocation() {
    return programCounter;
}

int32_t getNextInstr() {
    int32_t returnVal = get32(memMap(programCounter));
    programCounter += 4;
    return returnVal;
}

int32_t get32(const char* arr) {
    return ((unsigned char)arr[0] << 24) | ((unsigned char)arr[1] << 16) | ((unsigned char)arr[2] << 8) | (unsigned char)arr[3];
}

int32_t get16(const char* arr) {
    return ((unsigned char)arr[0] << 8) | (unsigned char)arr[1];
}

char returnCode() {
    return (char)(registers[2] & 0xFF);
}

void exitError(std::string msg, int errCode) {
    std::cerr << msg << std::endl << std::endl;
    std::exit(errCode);
}

char* memMap(int32_t addr) {
    if (addr < 0x4)
        return &ADDR_NULL[addr];
    else if (addr >= 0x10000000 && addr < 0x11000000)
        return &ADDR_INSTR[addr - 0x10000000];
    else if (addr >= 0x20000000 && addr < 0x24000000)
        return &ADDR_DATA[addr - 0x20000000];
    else if (addr >= 0x30000000 && addr < 0x30000004)
        return &ADDR_GETC[addr - 0x30000000];
    else if (addr >= 0x30000004 && addr < 0x30000008)
        return &ADDR_PUTC[addr - 0x30000004];
    else {
        exitError("Invalid memory access.", -11);
        return ADDR_NULL;
    }
}

void successfulExit() {
    programCounter = 0;
    // maybe execute an instruction here?
    std::cerr << "Successful execution." << std::endl << std::endl;
    std::exit(returnCode());
}

//************************** MIPS INSTRUCTIONS **************************

//----- R TYPE -----

void mfhi(char s1, char s2, char dest, char shAmt) {
    registers[dest] = HI;
}

void mflo(char s1, char s2, char dest, char shAmt) {
    registers[dest] = LO;
}

void div_instr(char s1, char s2, char dest, char shAmt) {
    if (s2 == 0)
        exitError("Cannot divide by 0.", -10);

    LO = s1 / s2;
    HI = s1 % s2;
}

void divu(char s1, char s2, char dest, char shAmt) {
    if (s2 == 0)
        exitError("Cannot divide by 0.", -10);

    LO = (unsigned)s1 / (unsigned)s2;
    HI = (unsigned)s1 % (unsigned)s2;
}

void addu(char s1, char s2, char dest, char shAmt) {
    if (shAmt != 0)
        std::cerr << "Shift amount not required for 'addu' operation. Value disregarded.\n";
    
    registers[dest] = registers[s1] + registers[s2];
}

void and_instr(char s1, char s2, char dest, char shAmt) {
    if (shAmt != 0)
        std::cerr << "Shift amount not required for 'and' operation. Value disregarded.\n";
    
    registers[dest] = registers[s1] & registers[s2];
}

//----- I TYPE -----

void addiu(char s1, char dest, int16_t data) {
    registers[dest] = registers[s1] + data;
}

void andi(char s1, char dest, int16_t data) {
    registers[dest] = registers[s1] & data;
}

void lb(char s1, char dest, int16_t data) {
    if (registers[s1] + data >= ADDR_DATA_P && registers[s1] + data > ADDR_DATA_LIMIT)
        exitError("Invalid memory access.", -11);

    registers[dest] = (int32_t)*memMap(registers[s1] + data);
    if ((registers[dest] & 0x00000080) == 0)
        registers[dest] = registers[dest] & 0x000000FF;
    else
        registers[dest] = registers[dest] | 0xFFFFFF00;
}

void lh(char s1, char dest, int16_t data) {
    if (registers[s1] + data >= ADDR_DATA_P && registers[s1] + data > ADDR_DATA_LIMIT - 1)
        exitError("Invalid memory access.", -11);

    registers[dest] = (int32_t)get16(memMap(registers[s1] + data));
    if ((registers[dest] & 0x00008000) == 0)
        registers[dest] = registers[dest] & 0x0000FFFF;
    else
        registers[dest] = registers[dest] | 0xFFFF0000;
}

void lbu(char s1, char dest, int16_t data) {
    if (registers[s1] + data >= ADDR_DATA_P && registers[s1] + data > ADDR_DATA_LIMIT)
        exitError("Invalid memory access.", -11);

    registers[dest] = (int32_t)*memMap(registers[s1] + data) & 0x000000FF;
}

//----- J TYPE -----

