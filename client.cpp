#include "socketFunctions.h"
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

using namespace std;

int main(int argc, char const *argv[]) {
  if(argc < 3){
    cout << "IP and port of the server are not given properly.\n";
    exit(EXIT_FAILURE);
  }

  int sockFd;
  struct sockaddr_in socketInfo;
  if(createAndConnectThroughSocket(sockFd, socketInfo, inet_addr(argv[1]), atoi(argv[2]))){
    string carId;
    cout << "Enter car ID:\n";
    cin >> carId;
    if(send(sockFd, carId.c_str(), carId.size(), 0) < 0)
      cout << "sending error.\n";
    else{
      char response[MAX_TRANSFER_SIZE];
      if(recv(sockFd, response, MAX_TRANSFER_SIZE, 0) < 0)
        cout << "data didn't recieve from server.\n";
      else
        cout << "Car fine: " << response << endl;
    }
  }
  return 0;
}
