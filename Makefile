CC = g++ -std=c++11

all: client server

client: client.o socketFunctions.o
	$(CC) client.o socketFunctions.o -o client

client.o: client.cpp socketFunctions.h
	$(CC) -c client.cpp -o client.o

server: server.o socketFunctions.o serverFunctions.o directoryParsing.o
	$(CC) server.o socketFunctions.o serverFunctions.o directoryParsing.o -o server

server.o: server.cpp socketFunctions.h serverFunctions.h directoryParsing.h
	$(CC) -c server.cpp -o server.o

tools.o: socketFunctions.h socketFunctions.cpp
	$(CC) -c socketFunctions.cpp -o socketFunctions.o

serverFunctions.o: serverFunctions.h serverFunctions.cpp
	$(CC) -c serverFunctions.cpp -o serverFunctions.o

directoryParsing.o: directoryParsing.h directoryParsing.cpp
	$(CC) -c directoryParsing.cpp -o directoryParsing.o

clean:
	rm client server *.o
