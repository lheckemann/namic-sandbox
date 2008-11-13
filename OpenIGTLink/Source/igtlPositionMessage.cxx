/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlPositionMessage.h"

#include "igtl_header.h"
#include "igtl_position.h"

#include <string.h>

namespace igtl {

PositionMessage::PositionMessage():
  MessageBase()
{
  Init();
}


PositionMessage::~PositionMessage()
{
}


void PositionMessage::Init()
{
  this->m_PackType      = ALL;

  this->m_Position[0]   = 0.0;
  this->m_Position[1]   = 0.0;
  this->m_Position[2]   = 0.0;

  this->m_Quaternion[0] = 0.0;
  this->m_Quaternion[1] = 0.0;
  this->m_Quaternion[2] = 0.0;
  this->m_Quaternion[3] = 1.0;
}


void PositionMessage::SetPackType(int t)
{
  if (t >= POSITION_ONLY && t <= ALL)
    {
    this->m_PackType = t;
    }
}


void PositionMessage::SetPosition(const float* pos)
{
  this->m_Position[0] = pos[0];
  this->m_Position[1] = pos[1];
  this->m_Position[2] = pos[2];
}


void PositionMessage::SetPosition(float x, float y, float z)
{
  this->m_Position[0] = x;
  this->m_Position[1] = y;
  this->m_Position[2] = z;
}


void PositionMessage::SetQuaternion(const float* quat)
{
  this->m_Quaternion[0] = quat[0];
  this->m_Quaternion[1] = quat[1];
  this->m_Quaternion[2] = quat[2];
  this->m_Quaternion[3] = quat[3];
}


void PositionMessage::SetQuaternion(float ox, float oy, float oz, float w)
{
  this->m_Quaternion[0] = ox;
  this->m_Quaternion[1] = oy;
  this->m_Quaternion[2] = oz;
  this->m_Quaternion[3] = w;
}


void PositionMessage::GetPosition(float* pos)
{
  pos[0] = this->m_Position[0];
  pos[1] = this->m_Position[1];
  pos[2] = this->m_Position[2];
}


void PositionMessage::GetPosition(float* x, float* y, float* z)
{
  *x = this->m_Position[0];
  *y = this->m_Position[1];
  *z = this->m_Position[2];
}


void PositionMessage::GetQuaternion(float* quat)
{
  quat[0] = this->m_Quaternion[0];
  quat[1] = this->m_Quaternion[1];
  quat[2] = this->m_Quaternion[2];
  quat[3] = this->m_Quaternion[3];
}


void PositionMessage::GetQuaternion(float* ox, float* oy, float* oz, float* w)
{
  *ox = this->m_Quaternion[0];
  *oy = this->m_Quaternion[1];
  *oz = this->m_Quaternion[2];
  *w  = this->m_Quaternion[3];
}


int PositionMessage::GetBodyPackSize()
{
  int ret;

  switch (this->m_PackType)
    {
    case POSITION_ONLY:
      ret = IGTL_POSITION_MESSAGE_POSITON_ONLY_SIZE;
      break;
    case WITH_QUATERNION3:
      ret = IGTL_POSITION_MESSAGE_WITH_QUATERNION3_SIZE;
      break;
    default:
      ret = IGTL_POSITION_MESSAGE_DEFAULT_SIZE;
    }

  return ret;

}


int PositionMessage::PackBody()
{
  // allocate pack
  AllocatePack();

  igtl_position* p = (igtl_position*)this->m_Body;

  p->position[0]   = this->m_Position[0];
  p->position[1]   = this->m_Position[1];
  p->position[2]   = this->m_Position[2];

  p->quaternion[0] = this->m_Quaternion[0];
  p->quaternion[1] = this->m_Quaternion[1];
  p->quaternion[2] = this->m_Quaternion[2];
  p->quaternion[3] = this->m_Quaternion[3];

  igtl_position_convert_byte_order(p);

  return 1;
}

int PositionMessage::UnpackBody()
{
  
  igtl_position* p = (igtl_position*)this->m_Body;
  
  igtl_position_convert_byte_order(p);

  this->m_Position[0]   = p->position[0];
  this->m_Position[1]   = p->position[1];
  this->m_Position[2]   = p->position[2];
  this->m_Quaternion[0] = p->quaternion[0];
  this->m_Quaternion[1] = p->quaternion[1];
  this->m_Quaternion[2] = p->quaternion[2];
  this->m_Quaternion[3] = p->quaternion[3];

  return 1;

}

} // namespace igtl





