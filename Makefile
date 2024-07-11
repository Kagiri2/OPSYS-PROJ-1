all: 
	g++ -o main.out *.cpp -lm
run1: main.o
	./main.out 3 1 32 0.001 1024
