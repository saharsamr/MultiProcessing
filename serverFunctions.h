#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include <unistd.h>
#include <string>

#define MAX_CLIENTS 100
#define MAX_TRANSFER_BYTES 1024

void handleRequests(int*, fd_set*, std::string&, int&);
int searchInTextFile(char*, std::string&);
std::string prepareDriverFine(char*, int, std::string&);
std::string findResults(std::string&, char*, std::string&);
ssize_t sock_fd_write(int sock,void *buf, ssize_t buflen, int fd);
ssize_t sock_fd_read(int sock, int *fd);

#endif
