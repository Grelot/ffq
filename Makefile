CXX=g++
CXXFLAGS=-O3 -funroll-loops -std=c++0x -Wall -fopenmp -finline-limit-50000 -lm -Dnullptr=0 -lz

PRG = ffq
OBJ = ffq.o

DEPS=input.h ffq.h

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
ffq: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f $(OBJ)
