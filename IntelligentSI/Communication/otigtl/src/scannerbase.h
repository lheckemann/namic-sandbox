#ifndef __SCANNER_BASE_H
#define __SCANNER_BASE_H

#include "igtlMath.h"
#include "igtlImageMessage.h"

class ScannerBase
{
public:
//  static ScannerBase* New();
//  static void Delete();
  
  virtual ::igtl::ImageMessage::Pointer GetCurrentFrame() = 0;

  virtual int Init()   = 0;
  virtual int Start()  = 0;
  virtual int Pause()  = 0;
  virtual int Stop()   = 0;
  virtual int SetMatrix(float*) = 0;
  
protected:
  ScannerBase();
  virtual ~ScannerBase()  {};
  
protected:

};


#endif// ScannerBase




