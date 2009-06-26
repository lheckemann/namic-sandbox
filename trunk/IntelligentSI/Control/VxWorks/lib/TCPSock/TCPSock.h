#ifndef TCP_SOCK_H
#define TCP_SOCK_H

#include <vxWorks.h>
#include <stdioLib.h>

#include <ioLib.h>
#include <hostLib.h>
#include <sockLib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <iostream>
#include <strstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <stdio.h>

#define SOCK_BUFSIZE  128000


class TCPSock {
 private:
  static TCPSock* m_Instance;
  int sd;
 public:
  TCPSock();
/*
  // for server
  SOCK(int);
  // for client
  SOCK(char *,int);
*/
  ~TCPSock();
  static TCPSock* GetInstance();
  int sockServerInit(int portnum);
  int sockClientInit(char *hostname,int portnum);
  int sockSetOption(int sock);

  int Shut();
  int Read(char *,int);
  int Write(char *,int);
  int getSd();
  void SetSock(int sock);

};

#endif //TCP_SOCK_H
