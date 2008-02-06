#ifndef __SCANNER_BASE_H
#define __SCANNER_BASE_H

#include "igtlImage.h"
#include "igtlMath.h"

class ScannerBase
{
public:

public:
//  static ScannerBase* New();
//  static void Delete();

  virtual igtl::Image* GetCurrentFrame() {};

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


#endif __IMAGE_H




