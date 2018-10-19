#include <iostream>
#include "registers.hpp"

int32_t Registers::read(char addr) {
    return regArray[addr];
}

void  Registers::write(char addr, int32_t data) { 
    if (addr != 0)
        regArray[addr] = data;
}

char Registers::exitCode() {
    return regArray[2] & 0xFF;
}