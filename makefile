main: main.cpp
	g++ -o weblet main.cpp -march=native -std=c++14 -fopenmp `pkg-config --cflags opencv` `pkg-config --libs opencv`
