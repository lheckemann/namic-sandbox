/***************************************************************************
 * FileName      : joint.h
 * Created       : 2007/08/23
 * LastModified  : 2007/08/27
 * Author        : Hiroaki KOZUKA
 * Aim           : Class for joint's motor control
 ***************************************************************************/
#ifndef JOINT__H
#define JOINT__H

#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <math.h>
#include <string.h>

#ifndef __linux__ //if not __linux__

#include <iostream.h>
#include <strstream.h>
#include <fstream.h>

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
#include <memLib.h>
#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"

#else //if __linux__

#include <iostream>
#include <sstream>
#include <fstream>

#endif //if not __linux__

#include "driver.h"
#include "common.h"


class JOINT{
 private:
  static const int jNum = 3;
  PID_DATA PID;
#ifndef __linux__ //if not __linux__
  static const PID_DEFAULT_DATA PID_D[jNum] = {
    {0.4, 20, 0.002},
    {0.4, 20, 0.002},
    {0.4, 20, 0.002},
/*      //for tool
    {0.13, 15, 0.0012},
    {0.13, 15, 0.0012},
    {0.01, 4, 0},//{0.25, 4, 0.0015},
*/
  };
#else //if __linux__
  static const PID_DEFAULT_DATA PID_D[jNum];
#endif //if not __linux__
  JOINT_DATA jData;
  int  jID;
  double curAngle;
  double speed;
  double PIDCalculate(double destValue, double curValue);
  void jointDataCalculate();
 public:
  DRIVER& deviceDriver;
  ~JOINT();
  JOINT(DRIVER& driver,int jid);
  void angleSet(double angle);
  void speedSet(double speed);
  void jointDataShow(JOINT_DATA* jData);
  JOINT_DATA angleControl(double destAngle);
  double ctrlForce();
};

#endif //JOINT__H

