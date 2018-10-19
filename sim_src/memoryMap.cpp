#include <iostream>
#include "memoryMap.hpp"
#include <fstream>

int32_t MemoryMap::read(unsigned int addr, unsigned char byteNum, bool signedRead) {
    if (addr == ADDR_GETC_P) {
        int32_t inputVal = getchar();
        if (inputVal == EOF)
            return -1;
        return inputVal;
    }
    
    int32_t returnVal = 0;
    for(int i = 0; i < byteNum; ++i) {
        if (addr < ADDR_NULL_SIZE) {
            //Nothing should happen
        } else if (addr >= ADDR_INSTR_P && addr < ADDR_INSTR_P + ADDR_INSTR_SIZE) {
            returnVal += ((ADDR_INSTR[addr - ADDR_INSTR_P] & 0xFF) << (i*8));
        } else if (addr >= ADDR_DATA_P && addr < ADDR_DATA_P + ADDR_DATA_SIZE) {
            returnVal += ((ADDR_DATA[addr - ADDR_DATA_P] & 0xFF) << (i*8));
        } else if (addr >= ADDR_PUTC_P && addr < ADDR_PUTC_P + ADDR_PUTC_SIZE) {
            returnVal += ((ADDR_PUTC[addr - ADDR_PUTC_P] & 0xFF) << (i*8));
        } else {
            std::cerr << "Invalid memory read." << std::endl;
            std::exit(-11);
        }
        addr += 4;
    }

    if (signedRead && ((returnVal >> (byteNum*8 - 1)) & 0x1)) {
        returnVal += (0xFFFFFFFF << (byteNum*8));
    }

    return returnVal;
}

void MemoryMap::write(unsigned int addr, int32_t data, unsigned char byteNum) {
    if (addr == ADDR_PUTC_P) {
        putchar(data & 0xFF);
    }

    if (byteNum == 2 && (addr & 0x1)) {
            std::cerr << "Invalid memory write: Address must be naturally aligned." << std::endl;
            std::exit(-11);
    }

    for(int i = 0; i < byteNum; ++i) {
        if (addr >= ADDR_DATA_P && addr < ADDR_DATA_P + ADDR_DATA_SIZE) {
            ADDR_DATA[addr - ADDR_DATA_P] = (data & 0xFF);
        } else {
            std::cerr << "Invalid memory write." << std::endl;
            std::exit(-11);
        }
        addr += 4;
        data = data >> 8;
    }
}

void MemoryMap::instrDump(std::ifstream& binFile) {
    binFile.seekg(0, std::ios::end);
    std::streampos size = binFile.tellg();
    if (size > ADDR_INSTR_SIZE) {
        std::cerr << ".bin file too large." << std::endl;
        std::exit(-21);
    }
    binFile.seekg(0, std::ios::beg);
    binFile.read(&ADDR_INSTR[0], size);
    binFile.close();
    std::cerr << "Binary read into memory." << std::endl;
}