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
#include "igtlImageMessage.h"
#include "Thread2.h"

class AcquisitionBase : public Thread2
{
public:
  
//  static AcquisitionBase* New();
//  static void Delete();

  virtual int  Init()      {};
  virtual int  StartScan() {};
  virtual int  PauseScan() {};
  virtual int  StopScan() {};
  virtual void Process() {};
  virtual int  SetMatrix(float* matrix){};

  void SetPostProcessThread(Thread2* thread);
  igtl::ImageMessage::Pointer GetFrameFromBuffer(int id);

protected:
  AcquisitionBase();
  virtual ~AcquisitionBase()  {};

  // Circular Buffer manipulation
  int SetCircularFrameBufferSize(int s);
  int GetCircularFrameBufferSize();
  int PutFrameToBuffer(igtl::ImageMessage::Pointer frame);
  int PutFrameToBuffer(int id, igtl::ImageMessage::Pointer frame);

protected:

  Thread2* PostProcessThread;

  // Circular Buffer manipulation
  std::vector<igtl::ImageMessage::Pointer> CircularFrameBuffer;
  int CurrentFrameId;

};


#endif// __ACQUISITION_BASE_H


