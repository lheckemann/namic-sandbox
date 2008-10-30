/***************************************************************************
 * FileName      : IF_MANAGER.h
 * Created       : 2007/11/21
 * LastModified  : 2007/
 * Author        : Hiroaki KOZUKA
 * Aim           : conect to other task
 *                 robot class
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef IF_MANAGER__H
#define IF_MANAGER__H

#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <math.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>

#ifndef __linux__ //if not __linux__

#include <strstream.h>
#include <vxWorks.h>
#include <taskLib.h>
#include <kernelLib.h>
#include <semLib.h>
#include <sysLib.h>
#include <usrLib.h>
#include <msgQLib.h>
#include <logLib.h>
#include <intLib.h>
#include <ioLib.h>
#include <pipeDrv.h>
#include <vxLib.h>
//#include <bLib.h>
#include <memLib.h>

#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"

#else //if __linux__

#include <pthread.h> 
#include <sstream>
#include <stdint.h>

#endif //if not __linux__

#include "PNTM_interface.h"
#include "common.h"
#include "keyConsole.h"
#include "SlicerInterface.h"

#ifdef __linux__ //if __linux__
typedef uint32_t UINT32;
#endif //if __linux__


class IF_MANAGER{
 private:
    //I/F
    static const int IFNum = 3;
    PNTM_INTERFACE*  slave;
  SlicerInterface*  slicer;
    KEYCONSOLE* keyCon;
    
    //semaphore
#ifndef __linux__ //if not __linux__
    SEM_ID exclusMutex;
#else //if __linux__
    pthread_mutex_t exclusMutex;
#endif //if not __linux__

    //struct
  EE_POSITION_S sPee;
  EE_POSITION_S rPee;
    ARM_DATA    armData;
    JOINT_DATA  jointData;
    DRIVER_STAT_DATA driverData;
  EVENT E;
  STATE S;
  //static STATE_MACHINE flag;
    //
    static const int jNum = 3;
    //status flag
    int SSFlag;
    int armFlag;
    int PeeFlag;
    int jointGetFlag;
    int jointOutFlag;
    int driverFlag;
  
 public:
    IF_MANAGER();
    
    void robotDataSend(ARM_DATA*);
    void jointDataSend(JOINT_DATA*);
    void driverDataSend(DRIVER_STAT_DATA*);
    void EEPositionSend(EE_POSITION* PEE);
    int  destEEPositionGet(int jID, EE_POSITION* destPosition);
    void EM_ModeGet();
    void modeChange(int flag, int mode);
  float F_BitSwap(const float* data);
  double W_BitSwap(const double* data);
  int INT_BitSwap(int data);
  int byteSwap(const void* ptr, const void* ptr_, unsigned char size);
  EVENT eventRead(STATE state);
    ~IF_MANAGER();
};

#endif //IF_MANAGER__H

