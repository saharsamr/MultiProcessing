#include "socketFunctions.h"
#include "serverFunctions.h"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char const *argv[]) {
  if(argc < 4){
    cout << "IP, port, and file address for this server are not given properly.\n";
    exit(EXIT_FAILURE);
  }

  int sockFd, clients[MAX_CLIENTS], max_sd;
  struct sockaddr_in socketInfo;
  if (createAndListenThroughtSocket(sockFd, socketInfo, inet_addr(argv[1]), atoi(argv[2]), clients)){
    fd_set readfds;
    while(true){
      setActiveFileDiscriptors(sockFd, clients, &readfds, max_sd);
      accepConnectionViaSelectFunc(sockFd, socketInfo, clients, max_sd, &readfds);
      string dirPath = argv[3];
      handleRequests(clients, &readfds, dirPath);
    }
  }
  return 0;
}
