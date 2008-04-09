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

#ifndef __Controller_h
#define __Controller_h

#include "igtlWin32Header.h"
#include "igtlMath.h"
#include "igtlImageMessage.h"
#include "Thread.h"

class IGTLCommon_EXPORT Controller
{
public:

  int Init();
  int Start();
  int SetAcquisition(Thread*);
  int SetTransfer(Thread*);
  
protected:
  Controller();
  virtual ~Controller()  {};
  
protected:

  bool    Running;

  Thread* AcquisitionThread;
  Thread* TransferThread;

};


#endif// ifdef __Controller_h

