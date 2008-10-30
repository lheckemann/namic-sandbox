/***************************************************************************
 * FileName      : robot.h
 * Created       : 2007/09/14
 * LastModified  : 2007/09/1
 * Author        : Hiroaki KOZUKA
 * Aim           : Main program for a Robot Control
                   robot class
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef ROBOT__H
#define ROBOT__H

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <math.h>
#include <string.h>

#ifndef __linux__  // not __linux__

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
#include "ioLib.h"
#include "pipeDrv.h"
#include <vxLib.h>
#include <memLib.h>
#include <wdLib.h>
#include <tickLib.h>

#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"

#else // __linux__

#include <sstream>
#include <linux/art_task.h>  
#include "artlinux-clock.h"

#endif // not __linux__


#include "common.h"
#include "IFmanager.h"
#include "driver.h"
#include "joint.h"
#include "save.h"
#include "frame.h"





class ROBOT{
 private:
    static STATE_MACHINE flag;
    static const int jNum = 3;

#ifndef __linux__  // not __linux__
    WDOG_ID timeLimit;
    SEM_ID timingSem;
#else // __linux__
#endif // not __linux__
    unsigned long long time;
    double destAngle[3];
    double curAngle[3]; 
    double dx, dy, dz;
    int stateSWFlag;
    
    //inner classes
    DRIVER driver;
    IF_MANAGER IF_manager;
    SAVE* save;
    JOINT* joint[jNum];
    FRAME* TF;
    
    //inner structures    
    JOINT_DATA curJoint[jNum];
    EE_POSITION destPee;
    EE_POSITION destPee_1;
    EE_POSITION curPee;
    EE_POSITION errPee;
    ARM_DATA curArm1;

    //
#ifndef __linux__ // not __linux__
    static void timing(void* sem);
#endif // not __linux__
  
    void stateTransition();
    void originSet();
    void destDataGet(int state, int state_1 , unsigned long ctrlTime);
    void jointControl(double* jAngle, double* curjAngle, JOINT_DATA* jData);
    void invKinematicCalculate(EE_POSITION* destPee, double* jAngle);
    void kinematicCalculate(double* angle, EE_POSITION* Pee);
    void armDataCalculate(EE_POSITION* Pee, ARM_DATA* armData);
    void degCompute(double* angle, double endtime, double Ts, double A, int mode);
    void dataDisp(int Ts);
    void bilateralErrCaluculate(EE_POSITION* dest, EE_POSITION* cur, EE_POSITION*  err);
    void armCtrl(int state, int state_1);
    void timeOutErr();
    void init();
    void robotMain();
 public:
    ROBOT();
    ~ROBOT();
};

#endif //ROBOT__H
