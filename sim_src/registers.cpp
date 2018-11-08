#include <iostream>
#include "registers.hpp"

int32_t Registers::read(uint8_t addr) {
    return regArray[addr];
}

void  Registers::write(uint8_t addr, int32_t data) { 
    if (addr != 0)
        regArray[addr] = data;
}

char Registers::exitCode() {
    return regArray[2] & 0xFF;
}

int32_t Registers::operator[] (int index) {
    return regArray[index];
}

Registers::Registers() {
    for(int i = 0; i < 32; ++i) {
        regArray[i] = 0;
    }
}

Registers& Registers::operator=(const Registers& reg1) {
    for(int i = 0; i < 32; ++i) {
        regArray[i] = reg1.regArray[i];
    }
    return *this;
}