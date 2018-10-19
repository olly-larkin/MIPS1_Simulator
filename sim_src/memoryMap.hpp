#ifndef MEMORYMAP_HPP
#define MEMORYMAP_HPP

#include <iostream>
#include <fstream>

#define ADDR_NULL_P 0x00000000
#define ADDR_INSTR_P 0x10000000
#define ADDR_DATA_P 0x20000000
#define ADDR_GETC_P 0x30000000
#define ADDR_PUTC_P 0x30000004

#define ADDR_NULL_SIZE 0x4
#define ADDR_INSTR_SIZE 0x1000000
#define ADDR_DATA_SIZE 0x4000000
#define ADDR_GETC_SIZE 0x4
#define ADDR_PUTC_SIZE 0x4

class MemoryMap {
public:
    int32_t read(unsigned int addr, unsigned char byteNum, bool signedRead = false);
    void write(unsigned int addr, int32_t data, unsigned char byteNum);
    void instrDump(std::ifstream& binFile);
private:
    char ADDR_NULL[ADDR_NULL_SIZE];
    char ADDR_INSTR[ADDR_INSTR_SIZE];
    char ADDR_DATA[ADDR_DATA_SIZE];
    char ADDR_GETC[ADDR_GETC_SIZE];
    char ADDR_PUTC[ADDR_PUTC_SIZE];
};

#endif //MEMORYMAP_HPP