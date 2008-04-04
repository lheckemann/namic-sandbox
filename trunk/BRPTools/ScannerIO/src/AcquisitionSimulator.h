/*=========================================================================

  Program:   Imaging Scanner Interface
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __AcquisitionSimulator_H
#define __AcquisitionSimulator_H

#include <vector>

#include "igtlImageMessage.h"
#include "AcquisitionBase.h"

class AcquisitionSimulator: public AcquisitionBase
{
public:

  virtual int Init();
  int StartScan();
  int PauseScan();
  int StopScan();
  int SetMatrix(float* matrix);

  // Simulator specific functions
  void SetFrameRate(float fps);
  int  LoadImageData(char* fnameTemp, int bindex, int eindex,
                     int scalarType, int size[3], float spacing[3]);
  int  SetSubVolumeDimension(int dim[3]);

  int  DeleteImages();

public:
  AcquisitionSimulator();

protected:
  virtual ~AcquisitionSimulator();
  virtual void Process();

  void GetRandomTestMatrix(igtl::Matrix4x4& matrix);
  //igtl::ImageMessage::Pointer GetCurrentFrame();
  void GetCurrentFrame(igtl::ImageMessage::Pointer& cf);

protected:

  int CurrentFrameIndex;
  int CurrentFrameSliceIndex;
  igtl::ImageMessage::Pointer CurrentFrame;
  std::vector<igtl::ImageMessage::Pointer> imageArray;

  int SubVolumeDimension[3];

  Thread2* AcquisitionThread;
  Thread2* TransferThread;

  int Interval_ms;

};


#endif // __AcquisitionSimulator_H


