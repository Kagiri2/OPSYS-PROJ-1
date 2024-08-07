all: 
	g++ -o main.out *.cpp -lm
run1: main.o
	./main.out 3 1 32 0.001 1024 4 0.75 256
run2: main.o
	./main.out 8 6 512 0.001 1024 6 0.9 128
run3: main.o
	./main.out 16 2 256 0.001 2048
run4: main.o
	./main.out 20 12 128 0.01 4096