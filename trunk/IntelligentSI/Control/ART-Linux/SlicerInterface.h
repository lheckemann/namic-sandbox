/***************************************************************************
 * FileName      : SlicerInterface.h
 * Created       : 2008/7/21
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : conect other device
 *                 
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef SLICER_INTER__H
#define SLICER_INTER__H

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
#include <semaphore.h>
#include <unistd.h> 

#endif //if not __linux__

#include "sockUDP.h"

//#include "crc32.h"
#include "common.h"



#ifdef __linux__ //if __linux__

typedef struct{
  //int *msgQid; // not used here
  udpSOCK *sock; 
  pthread_mutex_t *xMutex;
  sem_t *xSem;
}SlicerThreadData;  //a structure to hold the arguments to the write pthread function

#endif //if __linux__


class SlicerInterface{
 private:
  // SOCK data
  static IF_DATA  IF_Data;
  // IGTLink protocol struct
  static IGTL_PHANTOM writeData;
  
  // Semaphore
#ifndef __linux__ //if not __linux__
  SEM_ID wMutex;
#else  //if __linux__
        pthread_mutex_t wMutex;
        sem_t wSem;
        SlicerThreadData outSlicerThreadData;
        pthread_t tIFOutSlicerThread;
#endif //if not __linux__
  //sock
  udpSOCK  IF_clientSock;
  
#ifndef __linux__ //if not __linux__
  static void write(void*, void*, void*);
#else  //if __linux__
  static void *write(void*);
#endif //if not __linux__
  SlicerInterface();
  float W_BitSwap( const float* ptr);
  
 public:
  static SlicerInterface* IF_Instance;
  static SlicerInterface* createInterface(char *myName, char* yourName,
            int outPort, int inPort);
  void send(EE_POSITION_S*);
  ~SlicerInterface();
};

#endif
 //SLICER_INTER__H

