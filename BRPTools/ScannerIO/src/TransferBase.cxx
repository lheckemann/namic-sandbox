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

#include "TransferBase.h"

TransferBase::TransferBase()
{
}


int TransferBase::SetAcquisitionThread(AcquisitionBase* acquisition)
{
  this->AcquisitionThread = acquisition;
}
/*
virtual int  Init()      {};
virtual int  Connect()   {};
virtual int  Disconnect(){};
*/





