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
#include <semLib.h>
#include <sysLib.h>
#include <logLib.h>
#include <intLib.h>
#include "ioLib.h"
#include "pipeDrv.h"
#include <vxLib.h>
#include <memLib.h>
#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"

#else //if __linux__

#include <sstream>
using namespace std;

#endif //if not __linux__

class SAVE {
 private:
  double* data1;
  double* data2;
  double* data3;
  double* data4;
#ifndef __linux__ //if not __linux__
  static int fd;
#else  //if __linux__
  static ofstream ofs;
#endif //if not __linux__
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

#endif //SAVE__H
