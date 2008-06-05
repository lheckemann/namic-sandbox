/***************************************************************************
 * FileName      : interface.h
 * Created       : 2007/11/21
 * LastModified  : 2007/
 * Author        : Hiroaki KOZUKA Jumpei Arata NIT
 * Aim           : conect to other task
 *                 robot class
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef INTER__H
#define INTER__H

#include <vxWorks.h>
#include <taskLib.h>
#include <kernelLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <semLib.h>
#include <sysLib.h>
#include <usrLib.h>
#include <msgQLib.h>
#include <logLib.h>
#include <intLib.h>
#include <vxLib.h>
#include <iomanip>
#include <math.h>
#include <memLib.h>
#include <string.h>
#include <iostream>
#include <strstream>
#include <fstream>
#include <sockLib.h>

#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"


//#define BIG__ENDIAN
// choose IGTL/TCP/UDP
//#define IGTL__
//#define TCP_SOCK
#define UDP_SOCK

#if defined IGTL__
#    
#elif defined TCP_SOCK
#include "sock.h"
#elif defined UDP_SOCK
#include "sockUDP.h"
#endif

#include "common.h"

class INTERFACE{
 private:
    // SOCK data
    static IF_DATA  IF_Data;
    // status value
    static int writeStatFlag, readStatFlag;
    // joint number
    static const int jNum = 3;
    // IGTLink protocol struct
    static IGTL_POSITION readData;
    static IGTL_POSITION writeData;
    // Semaphore
    SEM_ID wMutex;
    SEM_ID rMutex;
    SEM_ID exclusMutex;
    // Msg Q
    MSG_Q_ID IF_R_MsgQID;
    //sock
#if defined IGTL__
#    
#elif defined TCP_SOCK
    SOCK  IF_serverSock;
    SOCK  IF_clientSock;
#elif defined UDP_SOCK
    udpSOCK  IF_serverSock;
    udpSOCK  IF_clientSock;
#endif
    //task
    static void read(void*, void*, void*, void*);
    static void write(void*, void*, void*);
    
    INTERFACE();
    static unsigned long long htonll(unsigned long long data);
 public:
    static INTERFACE* IF_Instance;
    static INTERFACE* createInterface(char *myName, char* yourName,
                      int outPort, int inPort);
    void start(int R_W);
    void stop(int R_W);
    void send(EE_POSITION_S*);
    void recieve(EE_POSITION_S*);
    ~INTERFACE();
};

#endif INTER__H

