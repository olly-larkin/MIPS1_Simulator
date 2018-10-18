simulator: main.o MIPS_instructions.o
	g++ sim_src/main.o sim_src/MIPS_instructions.o -o bin/mips_simulator

main.o: sim_src/main.cpp
	g++ --std=c++11 -c sim_src/main.cpp -o sim_src/main.o

MIPS_instructions.o: sim_src/MIPS_instructions.cpp
	g++ --std=c++11 -c sim_src/MIPS_instructions.cpp -o sim_src/MIPS_instructions.o

clean:
	rm sim_src/*.o bin/simulator

windows_clean: 
	del /F /Q *.o
	del /F /Q bin\simulator.exe