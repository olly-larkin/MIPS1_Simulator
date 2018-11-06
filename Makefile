simulator: main.o simulator.o memoryMap.o registers.o
	mkdir -p bin
	g++ sim_src/main.o sim_src/simulator.o sim_src/memoryMap.o sim_src/registers.o -o bin/mips_simulator

testbench:
	make clean
	git submodule update --init --recursive
	make simulator
	make -C parser -f ./Makefile parser
	tbench_src/bin/bingen

testbench_windows:
	make simulator
	#tr -d '\r' <bin/mips_testbench> bin/win_testbench
	dos2unix bin/win_testbench

main.o: sim_src/main.cpp
	g++ --std=c++11 -c sim_src/main.cpp -o sim_src/main.o

simulator.o: sim_src/simulator.cpp sim_src/simulator.hpp
	g++ --std=c++11 -c sim_src/simulator.cpp -o sim_src/simulator.o

memoryMap.o: sim_src/memoryMap.cpp sim_src/memoryMap.hpp
	g++ --std=c++11 -c sim_src/memoryMap.cpp -o sim_src/memoryMap.o

registers.o: sim_src/registers.cpp sim_src/registers.hpp
	g++ --std=c++11 -c sim_src/registers.cpp -o sim_src/registers.o

clean:
	rm -f -rf sim_src/*.o bin/mips_simulator test
	make -C parser -f ./Makefile clean
