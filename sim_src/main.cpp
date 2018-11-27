#include <iostream>
#include <fstream>
#include "simulator.hpp"

int main(int argc, const char * argv[]) {
    std::cerr << std::endl;
    if(argc == 1) {
        std::cerr << "No binary file passed." << std::endl << std::endl;
        std::exit(-21);
    }

    std::ifstream binaryFile(argv[1], std::ios::in | std::ios::binary | std::ios::ate);
    if (!binaryFile.is_open()) {
        std::cerr << "Unable to open binary file: " << argv[1] << std::endl << std::endl;
        std::exit(-21);
    }
    
    std::cerr << "Binary file opened: " << argv[1] << std::endl;
    
    int exitCode;
    try {
        Simulator sim;
        sim.loadInstr(binaryFile);
        exitCode = sim.execute();
    } catch(...) {
        std::cerr << "Unknown simulator error..." << std::endl;
        std::exit(-20);
    }

    std::cerr << "Successful execution: " << exitCode << std::endl << std::endl;
    std::exit(exitCode);
}