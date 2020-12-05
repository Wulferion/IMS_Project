CPP=g++

all: simulib

simulib: simulib.cpp
			$(CPP) main.cpp simulib.cpp -o sim.out

run: ./sim

clean:
	rm sim.out