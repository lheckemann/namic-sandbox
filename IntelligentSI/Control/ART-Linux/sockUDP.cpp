
#include "sockUDP.h"

// Usage ==== Example
//  Server Type : 
//   sock_m2s = new udpSOCK(1,"teal", 10000);
//  Client Type : 
//   sock_m2s = new udpSOCK(2,"rail", 10000);

#ifdef __linux__ //if __linux__
using namespace std;
#endif //if not __linux__

int
udpSOCK::sockServerInit(char *hostname,int portnum) {
  
  int udpsock;
  
#ifndef __linux__  //if not __linux__
  bzero((char *)&server_addr, sizeof(server_addr)); //clear server_addr
  bzero((char *)&client_addr, sizeof(client_addr)); //clear client_addr
  
  if ((udpsock = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR) {
    cout << "socket errorr." << endl << flush;
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
    cout << "bind error. " << ERROR << flush;
    return(ERROR);
  }
#else //if __linux__

  memset((void *)&server_addr, '\0', sizeof(server_addr)); //clear server_addr
  memset((void *)&client_addr, '\0', sizeof(client_addr)); //clear client_addr

  struct hostent *hostInfo;
  if ((udpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    cout << "socket error." << endl << flush;
    return(ERROR);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portnum);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(portnum);
  if ((hostInfo = gethostbyname(hostname)) == NULL) {
    cerr << "\nsockServerInit: ERROR: host with name " << hostname << " not found." << endl;
    exit(1); //TODO: handle properly
  }
  memcpy(&client_addr.sin_addr.s_addr, hostInfo->h_addr, hostInfo->h_length);  

  if (bind(udpsock, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
    close(udpsock);
    cout << "bind error. " << strerror(errno) << flush; 
    return(ERROR);
  }
  
#endif  //if not __linux__
  
  cout << "Done." << endl << flush;
  
  /* return the connected socket number */
  return udpsock;
}


int
udpSOCK::sockClientInit(char *hostname,int portnum) {
  
  int udpsock;
  
  /* clear the variables (just in case) */
  bzero((char *)&server_addr, sizeof(server_addr));
  bzero((char *)&client_addr, sizeof(client_addr));
  
#ifndef __linux__  //if not __linux__
  if (strcmp(hostname,NULL) == 0) {
    cout << "hostname -> " << endl << flush;
    gets(hostname);
  }
  
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portnum);
  server_addr.sin_addr.s_addr = hostGetByName(hostname);
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(portnum);
  client_addr.sin_addr.s_addr = INADDR_ANY;
  
  /* create a socket */ 
  if ((udpsock = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR) {
    cout << "socket error." << endl << flush;
    return(ERROR);
  }
#else //if __linux__
  struct hostent *hostInfo;
  if (hostname == NULL) {
    cout << "hostname -> " << endl << flush;
    fgets(hostname, sizeof(hostname), stdin);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portnum);
  if ((hostInfo = gethostbyname(hostname)) == NULL) {
    cerr << "\nsockClientInit: ERROR: host with name " << hostname << " not found." << endl;
    exit(1); //TODO: handle properly
  }
  memcpy(&server_addr.sin_addr.s_addr, hostInfo->h_addr, hostInfo->h_length);  
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(portnum);
  client_addr.sin_addr.s_addr = INADDR_ANY;

  /* create a socket */ 
  if ((udpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    cout << "socket error." << endl << flush;
    return(ERROR);
  }
#endif  //if not __linux__

  cout << "Done." << endl << flush;
  
  /* return the connected socket number */
  return udpsock;
} /* socketClientInitialize */


int
udpSOCK::sockSetOption(int sock) {
  int on;
  /*----- SO_REUSEADDR -----*/
  on = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
  return sock;
}

int
udpSOCK::setSendBuf(int sock, int size) {
  /*----- 送信用バッファ・サイズを設定 -----*/
  setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(size));
  return OK;
}
int
udpSOCK::setRecvBuf(int sock, int size) {
  /*----- 受信用バッファ・サイズを設定 -----*/
  setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size));
  return OK;
}

udpSOCK::udpSOCK(int type, char *hostname,int port) {
  
  if (type == 1) {
    cout << "UDP Sock (Sever) Creating... " << flush;
    this->sd = sockServerInit(hostname,port);
    sockSetOption(this->sd);
  }
  else if(type == 2) {
    cout << "UDP Sock (Client) Creating... " << flush;
    this->sd = sockClientInit(hostname,port);
    //sockSetOption(this->sd);
  }
  else {
    cout << "udpSock >>> invalid type definition." << endl << flush;
  }
}


udpSOCK::~udpSOCK() {
#ifndef __linux__  //if not __linux__
  shutdown(2,this->sd);
#else //if  __linux__
  shutdown(this->sd, SHUT_RDWR);
#endif //if not __linux__
  close(this->sd);
}

int
udpSOCK::Read(char *ptr,int size) {
#ifndef __linux__  //if not __linux__
  int nleft, nread,num;
#else //if  __linux__
  int nleft;
  ssize_t nread;
  socklen_t num;
#endif //if not __linux__
  nleft = size;
  while (nleft > 0) {
    num = sizeof(client_addr);
#ifndef __linux__  //if not __linux__
    nread = recvfrom(this->sd, ptr, nleft, 0, (SOCKADDR *)&client_addr,
         &num);
#else //if  __linux__
    nread = recvfrom(this->sd, ptr, nleft, 0, (struct sockaddr *)&client_addr, &num);
#endif //if not __linux__

    if (nread < 0) {
      return(nread);
    } else if (nread == 0) {
      break;         /* EOF */
    }
    nleft -= nread;
    ptr   += nread;
  }
  return size - nleft;
}

int
udpSOCK::Write(char *ptr,int size) {
  int nlefts, nwritten;
  
  nlefts = size;
  while (nlefts > 0) {
#ifndef __linux__  //if not __linux__
    nwritten = sendto(this->sd, ptr, nlefts,0,(SOCKADDR *)&server_addr,
          sizeof(server_addr));
#else //if  __linux__
    nwritten = sendto(this->sd, ptr, nlefts,0,(struct sockaddr *)&server_addr,
          sizeof(server_addr));
#endif //if not __linux__
    if (nwritten <= 0) return(nwritten); /* error */
    nlefts -= nwritten;
    ptr    += nwritten;
  }
  return size - nlefts;
}

int
udpSOCK::getSd(){
  return sd;
}
