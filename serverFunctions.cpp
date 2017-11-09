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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "serverFunctions.h"
#include "socketFunctions.h"
#include <dirent.h>
#include <string>

using namespace std;

void handleRequests(int* client_sockets, fd_set* readfds, string& dirPath){
  char buffer[MAX_TRANSFER_BYTES], response[MAX_TRANSFER_BYTES];
  int i;
  for (i = 0; i < MAX_CLIENTS; i++){
    int sd = client_sockets[i];
    if (FD_ISSET(sd, readfds)){
      memset(buffer, '\0', MAX_TRANSFER_BYTES);
      int valread;
      if ((valread = recv(sd ,buffer, MAX_TRANSFER_BYTES, 0)) == 0){
        close(sd);
        client_sockets[i] = 0;
      }
      else{
        buffer[valread] = '\0';
        cout << buffer << "\n";
        int pid = fork();
        if(pid == 0){
          string result = "";
          findResults(result, dirPath);
        }
        else
          cout << "parent.\n" << getpid() << "\n";\
      }
    }
  }
}

void findResults(string& result, string& dirPath){
  DIR *dirp;
  struct dirent *directory;
  dirp = opendir(dirPath.c_str());
  if (dirp){
    while ((directory = readdir(dirp)) != NULL){
      printf("%s\n", directory->d_name);
    }
    closedir(dirp);
  }
}
