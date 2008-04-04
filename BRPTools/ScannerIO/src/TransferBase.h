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

#ifndef __TRANSFER_BASE_H
#define __TRANSFER_BASE_H

#include "igtlMath.h"
#include "igtlImageMessage.h"
#include "AcquisitionBase.h"
#include "Thread2.h"

class TransferBase : public Thread2
{
public:
  
//  static TransferBase* New();
//  static void Delete();

  virtual int  Init()      {};
  virtual int  Connect()   {};
  virtual int  Disconnect(){};

  int SetAcquisitionThread(AcquisitionBase*);

protected:
  TransferBase();
  virtual ~TransferBase()  {};
  
protected:
  AcquisitionBase* AcquisitionThread;

};


#endif// __TRANSFER_BASE_H






