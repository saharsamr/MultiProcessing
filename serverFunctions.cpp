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
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

void handleRequests(int* client_sockets, fd_set* readfds, string& dirPath, int& fd_fifo){
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
        char temp[20];
        memset(temp, '\0', 20);
        strcpy(temp, answer.c_str());
        // write(fd_fifo, answer.c_str(), answer.size());
        sock_fd_write(sd, temp, answer.size(), fd_fifo);
        // close(fd_fifo);
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

ssize_t
sock_fd_write(int sock,void *buf, ssize_t buflen, int fd)
{
    ssize_t     size;
    struct msghdr   msg;
    struct iovec    iov;
    union {
        struct cmsghdr  cmsghdr;
        char        control[CMSG_SPACE(sizeof (int))];
    } cmsgu;
    struct cmsghdr  *cmsg;

    iov.iov_base = buf;
    iov.iov_len = buflen;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if (fd != -1) {
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_len = CMSG_LEN(sizeof (int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;

        printf ("passing fd %d\n", fd);
        *((int *) CMSG_DATA(cmsg)) = fd;
    } else {
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        // printf ("not passing fd\n");
    }

    size = sendmsg(sock, &msg, 0);

    if (size < 0)
        perror ("sendmsg");
    return size;
}

ssize_t sock_fd_read(int sock, int *fd)
{
    char buf[16];
    ssize_t bufsize = sizeof buf;
    ssize_t     size;

    if (fd) {
        struct msghdr   msg;
        struct iovec    iov;
        union {
            struct cmsghdr  cmsghdr;
            char        control[CMSG_SPACE(sizeof (int))];
        } cmsgu;
        struct cmsghdr  *cmsg;

        iov.iov_base = buf;
        iov.iov_len = bufsize;

        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);
        size = recvmsg (sock, &msg, 0);
        if (size < 0) {
            cout<<"recv failed"<<endl;
            exit(1);
        }
        cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg && cmsg->cmsg_len == CMSG_LEN(sizeof(int))) {
            if (cmsg->cmsg_level != SOL_SOCKET) {
                exit(1);
            }
            if (cmsg->cmsg_type != SCM_RIGHTS) {
                exit(1);
            }

            *fd = *((int *) CMSG_DATA(cmsg));
        } else
            *fd = -1;
    } else {
        size = read (sock, buf, bufsize);
        if (size < 0) {
            cout<<"read failed"<<endl;
            exit(1);
        }
    }
    return size;
}
