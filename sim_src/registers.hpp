#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <iostream>

class Registers {
public:
    int32_t read(char addr);
    void write(char addr, int32_t data);
    char exitCode();
    int32_t operator[] (int index);
    Registers();
    Registers& operator=(const Registers& reg1);
private:
    int32_t regArray[32];
};

#endif //REGISTERS_HPP