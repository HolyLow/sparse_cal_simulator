CC = g++
sim: main.o PE.o
	$(CC) -o sim main.o PE.o

main.o: main.c 	PE.h

PE.o: PE.c PE.h


