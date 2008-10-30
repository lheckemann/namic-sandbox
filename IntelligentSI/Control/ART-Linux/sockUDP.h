#ifndef SOCK_H
#define SOCK_H

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <stdio.h>

#ifndef __linux__ //if not __linux__

#include <strstream>
#include <vxWorks.h>
#include <stdioLib.h>

#include <ioLib.h>
#include <hostLib.h>
#include <sockLib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#else //if __linux__

#include <sstream>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <cerrno>

#include "common.h" //for OK, ERROR
#endif //if not __linux__



#define SOCK_BUFSIZE  512000


class udpSOCK {
 private:
  int sd;
  struct sockaddr_in server_addr, client_addr;
 public:
  // for server
  udpSOCK(char *,int);
  // for client
  udpSOCK(int,char *,int); 
  ~udpSOCK();
  int Shut();
  int Read(char *,int);
  int Write(char *,int);
  int getSd();
  int sockServerInit(char *,int);
  int sockClientInit(char *,int);
  int sockSetOption(int);
  int setSendBuf(int, int);
  int setRecvBuf(int, int);
};

#endif
 //SOCK_H
