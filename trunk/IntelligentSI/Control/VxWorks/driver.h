/***************************************************************************
 * FileName      : driver.h
 * Created       : 2007/08/23
 * LastModified  : 2007/10/
 * Author        : Hiroaki KOZUKA
 * Aim           : driver class for a motor control
 ***************************************************************************/

#ifndef DRIVER__H
#define DRIVER__H

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
#include "ioLib.h"
#include "pipeDrv.h"
#include <vxLib.h>
#include <iomanip>
#include <math.h>
#include <memLib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>
#include <fstream.h>
#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"

#define VX__DRIVER
//#define ART__DRIVER

#if defined VX__DRIVER
#include "acp420.h"
#include "acp550.h"
#include "acp560.h"
#elif defined ART__DRIVER
    
#endif

#include "common.h"

class DRIVER{
 private:
#if defined VX__DRIVER
  ACP420 *CNT_D;
  ACP550 *AD_D;
  ACP560 *DA_D;
#elif defined ART__DRIVER
  
#endif
  static const int jNum = 3;
  static const HARDWARE_DATA_ HW[jNum] ={
    {10, -10, 0, 1, revl, minus, 67548},
    {10, -10, 0, 1, revl, minus, 67548},
    {10, -10, 0, 1, revl, plus, 23552},
  };
  
  double rAngle[jNum]; //! Output Enc data
  double wAngle[jNum]; //! Input Enc data
  double rForce[jNum]; //! A/D volt data
  double wVolt[jNum];  //! D/A volt data
 public:
  DRIVER();
  ~DRIVER();
  void angleReadWrite(int R_W);
  void speedWrite();
  void stop();
  void angleZeroSet();
  double converSpeed2Volt(int jid, double volt);
  double angleGet(int jid);
  //double getForce(int jid);
  void speedSet(int jid ,double speedData );
  void angleSet(int jid, double setAngle);
  double directionDataShow(int jid);
};



#endif DRIVER__H

