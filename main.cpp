#include <iostream>
#include <fstream>
#include "MIPS_instructions.hpp"

enum INSTRUCTION_TYPE {R, J, I};

INSTRUCTION_TYPE opcode(int32_t);
void execute(int32_t);

int main(int argc, const char * argv[]) {
    std::cerr << std::endl;
    if(argc == 1)
        exitError("No binary file passed.", 0);     // PROGRAM SHOULD EXIT IF NO FILE PASSED IN
    
    std::ifstream binaryFile(argv[1], std::ios::in | std::ios::binary | std::ios::ate);
    if (!binaryFile.is_open())
        exitError("Unable to open binary file.", 0);        // PROGRAM EXIT IF FILE DOES NOT EXIST OR CANNOT BE OPENED
    
    std::cerr << "Binary file opened: " << argv[1] << std::endl;
    
    std::streampos size = binaryFile.tellg();
    binaryFile.seekg(0, std::ios::beg);
    binaryFile.read(executeMem(), size);
    binaryFile.close();
    std::cerr << "Binary read into memory." << std::endl;
    
    while(pcLocation() != 0) {
        int32_t instr = getNextInstr();
        
        if (instr == 0)
            //exitError("Exit instruction not defined.", -12);
            successfulExit();
        
        execute(instr);
        
        if (pcLocation() >= ADDR_INSTR_LIMIT)
            exitError("Program Counter has exceeded executable memory.", -11);
    }
}

INSTRUCTION_TYPE opcode(int32_t instr) {
    instr = instr >> 26;
    if (instr == 0)
        return R;
    else if (instr == 0b000010 || instr == 0b000011)
        return J;
    else
        return I;
}

void execute(int32_t instr) {
    switch(opcode(instr)) {
        case R: {
            char fn, s1, s2, dest, shAmt;
            fn = instr & 0x3F;
            s1 = (instr >> 21) & 0x1F;
            s2 = (instr >> 16) & 0x1F;
            dest = (instr >> 11) & 0x1F;
            shAmt = (instr >> 6) & 0x1F;
            executeR(fn, s1, s2, dest, shAmt);
            break;
        }
        case I: {
            char op, s1, dest;
            int16_t data;
            op = (instr >> 26) & 0x3F;
            s1 = (instr >> 21) & 0x1F;
            dest = (instr >> 16) & 0x1F;
            data = instr & 0xFFFF;
            executeI(op, s1, dest, data);
            break;
        }
        case J: {
            char op = (instr >> 26) & 0x3F;
            int32_t addr = instr & 0x03FFFFFF;
            executeJ(op, addr);
            break;
        }
    }
}
