#include "socketFunctions.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

bool createAndConnectThroughSocket(int& socketFd, struct sockaddr_in& socketStruct, unsigned long int IP, int port){
  socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketFd == -1){
    cout << "Socket creation failed.\n";
    return false;
  }

  socketStruct.sin_family = AF_INET;
  socketStruct.sin_addr.s_addr = IP;
  socketStruct.sin_port= htons(port);

  if (connect(socketFd, (sockaddr*)&socketStruct, sizeof(socketStruct)) < 0){
    cout << "Connection error.\n";
    return false;
  }
  return true;
}

bool createAndListenThroughtSocket(int& socketFd, struct sockaddr_in& socketStruct, unsigned long int IP, int port, int* client_sockets){
  if ((socketFd = socket(AF_INET , SOCK_STREAM , 0)) == 0){
    write(2, "socket failed", 13);
    return false;
  }
  int opt = TRUE, i;
  if( setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ){
    write(2, "setsockopt error", 16);
    return false;
  }
  socketStruct.sin_family = AF_INET;
  socketStruct.sin_addr.s_addr = IP;
  socketStruct.sin_port = htons(port);
  if (bind(socketFd, (struct sockaddr *)&socketStruct, sizeof(socketStruct)) < 0){
    write(2, "bind failed", 11);
    return false;
  }
  if (listen(socketFd, MAX_CLIENTS) < 0){
    write(2, "listen error", 12);
    return false;
  }
  for (i = 0; i < MAX_CLIENTS; i++)
    client_sockets[i] = 0;
  write(1, "Waiting for connections...\n", 27);
  return true;
}

void setActiveFileDiscriptors(int& serverSocket, int* client_sockets, fd_set* readfds, int& max_sd){
  FD_ZERO(readfds);
  FD_SET(serverSocket, readfds);
  max_sd = serverSocket;
  int i;
  for ( i = 0 ; i < MAX_CLIENTS ; i++){
    int sd = client_sockets[i];
    if(sd > 0)
      FD_SET(sd, readfds);
    if(sd > max_sd)
      max_sd = sd;
  }
}

void accepConnectionViaSelectFunc(int& serverSocket, struct sockaddr_in& socketStruct, int* client_sockets, int& max_sd, fd_set* readfds){
  int activity = select(max_sd+1, readfds, NULL, NULL, NULL);
  if ((activity < 0) && (errno!=EINTR)){
    write(2, "select error\n", 13);
    return;
  }
  if (FD_ISSET(serverSocket, readfds)){
    int new_socket, addrlen = sizeof(socketStruct);
    if ((new_socket = accept(serverSocket, (struct sockaddr *)&socketStruct, (socklen_t*)&addrlen)) < 0){
      write(2, "accept error", 12);
      return;
    }
    write(1, "connection accepted.\n", 21);
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
      if( client_sockets[i] == 0 ){
        client_sockets[i] = new_socket;
        break;
      }
  }
}
