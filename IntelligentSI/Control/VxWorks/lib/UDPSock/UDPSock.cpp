/******************************************************************************
 * FileName      : UDPSock.cpp
 * Created       :
 * LastModified  :
 * Author        :
 * Aim           : UDP/IP socket class
 *
 * OS            : VxWorks 5.5.1
 *****************************************************************************/

#include "UDPSock.h"

UDPSock* UDPSock::m_Instance = NULL;

//-----------------------------------------------------------------------------
//! Create socket instance. return sock class instance
//! GetInstance( SERVER or CLIENT, "name", port number )
UDPSock*
UDPSock::GetInstance( int type, char *hostname, int port ){
    m_Instance = new UDPSock( type, hostname, port );
    return m_Instance;
}

UDPSock::UDPSock(){

}

//-----------------------------------------------------------------------------
//! Constractor
//! UDPSock( SERVER or CLIENT, "name", port number )
UDPSock::UDPSock(int type, char *hostname,int port) {

  if (type == 1) {
    std::cout << "UDP Sock (Sever) Creating... " << std::flush;
    this->sd = InitSockServer( hostname, port );
    SetSockOption(this->sd);
  }
  else if(type == 2) {
    std::cout << "UDP Sock (Client) Creating... " << std::flush;
    this->sd = InitSockClient( hostname, port );
    SetSockOption(this->sd);
  }
  else {
    std::cout << "udpSock >>> invalid type definition." << std::endl;
  }
}

//-----------------------------------------------------------------------------
//! Destranctor
UDPSock::~UDPSock() {
  m_Instance = NULL;
  shutdown(2,this->sd);
  close(this->sd);
  std::cout<<"End UDPSock."<<std::endl;
}

//-----------------------------------------------------------------------------
//! Intialize server socket. Return socket number
//! InitSockServer("name",port number)
int
UDPSock::InitSockServer(char *hostname,int portnum){
  int udpsock;

  bzero((char *)&server_addr, sizeof(server_addr)); //clear server_addr
  bzero((char *)&client_addr, sizeof(client_addr)); //clear client_addr

  if ((udpsock = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR) {
    std::cout << "socket errorr." << std::endl;
    return(ERROR);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portnum);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(portnum);
  client_addr.sin_addr.s_addr = hostGetByName(hostname);

  if (bind(udpsock, (SOCKADDR *)&server_addr,sizeof(server_addr)) == ERROR) {
    close(udpsock);
    std::cout << "bind error. " << ERROR << std::flush;
    return(ERROR);
  }

  std::cout << "UDP/IP Server Socket Done." << std::endl;

  // Return conneted socket number
  return udpsock;
}

//-----------------------------------------------------------------------------
//! Intialize client socket. Return socket number
//! InitSockClient("name",port number)
int
UDPSock::InitSockClient(char *hostname,int portnum) {

  int udpsock;

  // clear
  bzero((char *)&server_addr, sizeof(server_addr));
  bzero((char *)&client_addr, sizeof(client_addr));

  if (strcmp(hostname,NULL) == 0) {
    std::cout << "hostname -> " << std::endl;
    gets(hostname);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portnum);
  server_addr.sin_addr.s_addr = htons(hostGetByName(hostname));
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(portnum);
  client_addr.sin_addr.s_addr = INADDR_ANY;

  // create socket
  if ((udpsock = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR) {
    std::cout << "socket error." << std::endl;
    return(ERROR);
  }

  std::cout << "Done." << std::endl;

  // Return conneted socket number
  return udpsock;
} // socketClientInitialize

//-----------------------------------------------------------------------------
//! Set socket option. Return socket number
//! SetSockOption( socket number )
int
UDPSock::SetSockOption(int sock) {
  int on;
  //SO_REUSEADDR
  on = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
  return sock;
}

//-----------------------------------------------------------------------------
//! Set socket client buffer. Return OK
//! SetSendBuf(socket number, buffer size)
int
UDPSock::SetSendBuf(int sock, int size) {
  // set send socket buffer size
  setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(size));
  return OK;
}

//-----------------------------------------------------------------------------
//! Set socket server buffer. Return OK
//! SetRecvBuf(socket number, buffer size)
int
UDPSock::SetRecvBuf(int sock, int size) {
  // set receive socket buffer size
  setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size));
  return OK;
}

int
UDPSock::Peek(){
  return recv(this->sd, buff, 2048, MSG_PEEK);
}


//-----------------------------------------------------------------------------
//! Receive data. Return received size
//! Read(data pointer, size)
int
UDPSock::Read(char *ptr,int size) {
  //char buff[1024];
  //int buffSize = recv(this->sd, buff, size, MSG_PEEK);
  int nleft, nread;

  nleft = size;
  int num = sizeof(client_addr);

  while (nleft > 0) {
    nread = recvfrom(this->sd, ptr, nleft, 0, (SOCKADDR *)&client_addr,
         &num );
    if (nread < 0) {
      return(nread);
    } else if (nread == 0) {
      break;         // EOF
    }
    nleft -= nread;
    ptr   += nread;
  }
  return size - nleft;
}

//-----------------------------------------------------------------------------
//! Send data. Return sended size
//! Write(data pointer, size)
int
UDPSock::Write(char *ptr,int size) {
  int nlefts, nwritten;
  nlefts = size;

  while (nlefts > 0) {
    int key = intLock();
    nwritten = sendto(this->sd, ptr, nlefts, 0, (SOCKADDR *)&server_addr,
          sizeof(server_addr));
  intUnlock(key);
    if (nwritten <= 0) return(nwritten); // Error
    nlefts -= nwritten;
    ptr    += nwritten;
  }
  return size - nlefts;
}

//-----------------------------------------------------------------------------
//! Return socket number
int
UDPSock::GetSd(){
  return this->sd;
}

void
UDPSock::SetSock(int sock){
  this->sd = sock;
}

