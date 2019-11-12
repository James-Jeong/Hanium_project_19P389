all: TCPserver

TCPserver: Processing.o main.o
	gcc -o TCPserver Processing.o main.o -lpthread

Processing.o: Processing.h Processing.c
	gcc -c -o Processing.o Processing.c

main.o: Processing.h main.c
	gcc -c -o main.o main.c -lpthread

clean:
	rm -f Processing.o main.o
	rm -f TCPserver

.PHONY: clean

