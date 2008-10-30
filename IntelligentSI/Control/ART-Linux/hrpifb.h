#ifndef HRPIFB_h
#define HRPIFB_h

#ifdef __linux__

#include <stdio.h>
#include <iostream>
extern "C" {
#include "I7sIo.h"
}

class HRPIFB {
 private:
  HRPIFB();
  int DeviceInit(int unitNum);
  int DeviceLookup();
  IB7_PARAINFO  Ib7ParaInfo;
  IB7_PARAAD    Ib7ParaAd;
  IB7_PARADA    Ib7ParaDa;
  IB7_PARAENC   Ib7ParaEnc;
  double ad_buf;



 public:
  ~HRPIFB();
  int board_num;
  static HRPIFB* m_Instance;
  static HRPIFB* getInstance();
        void Write(int uniNum, long *wData);
  void Read(int unitNum, long *readData);
  double ReadData(int unitNum, int ch);
  void Output(int unitNum, double v1, double v2, double v3, double v4);
};
#endif // __linux__
#endif // HRPIFB_h
