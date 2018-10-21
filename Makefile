#simulator: main.o MIPS_instructions.o
#	g++ sim_src/main.o sim_src/MIPS_instructions.o -o bin/mips_simulator

#MIPS_instructions.o: sim_src/MIPS_instructions.cpp
#	g++ --std=c++11 -c sim_src/MIPS_instructions.cpp -o sim_src/MIPS_instructions.o

simulator: main.o simulator.o memoryMap.o registers.o
	g++ sim_src/main.o sim_src/simulator.o sim_src/memoryMap.o sim_src/registers.o -o bin/mips_simulator

testbench:
	make simulator
	chmod u+x bin/mips_testbench

main.o: sim_src/main.cpp
	g++ --std=c++11 -c sim_src/main.cpp -o sim_src/main.o

simulator.o: sim_src/simulator.cpp sim_src/simulator.hpp
	g++ --std=c++11 -c sim_src/simulator.cpp -o sim_src/simulator.o

memoryMap.o: sim_src/memoryMap.cpp sim_src/memoryMap.hpp
	g++ --std=c++11 -c sim_src/memoryMap.cpp -o sim_src/memoryMap.o

registers.o: sim_src/registers.cpp sim_src/registers.hpp
	g++ --std=c++11 -c sim_src/registers.cpp -o sim_src/registers.o

clean:
	rm sim_src/*.o bin/mips_simulator