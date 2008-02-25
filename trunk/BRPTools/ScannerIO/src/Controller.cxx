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

#include "Controller.h"

Controller::Controller()
{
  this->AcquisitionThread = NULL;
  this->TransferThread    = NULL;

  this->Running = false;
}

int Controller::Init()
{
  this->Running = false;
}

int Controller::SetAcquisition(Thread* thread)
{
  if (!this->Running)
    {
      this->AcquisitionThread = thread;
      return thread;
    }
  else
    {
      0;
    }
}

int Controller::SetTransfer(Thread* thread)
{
  if (!this->Running)
    {
      this->TransferThread = thread;
      return thread;
    }
  else
    {
      return 0;
    }
}

int Controller::Start()
{
  if (this->AcquisitionThread == NULL ||
      this->TransferThread == NULL)
    {
      return 0;
    }
  
  return 1;
}

int Controller::Stop()
{
  if (this->Running)
    {
      this->AcquisitionThread->Stop();
      this->TransferThread->Stop();
      return 1;
    }
  else
    {
      return 0;
    }
}






