main: main.cpp
	g++ -o weblet.o main.cpp  -O2 -march=native -std=c++14 -fopenmp `pkg-config --cflags opencv` `pkg-config --libs opencv`
