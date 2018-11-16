#include <iostream>
#include "registers.hpp"

uint32_t Registers::read(uint8_t addr) {
    return regArray[addr];
}

void  Registers::write(uint8_t addr, uint32_t data) { 
    if (addr != 0)
        regArray[addr] = data;
}

char Registers::exitCode() {
    return regArray[2] & 0xFF;
}

uint32_t Registers::operator[] (int index) {
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