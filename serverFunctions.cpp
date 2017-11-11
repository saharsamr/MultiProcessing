#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "serverFunctions.h"
#include "socketFunctions.h"
#include <dirent.h>
#include <string>
#include <cstring>

using namespace std;

void handleRequests(int* client_sockets, fd_set* readfds, string& dirPath){
  char buffer[MAX_TRANSFER_BYTES], response[MAX_TRANSFER_BYTES];
  int i, valread;
  for (i = 0; i < MAX_CLIENTS; i++){
    int sd = client_sockets[i];
    if (FD_ISSET(sd, readfds)){
      memset(buffer, '\0', MAX_TRANSFER_BYTES);
      if ((valread = recv(sd ,buffer, MAX_TRANSFER_BYTES, 0)) == 0){
        close(sd);
        client_sockets[i] = 0;
      }
      else
        prepareDriverFine(buffer, valread, dirPath);
    }
  }
}

void prepareDriverFine(char* buffer, int valread, string& dirPath){
  buffer[valread] = '\0';
  cout << buffer << "\n";
  int file_desc[2];
  if(pipe(file_desc) < 0){
    cout << "pipe error.\n";
    return;
  }
  int pid = fork();
  if(pid == 0){
    close(file_desc[0]);
    string result = "";
    write(file_desc[1], findResults(result, dirPath).c_str(), 1024);
    exit(0);
  }
  else{
    char buffer[1024];
    close(file_desc[1]);
    read(file_desc[0], buffer, 1024);
    close(file_desc[0]);
    cout << buffer << "\n";
  }
}

string findResults(string& result, string& dirPath){
  DIR *dirp;
  struct dirent *directory;
  dirp = opendir(dirPath.c_str());
  if (dirp){
    while ((directory = readdir(dirp)) != NULL){
      result += directory->d_name;
      // printf("%s\n", directory->d_name);
    }
    closedir(dirp);
  }
  return result;
}
