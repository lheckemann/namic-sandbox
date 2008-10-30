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

#include "sockUDP.h"

//#include "crc32.h"
#include "common.h"

class SlicerInterface{
 private:
  // SOCK data
  static IF_DATA  IF_Data;
  // IGTLink protocol struct
  static IGTL_PHANTOM writeData;
  
  // Semaphore
  SEM_ID wMutex;
  //sock
  udpSOCK  IF_clientSock;
  
  static void write(void*, void*, void*);
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

