#include <iostream>
#include <stdlib.h>
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
#include "directoryParsing.h"
#include <dirent.h>
#include <string>
#include <cstring>
#include <fstream>

using namespace std;

void handleRequests(int* client_sockets, fd_set* readfds, string& dirPath){
  char buffer[MAX_TRANSFER_BYTES], response[MAX_TRANSFER_BYTES];
  string answer;
  int i, valread;
  for (i = 0; i < MAX_CLIENTS; i++){
    int sd = client_sockets[i];
    if (FD_ISSET(sd, readfds)){
      memset(buffer, '\0', MAX_TRANSFER_BYTES);
      if ((valread = recv(sd ,buffer, MAX_TRANSFER_BYTES, 0)) == 0){
        close(sd);
        client_sockets[i] = 0;
      }
      else{
        answer = prepareDriverFine(buffer, valread, dirPath);
        write(sd, answer.c_str(), answer.size());
      }
    }
  }
}

string prepareDriverFine(char* buffer, int valread, string& dirPath){
  buffer[valread] = '\0';
  cout << buffer << "\n";
  int file_desc[2];
  if(pipe(file_desc) < 0){
    cout << "pipe error.\n";
    return "";
  }
  int pid = fork();
  if(pid == 0){
    close(file_desc[0]);
    string result = "0";
    findResults(dirPath, buffer, result);
    write(file_desc[1], result.c_str(), sizeof(result));
    close(file_desc[1]);
    exit(0);
  }
  else{
    wait(NULL);
    char result[1024];
    close(file_desc[1]);
    read(file_desc[0], result, 1024);
    close(file_desc[0]);
    cout << result << "\n";
    return result;
  }
}

string findResults(string& dirPath, char* buffer, string& result){
  vector <string> files;
  read_directory(dirPath, files);

  if(files.size() == 0)
    return result;

  int fileDescs[files.size()][2];

  int status;

  for (int i = 0; i < files.size(); i++){
    pipe(fileDescs[i]);
    int childPid = fork();
    string newPath = dirPath+"/"+files[i];
    if(childPid == 0){
      if(is_regular_file(newPath.c_str())){
        close(fileDescs[i][0]);
        string temp = to_string(searchInTextFile(buffer, newPath));
        write(fileDescs[i][1], temp.c_str(), sizeof(temp));
        close(fileDescs[i][1]);
        exit(0);
      }
      else{
        close(fileDescs[i][0]);
        string temp;
        temp = findResults(newPath, buffer, result);
        write(fileDescs[i][1], temp.c_str(), sizeof(result));
        close(fileDescs[i][1]);
        exit(0);
      }
    }
  }
  wait(&status);
  for(int i = 0; i < files.size(); i++){
    close(fileDescs[i][1]);
    char fine[1024];
    memset(fine, '\0', 1024);
    read(fileDescs[i][0], fine, 1024);
    result = to_string(stoi(result) + atoi(fine));
    close(fileDescs[i][0]);
  }
  return result;
}

int searchInTextFile(char* driverId, string& path){
  ifstream fineFile(path.c_str());
  int sum = 0;
  string id, date, fineValue;

  while(fineFile >> id >> date >> fineValue)
    if(strcmp(driverId, id.c_str()) == 0)
      sum += atoi(fineValue.c_str());

  return sum;
}
