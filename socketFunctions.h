#ifndef SOCKET_FUNCTIONS_H
#define SOCKET_FUNCTIONS_H

#define MAX_TRANSFER_SIZE 1024
#define MAX_CLIENTS 100
#define TRUE 1
#define FALSE 0

#include <netinet/in.h>

bool createAndConnectThroughSocket(int&,struct sockaddr_in&, unsigned long int, int);
bool createAndListenThroughtSocket(int&, struct sockaddr_in&, unsigned long int, int, int*);
void setActiveFileDiscriptors(int&, int*, fd_set*, int&);
void accepConnectionViaSelectFunc(int&, struct sockaddr_in&, int*, int&, fd_set*);

#endif
