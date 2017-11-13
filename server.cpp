#include "socketFunctions.h"
#include "serverFunctions.h"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

int main(int argc, char const *argv[]) {
  if(argc < 4){
    cout << "IP, port, and file address for this server are not given properly.\n";
    exit(EXIT_FAILURE);
  }

  mkfifo("./serverFifo", 0666);
  int namedPipe[2];
  if(socketpair(AF_LOCAL, SOCK_STREAM, 0, namedPipe) < 0){
    perror("pair socket.\n");
    exit(EXIT_FAILURE);
  }
  close(namedPipe[1]);
  int fd_fifo = open("./serverFifo", O_WRONLY);

  int sockFd, clients[MAX_CLIENTS], max_sd;
  struct sockaddr_in socketInfo;
  if (createAndListenThroughtSocket(sockFd, socketInfo, inet_addr(argv[1]), atoi(argv[2]), clients)){
    fd_set readfds;
    while(true){
      setActiveFileDiscriptors(sockFd, clients, &readfds, max_sd);
      accepConnectionViaSelectFunc(sockFd, socketInfo, clients, max_sd, &readfds);
      string dirPath = argv[3];
      handleRequests(clients, &readfds, dirPath, fd_fifo);
    }
  }
  return 0;
}
