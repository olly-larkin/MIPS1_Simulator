simulator: main.o MIPS_instructions.o
	g++ main.o MIPS_instructions.o -o bin/simulator

main.o: main.cpp
	g++ --std=c++11 -c main.cpp

MIPS_instructions.o: MIPS_instructions.cpp
	g++ --std=c++11 -c MIPS_instructions.cpp
