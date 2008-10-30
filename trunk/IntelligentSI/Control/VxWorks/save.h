/***************************************************************************
 * FileName      : save.cpp
 * Created       : 2007/10/01
 * LastModified  : 2007/10/
 * Author        : hiroaki KOZUKA
 * Aim           : save class for data log
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef SAVE__H
#define SAVE__H

#include <vxWorks.h>
#include <taskLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <semLib.h>
#include <sysLib.h>
#include <logLib.h>
#include <intLib.h>
#include "ioLib.h"
#include "pipeDrv.h"
#include <vxLib.h>
#include <iomanip>
#include <math.h>
#include <memLib.h>
#include <string.h>
#include <iostream>
#include <strstream>
#include <fstream>
#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"

class SAVE {
 private:
  double* data1;
  double* data2;
  double* data3;
  double* data4;
  static int fd;
        int dataLength;
        int pointNum;
  int dataNum;
  int dataVL;
 public:
  SAVE(int timeNum);
  ~SAVE();
  void saveBuf(int time, double databuf1, double databuf2,
               double databuf3,double databuf4, double databuf5);
  void savef();
};

#endif SAVE__H
