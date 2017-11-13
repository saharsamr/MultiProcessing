#include "socketFunctions.h"
#include "serverFunctions.h"
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>

#define MAX_DIGIT_NUM 20

using namespace std;

ssize_t sock_fd_read(int sock,void *buf, ssize_t bufsize, int *fd);

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
      int fd_fifo;
      memset(response, '\0', MAX_DIGIT_NUM);
      sock_fd_read(sockFd, response, MAX_DIGIT_NUM, &fd_fifo);
      cout << "Car fine: " << response << endl;
    }
  }
  return 0;
}

ssize_t sock_fd_read(int sock, void *buf, ssize_t bufsize, int *fd){
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
            perror ("recvmsg");
            exit(1);
        }
        cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg && cmsg->cmsg_len == CMSG_LEN(sizeof(int))) {
            if (cmsg->cmsg_level != SOL_SOCKET) {
                fprintf (stderr, "invalid cmsg_level %d\n",
                     cmsg->cmsg_level);
                exit(1);
            }
            if (cmsg->cmsg_type != SCM_RIGHTS) {
                fprintf (stderr, "invalid cmsg_type %d\n",
                     cmsg->cmsg_type);
                exit(1);
            }

            *fd = *((int *) CMSG_DATA(cmsg));
            printf ("received fd %d\n", *fd);
        } else
            *fd = -1;
    } else {
        size = read (sock, buf, bufsize);
        if (size < 0) {
            perror("read");
            exit(1);
        }
    }
    return size;
}
