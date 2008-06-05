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

#ifndef __AcquisitionTrackingSimulator_H
#define __AcquisitionTrackingSimulator_H

#include <vector>
#include <string>

#include "igtlWin32Header.h"
#include "AcquisitionBase.h"


class IGTLCommon_EXPORT AcquisitionTrackingSimulator: public AcquisitionBase
{
public:

  virtual int Init();
  int StartScan();
  int PauseScan();
  int StopScan();
  int SetMatrix(float* matrix);

  // Simulator specific functions
  void SetFrameRate(float fps);
  void  SetDeviceName(const char* name);


public:
  AcquisitionTrackingSimulator();

protected:
  virtual ~AcquisitionTrackingSimulator();
  virtual void Process();

  void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

protected:

  Thread* AcquisitionThread;
  Thread* TransferThread;

  int Interval_ms;
  std::string DeviceName;

};


#endif // __AcquisitionTrackingSimulator_H


