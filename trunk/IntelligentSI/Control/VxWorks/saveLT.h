/******************************************************************************
 * FileName      : save.cpp
 * Created       : 2007/10/01
 * LastModified  : 2007/10/
 * Author        : hiroaki KOZUKA
 * Aim           : save class for data log
 * OS            : VxWorks 5.5.1
 *****************************************************************************/
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
#include <ioLib.h>
#include <vxLib.h>
#include <iomanip>
#include <math.h>
#include <memLib.h>
#include <string.h>
#include <iostream>
#include <strstream>
#include <fstream>

#include "InterfaceBase.h"


class SAVE : public InterfaceBase {
 private:
  InterfaceBase::IF_MSG_Q_ID IFMsgQID;
  static int fd;
  FILE* fdb;

  unsigned long long SaveNum;
  static unsigned long long iii;
  static unsigned long long T;

  unsigned long long Ts;
  unsigned long long  dataNum;
  int dataVL;
  float** data;
  float * Time;
  char buff[2048];


  void saveBuf( const unsigned long long* time1, const unsigned long long* time2,
                int num, const float* databuf );

  static void savef(void* );

  void Openf(char [], int );
public:
  SAVE(int , double , int , int );
  ~SAVE();
  InterfaceBase::IF_MSG_Q_ID GetMsgQID();

};

#endif //SAVE__H

