/******************************************************************************
 * FileName      : UDPSock.h
 * Created       :
 * LastModified  :
 * Author        :
 * Aim           : UDP/IP socket class
 *
 * OS            : VxWorks 5.5.1
 *****************************************************************************/

#ifndef UDP_SOCK__H
#define UDP_SOCK__H

#include <vxWorks.h>
#include <taskLib.h>
#include <stdlib.h>
#include <semLib.h>
#include <sysLib.h>
#include <logLib.h>
#include <intLib.h>

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

//#define SOCK_BUFSIZE  512000

//! Usage ==== Example
//!  Server Type :
//!   sock_m2s = new UDPSock(1,"teal", 10000);
//!  Client Type :
//!   sock_m2s = new UDPSock(2,"rail", 10000);

//! UDP/IP Socket class
class UDPSock {
 private:
  //! socket descriptor
  int sd;

  //! UDPSock instance
  static UDPSock* m_Instance;

  //! socket setting structure
  struct sockaddr_in server_addr, client_addr;

  char buff[2048];

 public:
  //! Create socket
  static UDPSock* GetInstance(int type, char *hostname, int port);
  ///
  UDPSock();
  //!
  UDPSock(int,char *,int);
  //!
  ~UDPSock();
  //!
  int Shut();
  ///
  int Peek();
  //! receive (server)
  int Read(char *,int);
  //! send (client)
  int Write(char *,int);
  //! get socket descriptor
  int GetSd();
  //! initialize server socket
  int InitSockServer(char *,int);
  //! initialize slient socket
  int InitSockClient(char *,int);
  //! set socket option
  int SetSockOption(int);
  //! set send socket buffer (server)
  int SetSendBuf(int, int);
  //! set receive socket buffer (client)
  int SetRecvBuf(int, int);
  void SetSock(int sock);

};

#endif // UDP_SOCK__H
