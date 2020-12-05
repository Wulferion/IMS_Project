CPP=g++
CPPFLAGS=-std=c++17 -Wall

all: simulib

simulib: simulib.cpp
			$(CPP) $(CPPFLAGS) main.cpp simulib.cpp -o sim.out

run: ./sim

clean:
	rm sim.out