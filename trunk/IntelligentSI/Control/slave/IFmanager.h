/***************************************************************************
 * FileName      : IF_MANAGER.h
 * Created       : 2007/11/21
 * LastModified  : 2007/
 * Author        : Hiroaki KOZUKA Jumpei Arata
 * Aim           : conect to other task
 *                 robot class
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef IF_MANAGER__H
#define IF_MANAGER__H

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
#include <ioLib.h>
#include <pipeDrv.h>
#include <vxLib.h>
#include <iomanip>
#include <math.h>
//#include <bLib.h>
#include <memLib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>
#include <fstream.h>

#include "PNTM_interface.h"
#include "common.h"
#include "keyConsole.h"
#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"


class IF_MANAGER{
 private:
    //I/F
    static const int IFNum = 3;
    PNTM_INTERFACE*  slave;
    KEYCONSOLE* keyCon;
    
    //semaphore
    SEM_ID exclusMutex;

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
    static const int E_EMERGENCY_0      = 0;
    static const int E_EMERGENCY_1      = 1;
    static const int E_EMERGENCY_2      = 2;
    static const int E_EXIT             = 100;
    static const int E_PRGM_START       = 101;
    static const int E_PRGM_STOP        = 102;
    static const int E_CTRL_STOP_START  = 103;
    static const int E_CTRL_STOP_STOP   = 104;
    static const int E_CTRL_RUN_START   = 105;
    static const int E_CTRL_RUN_STOP    = 106;
    
 public:
    IF_MANAGER();
    
    void robotDataSend(ARM_DATA*);
    void jointDataSend(JOINT_DATA*);
    void driverDataSend(DRIVER_STAT_DATA*);
    void errEEPositionSend(EE_POSITION* cur);
    int  destEEPositionGet(int jID, EE_POSITION* destPosition);
    void EM_ModeGet();
    void modeChange(int flag, int mode);
    float W_BitSwap(const float* data);
    int    INT_BitSwap(int data);
    int byteSwap(const void* ptr, const void* ptr_, unsigned char size);
    int stateRead(int state);
    ~IF_MANAGER();
};

#endif IF_MANAGER__H

