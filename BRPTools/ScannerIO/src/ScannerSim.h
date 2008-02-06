#ifndef __SCANNER_SIM_H
#define __SCANNER_SIM_H

#include "ScannerBase.h"

class ScannerSim : public ScannerBase
{
public:

public:

  virtual igtl::Image* GetCurrentFrame();
  virtual int Init();
  virtual int Start();
  virtual int Pause();
  virtual int Stop();
  virtual int SetMatrix(float* matrix);

  int LoadImageData(char* fnameTemp, int bindex, int eindex,
                    int scalarType, int size[3], float spacing[3]);

  int DeleteImages();

  void Delete();

protected:
  ScannerSim();
  virtual ~ScannerSim();
  
protected:

  int numImages;
  igtl::Image** image;

};


#endif // __SCANNER_SIM_H
