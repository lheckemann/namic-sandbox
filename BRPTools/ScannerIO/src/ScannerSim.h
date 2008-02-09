#ifndef __SCANNER_SIM_H
#define __SCANNER_SIM_H

#include "ScannerBase.h"
#include "igtlImageMessage.h"

#include <vector>

class ScannerSim: public ScannerBase
{
public:

  virtual igtl::ImageMessage::Pointer GetCurrentFrame();
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
  std::vector<igtl::ImageMessage::Pointer> imageArray;

};


#endif // __SCANNER_SIM_H
