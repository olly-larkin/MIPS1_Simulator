#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <iostream>

class Registers {
public:
    int32_t read(uint8_t addr);
    void write(uint8_t addr, int32_t data);
    char exitCode();
    int32_t operator[] (int index);
    Registers();
    Registers& operator=(const Registers& reg1);
private:
    int32_t regArray[32];
};

#endif //REGISTERS_HPP