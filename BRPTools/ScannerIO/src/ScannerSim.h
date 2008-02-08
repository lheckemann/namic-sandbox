#ifndef __SCANNER_SIM_H
#define __SCANNER_SIM_H

#include "ScannerBase.h"
#include "igtlImage.h"

#include <vector>

class ScannerSim: public ScannerBase
{
public:

  virtual igtl::Image::Pointer GetCurrentFrame();
  virtual int Init();
  virtual int Start();
  virtual int Pause();
  virtual int Stop();
  virtual int SetMatrix(float* matrix);

  int LoadImageData(char* fnameTemp, int bindex, int eindex,
                    int scalarType, int size[3], float spacing[3]);

  int DeleteImages();
  void Delete();

public:
  ScannerSim();

protected:
  virtual ~ScannerSim();
  
protected:

  int currentFrame;
  std::vector<igtl::Image::Pointer> imageArray;

};


#endif // __SCANNER_SIM_H
