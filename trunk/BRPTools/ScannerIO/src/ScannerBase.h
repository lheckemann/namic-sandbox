#ifndef __SCANNER_BASE_H
#define __SCANNER_BASE_H

#include "igtlMath.h"
#include "igtlImage.h"

class ScannerBase
{
public:
//  static ScannerBase* New();
//  static void Delete();
  
  virtual ::igtl::Image::Pointer GetCurrentFrame() {};

  virtual int Init()   {};
  virtual int Start()  {};
  virtual int Pause()  {};
  virtual int Stop()   {};
  virtual int SetMatrix(float*) {};
  
protected:
  ScannerBase();
  virtual ~ScannerBase()  {};
  
protected:

};


#endif// ScannerBase




