#ifndef __SCANNER_BASE_H
#define __SCANNER_BASE_H

#include "igtlMath.h"
#include "igtlWin32Header.h"
#include "igtlImageMessage.h"

class IGTLCommon_EXPORT ScannerBase
{
public:
//  static ScannerBase* New();
//  static void Delete();
  
  virtual ::igtl::ImageMessage::Pointer GetCurrentFrame() { return NULL; };

  virtual int Init()   { return 0; };
  virtual int Start()  { return 0; };
  virtual int Pause()  { return 0; };
  virtual int Stop()   { return 0; };
  virtual int SetMatrix(float*) { return 0; };
  
protected:
  ScannerBase();
  virtual ~ScannerBase()  {};
  
protected:

};


#endif// ScannerBase




