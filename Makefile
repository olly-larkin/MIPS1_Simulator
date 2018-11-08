CXX=g++
CXXARGS=--std=c++11 -Wall -Wextra -Werror -pedantic

simulator: main.o simulator.o memoryMap.o registers.o
	mkdir -p bin
	$(CXX) $(CXXARGS) sim_src/main.o sim_src/simulator.o sim_src/memoryMap.o sim_src/registers.o -o bin/mips_simulator

testbench:
	rm -rf test
	git submodule update --init --recursive
	make -C parser -f ./Makefile parser
	tbench_src/bin/bingen

tb_run: testbench simulator
	bin/mips_testbench bin/mips_simulator

testbench_windows:
	make simulator
	dos2unix bin/win_testbench

main.o: sim_src/main.cpp
	$(CXX) $(CXXARGS) -c sim_src/main.cpp -o sim_src/main.o

simulator.o: sim_src/simulator.cpp sim_src/simulator.hpp
	$(CXX) $(CXXARGS) -c sim_src/simulator.cpp -o sim_src/simulator.o

memoryMap.o: sim_src/memoryMap.cpp sim_src/memoryMap.hpp
	$(CXX) $(CXXARGS) -c sim_src/memoryMap.cpp -o sim_src/memoryMap.o

registers.o: sim_src/registers.cpp sim_src/registers.hpp
	$(CXX) $(CXXARGS) -c sim_src/registers.cpp -o sim_src/registers.o

clean:
	rm -rf sim_src/*.o bin/mips_simulator test
	make -C parser -f ./Makefile clean
