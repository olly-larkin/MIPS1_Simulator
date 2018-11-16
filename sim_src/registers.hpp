#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <iostream>

class Registers {
public:
    uint32_t read(uint8_t addr);
    void write(uint8_t addr, uint32_t data);
    char exitCode();
    uint32_t operator[] (int index);
    Registers();
    Registers& operator=(const Registers& reg1);
private:
    uint32_t regArray[32];
};

#endif //REGISTERS_HPP