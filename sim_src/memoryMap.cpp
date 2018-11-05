#include <iostream>
#include "memoryMap.hpp"
#include <fstream>

MemoryMap::MemoryMap() {
    ADDR_NULL = std::vector<char>(ADDR_NULL_SIZE, 0);
    ADDR_INSTR = std::vector<char>(ADDR_INSTR_SIZE, 0);
    ADDR_DATA = std::vector<char>(ADDR_DATA_SIZE, 0);
    ADDR_GETC = std::vector<char>(ADDR_GETC_SIZE, 0);
    ADDR_PUTC = std::vector<char>(ADDR_PUTC_SIZE, 0);
}

int32_t MemoryMap::read(unsigned int addr, unsigned char byteNum, bool signedRead) {
    if (addr % byteNum != 0) {
        std::cerr << "Unaligned memory access." << std::endl << std::endl;
        std::exit(-11);
    }
    
    int32_t returnVal = 0;
    for(int i = 0; i < byteNum; ++i) {
        int shift = (byteNum - i - 1) * 8;
        if (addr < ADDR_NULL_SIZE) {
            //Nothing should happen
        } else if (addr >= ADDR_INSTR_P && addr < ADDR_INSTR_P + ADDR_INSTR_SIZE) {
            returnVal += ((ADDR_INSTR[addr - ADDR_INSTR_P] & 0xFF) << shift);
        } else if (addr >= ADDR_DATA_P && addr < ADDR_DATA_P + ADDR_DATA_SIZE) {       
            returnVal += ((ADDR_DATA[addr - ADDR_DATA_P] & 0xFF) << shift);
        } else if (addr >= ADDR_GETC_P && addr < ADDR_GETC_P + ADDR_GETC_SIZE) {
            if (addr == ADDR_GETC_P + 3) {
                int32_t inputVal = getchar();
                if (inputVal == EOF)
                    return -1;
                returnVal += inputVal;
            }
        } else {
            std::cerr << "Invalid memory read." << std::endl << std::endl;
            std::exit(-11);
        }
        addr += 1;
    }

    if (signedRead && ((returnVal >> (byteNum*8 - 1)) & 0x1)) {
        returnVal += (0xFFFFFFFF << (byteNum*8));
    }
    return returnVal;
}

void MemoryMap::write(unsigned int addr, int32_t data, unsigned char byteNum) {
    if (addr % byteNum != 0) {
        std::cerr << "Invalid memory write: Address must be naturally aligned." << std::endl << std::endl;
        std::exit(-11);
    }

    char out = 0;
    bool output = false;

    for(int i = 0; i < byteNum; ++i) {
        int shift = (byteNum - i - 1) * 8;
        if (addr >= ADDR_DATA_P && addr < ADDR_DATA_P + ADDR_DATA_SIZE) {
            ADDR_DATA[addr - ADDR_DATA_P] = ((data >> shift) & 0xFF);
        } else if (addr >= ADDR_PUTC_P && addr < ADDR_PUTC_P + ADDR_PUTC_SIZE) {
            output = true;
            if (addr == ADDR_PUTC_P + 3)
                //putchar(data & 0xFF);
                out = data & 0xFF;
        } else {
            std::cerr << "Invalid memory write." << std::endl << std::endl;
            std::exit(-11);
        }
        addr += 1;
    }
    if (output)
        putchar(out);
}

void MemoryMap::instrDump(std::ifstream& binFile) {
    binFile.seekg(0, std::ios::end);
    std::streampos size = binFile.tellg();
    if (size > ADDR_INSTR_SIZE) {
        std::cerr << ".bin file too large." << std::endl << std::endl;
        std::exit(-21);
    }
    binFile.seekg(0, std::ios::beg);
    binFile.read(&ADDR_INSTR[0], size);
    binFile.close();
    std::cerr << "Binary read into memory." << std::endl;
}