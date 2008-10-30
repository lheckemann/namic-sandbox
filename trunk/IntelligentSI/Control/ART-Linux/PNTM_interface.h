/***************************************************************************
 * FileName      : PNTM_interface.h
 * Created       : 2008/2/5
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : conect other device
 *                 
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef PNTM_INTER__H
#define PNTM_INTER__H

#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <math.h>
#include <string.h>
#include <iostream>
#include <fstream>

#ifndef __linux__ //if not __linux__

#include <strstream>
#include <vxWorks.h>
#include <taskLib.h>
#include <kernelLib.h>
#include <semLib.h>
#include <sysLib.h>
#include <usrLib.h>
#include <msgQLib.h>
#include <logLib.h>
#include <intLib.h>
#include <vxLib.h>
#include <memLib.h>
#include <sockLib.h>

#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"

#else  //if __linux__

#include <pthread.h> 
#include <sstream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <unistd.h> 
#include <cerrno> 

#endif //if not __linux__


//#define BIG__ENDIAN

/*-------------------------*/
//choose IGTL/TCP/UDP
//#define IGTL__
//#define TCP_SOCK
#define UDP_SOCK

#define IGTL__HEADER
/*-------------------------*/
#if defined IGTL__

#elif defined TCP_SOCK
#include "sock.h"
#elif defined UDP_SOCK
#include "sockUDP.h"
#endif

//#include "crc32.h"
#include "common.h"

//extern "C" 


#ifdef __linux__ //if __linux__
#define KEY_PATHNAME "/tmp"    //for the message queue; 
#define KEY_PROJID   (char)'R'

typedef struct {
  long mtype;
  char mtext[1];
}sv_msgbuf;

typedef struct{
  int *msgQid;
#if defined IGTL__
#    
#elif defined TCP_SOCK
  SOCK *sock;         //TODO: confirm "sock.h" and SOCK
#elif defined UDP_SOCK
  udpSOCK *sock; 
#endif
  pthread_mutex_t *xMutex;
  sem_t *xSem;
}SockThreadData;  //a structure to hold the arguments to the read/write pthread functions
#endif //if __linux__



class PNTM_INTERFACE{
 private:
  // SOCK data
  static IF_DATA  IF_Data;
  // status value
  static int writeStatFlag, readStatFlag;
  // joint number
  static const int jNum = 3;
  // IGTLink protocol struct
#if defined IGTL__HEADER
  static IGTL_PHANTOM readData;
  static IGTL_PHANTOM writeData;
#else
  static MASTER_PHANTOM_DATA readData;
  static MASTER_PHANTOM_DATA writeData;
#endif
  //sock
#if defined IGTL__
  
#elif defined TCP_SOCK
  SOCK  IF_serverSock;
  SOCK  IF_clientSock;
#elif defined UDP_SOCK
  udpSOCK  IF_serverSock;
  udpSOCK  IF_clientSock;
#endif

#ifndef __linux__ //if not __linux__
  // Semaphore
  SEM_ID wMutex;
  SEM_ID Mutex;
  SEM_ID exclusMutex;
  // Msg Q
  MSG_Q_ID IF_R_MsgQID;
  static void read(void*, void*, void*);
  static void write(void*, void*, void*);
#else //if __linux__
        pthread_mutex_t wMutex;
        sem_t wSem;
        int IF_R_MsgQID;
        pthread_mutex_t Mutex;
        pthread_mutex_t exclusMutex;
        pthread_t tIFInSockThread;
        pthread_t tIFOutSockThread;
        SockThreadData inSockThreadData;
        SockThreadData outSockThreadData;
        sv_msgbuf *r_msgbuff; //for receive()
        static void *read(void*);
        static void *write(void*);
#endif //if not __linux__

  static EVENT stateFlag;
  static unsigned long long htonll(unsigned long long data);
  PNTM_INTERFACE();
  float W_BitSwap( const float* ptr);
 public:
  static PNTM_INTERFACE* IF_Instance;
  static PNTM_INTERFACE* createInterface(char *myName, char* yourName,
                 int outPort, int inPort);
  //void start(int R_W);
  //void stop(int R_W);
  void send(EE_POSITION_S*);
  void recieve(EE_POSITION_S*);
  EVENT eventRead();
  ~PNTM_INTERFACE();
};

#endif //PNTM_NTER__H

