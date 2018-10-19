#include "MIPS_instructions.hpp"
#include <iostream>
#include <string>
#include <map>

std::map<char, rTypeFunc> R_FUNC = {
    {"add", 32}
};
std::map<char, iTypeFunc> I_FUNC = {
    
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
    else if (addr >= 0x30000000 && addr < 0x30000004) {
        char input = getchar();
        if (input == EOF)
            input = -1;
        ADDR_GETC[addr - 0x30000000] = input;
        return &ADDR_GETC[addr - 0x30000000];
    } else if (addr >= 0x30000004 && addr < 0x30000008)
        return &ADDR_PUTC[addr - 0x30000004];
    else {
        exitError("Invalid memory access.", -11);
        return ADDR_NULL;
    }
}

void successfulExit() {
    programCounter = 0;
    std::cerr << "Successful execution." << std::endl << std::endl;
    std::exit(returnCode());
}

bool validDest(char dest) {
    if (dest == 0 || dest == 1 || dest == 26 || dest == 27) {
        exitError("Invalid register destination.", -12);
        return false;
    }
    return true;
}

//************************** MIPS INSTRUCTIONS **************************

//----- R TYPE -----

void add(char s1, char s2, char dest, char shAmt) {
    int32_t in1 = registers[s1];
    int32_t in2 = registers[s2];
    int32_t out = in1 + in2;
    if (((in1 >> 31) == (in2 >> 31)) && ((out >> 31) != (in1 >> 31))) {
        exitError("Overflow detected", -10);
    }
    if (validDest(dest))
        registers[dest] = out;
}

//----- I TYPE -----



//----- J TYPE -----

