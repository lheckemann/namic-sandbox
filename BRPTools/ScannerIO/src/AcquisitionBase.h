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

#ifndef __ACQUISITION_BASE_H
#define __ACQUISITION_BASE_H

#include <vector>

#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlWin32Header.h"

#include "Thread.h"

class IGTLCommon_EXPORT AcquisitionBase : public Thread
{
public:
  
//  static AcquisitionBase* New();
//  static void Delete();

  virtual int  Init()      { return 0; };
  virtual int  StartScan() { return 0; };
  virtual int  PauseScan() { return 0; };
  virtual int  StopScan()  { return 0; };
  virtual void Process()   {};
  virtual int  SetMatrix(float* matrix){ return 0; };
  virtual int  SetMatrix(igtl::Matrix4x4& m) { return 0; }

  void SetPostProcessThread(Thread* thread);
  igtl::MessageBase::Pointer GetFrameFromBuffer(int id);

protected:
  AcquisitionBase();
  virtual ~AcquisitionBase()  {};

  // Circular Buffer manipulation
  int SetCircularFrameBufferSize(int s);
  int GetCircularFrameBufferSize();
  int PutFrameToBuffer(igtl::MessageBase::Pointer frame);
  int PutFrameToBuffer(int id, igtl::MessageBase::Pointer frame);

protected:

  Thread* PostProcessThread;

  // Circular Buffer manipulation
  std::vector<igtl::MessageBase::Pointer> CircularFrameBuffer;
  int CurrentFrameId;

};


#endif// __ACQUISITION_BASE_H


