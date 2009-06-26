#include "TCPSock.h"

TCPSock* TCPSock::m_Instance = NULL;

TCPSock*
TCPSock::GetInstance(){
  m_Instance = new TCPSock();
  return m_Instance;
}


int
TCPSock::sockServerInit(int portnum) {
    struct sockaddr_in server_addr, client_addr;
    int tmpsock, sock;
    int client_len;

    bzero((char *)&server_addr, sizeof(server_addr)); //clear server_addr
    bzero((char *)&client_addr, sizeof(client_addr)); //clear client_addr

    if ((tmpsock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
      cout << "socket errorr." << endl << flush;
      return(ERROR);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnum);

    if (bind(tmpsock, (SOCKADDR *)&server_addr,sizeof(server_addr)) == ERROR) {
        close(tmpsock);
  cout << "bind error. " << ERROR << flush;
        return(ERROR);
    }

    /* clientからの接続待ちの準備 */
    if (listen(tmpsock, 5) == ERROR) {
        close(tmpsock);
  cout << "listen errorr." << endl << flush;
        //return(ERROR);
    }

    client_len = sizeof(client_addr);

    /* clientからの接続待ち */
    if ((sock = accept(tmpsock, (SOCKADDR *)&client_addr,
           (int *)&client_len)) == ERROR) {
        close(tmpsock);
  cout << "accept errorr." << endl << flush;
        return(ERROR);
    }
    cout << "done." << endl << flush;

    close(tmpsock);

    /* 接続できたsocketの番号を返す */

  this->sd = sock;
    return sock;
}


int
TCPSock::sockClientInit(char *hostname,int portnum) {
    struct sockaddr_in server_addr,client_addr;
    int sock;

    /* 念の為、変数をクリアする */
    bzero((char *)&server_addr, sizeof(server_addr));
    bzero((char *)&client_addr, sizeof(client_addr));

    if (strcmp(hostname,NULL) == 0) {
      cout << "hostname -> " << endl << flush;
  gets(hostname);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnum);
    server_addr.sin_addr.s_addr = htons(hostGetByName(hostname));

    /* socketを作る */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
      cout << "socket error." << endl << flush;
      return(ERROR);
    }

    /* serverとの接続 */
    if (connect(sock, (SOCKADDR *)&server_addr,
                sizeof(server_addr)) == ERROR) {
  close(sock);
  cout << "connect error." << endl << flush;
  return(ERROR);
    }

    /* 接続できたsocketの番号を返す */
  this->sd = sock;
    return sock;
} /* socketClientInitialize */

int
TCPSock::sockSetOption(int sock) {
    int on;
    struct linger lin;

    /*----- 終了時にすぐにポートを解放する -----*/
    on = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

    /*----- 時間遅れなくデータを送る -----*/
    on = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on));

    /*----- 送信用バッファ・サイズを設定 -----*/
    on = SOCK_BUFSIZE;
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&on, sizeof(on));

    /*----- 受信用バッファ・サイズを設定 -----*/
    on = SOCK_BUFSIZE;
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&on, sizeof(on));

    /*----- 終了時にバッファにデータが残っていても捨てる -----*/
    lin.l_onoff = 0; lin.l_linger = 0;
    setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *)&lin, sizeof(lin));

    return sock;
}

TCPSock::TCPSock(){
}

/*
TCPSock::SOCK(int port) {
    this->sd = sockServerInit(port);
    sockSetOption(this->sd);
}

TCPSock::SOCK(char *hostname,int port) {
    this->sd = sockClientInit(hostname,port);
    sockSetOption(this->sd);
}
*/

TCPSock::~TCPSock() {
    shutdown(2,this->sd);
    close(this->sd);
  m_Instance = NULL;
  cout<<"End TCPSock."<<endl;
}

int
TCPSock::Read(char *ptr,int size) {
    int nleft, nread;

    nleft = size;
    while (nleft > 0) {
        nread = recv(this->sd, ptr, nleft,0);
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
TCPSock::Write(char *ptr,int size) {
    int nlefts, nwritten;

    nlefts = size;
    while (nlefts > 0) {
        nwritten = send(this->sd, ptr, nlefts,0);
        if (nwritten <= 0) return(nwritten); /* エラー */
        nlefts -= nwritten;
        ptr    += nwritten;
    }
    return size - nlefts;
}

int
TCPSock::Shut() {
  if ( shutdown(2,this->sd) == ERROR) {
    return(ERROR);
  }
  return(OK);
}

int TCPSock::getSd() {
  return sd;
}

void
TCPSock::SetSock(int sock){
  this->sd = sock;
}

