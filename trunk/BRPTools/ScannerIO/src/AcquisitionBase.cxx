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

#include "AcquisitionBase.h"


AcquisitionBase::AcquisitionBase()
{
  this->PostProcessThread = NULL;
  this->CircularFrameBuffer.clear();
  this->CurrentFrameId = -1;
}

void AcquisitionBase::SetPostProcessThread(Thread* thread)
{
  this->PostProcessThread = thread;
}

int AcquisitionBase::SetCircularFrameBufferSize(int s)
{
  if (s <= 0)
    {
      return 0;
    }

  if (this->CircularFrameBuffer.size() != s)
    {
      // To resize, erase all elements in the buffer
      std::vector<igtl::MessageBase::Pointer>::iterator iter;
      for (iter = this->CircularFrameBuffer.begin();
           iter != this->CircularFrameBuffer.end(); iter ++)
        {
          (*iter)->Delete();
        }
      this->CircularFrameBuffer.clear();

      // Allocate new elements
      for (int i = 0; i < s; i ++) {
        this->CircularFrameBuffer.push_back(igtl::MessageBase::New());
      }
    }
  this->CurrentFrameId = -1;
  return this->GetCircularFrameBufferSize();
}

int AcquisitionBase::GetCircularFrameBufferSize()
{
  return this->CircularFrameBuffer.size();
}

int AcquisitionBase::PutFrameToBuffer(igtl::MessageBase::Pointer frame)
{
  std::cerr << "GetCircularFrameBufferSize() = " << this->GetCircularFrameBufferSize() << std::endl;
  std::cerr << "CircularFrameId = " << this->CurrentFrameId<< std::endl;
  int nextId = (this->CurrentFrameId + 1) % this->GetCircularFrameBufferSize();
  this->CircularFrameBuffer[nextId] = frame;
  this->CurrentFrameId = nextId;

  return nextId;
}

int AcquisitionBase::PutFrameToBuffer(int id, igtl::MessageBase::Pointer frame)
{
  if (id >= 0 && id < this->GetCircularFrameBufferSize())
    {
      this->CircularFrameBuffer[id] = frame;
    }
  else
    {
      return 0;
    }
}


igtl::MessageBase::Pointer AcquisitionBase::GetFrameFromBuffer(int id)
{
  if (id >= 0 && id < this->GetCircularFrameBufferSize())
    {
      return this->CircularFrameBuffer[id];
    }
  else
    {
      return NULL;
    }
}


