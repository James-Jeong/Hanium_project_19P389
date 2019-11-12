all: Server_v1

Server_v1: Server_v1.o
	gcc -o Server_v1 Server_v1.o -lmysqlclient -lstdc++

Server_v1.o: Server_v1.cpp
	gcc -c -o Server_v1.o Server_v1.cpp

clean:
	rm -f *.o
	rm -f Server_v1

.PHONY: clean

