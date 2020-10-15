all: prog test

prog:
	g++ -std=c++17 Task_1.cpp -o Task_1
test: 
	./Task_1
