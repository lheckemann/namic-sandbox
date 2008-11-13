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

#ifndef __igtlPositionMessage_h
#define __igtlPositionMessage_h

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

namespace igtl
{

class IGTLCommon_EXPORT PositionMessage: public MessageBase
{
public:
  enum {
    POSITION_ONLY =  1,
    WITH_QUATERNION3,
    ALL,
  };


public:
  typedef PositionMessage                Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::PositionMessage, igtl::MessageBase);
  igtlNewMacro(igtl::PositionMessage);

public:

  void         Init();

  void         SetPackType(int t); /* POSITION_ONLY / WITH_QUATERNION3 / ALL */

  void         SetPosition(const float* pos);
  void         SetPosition(float x, float y, float z);
  void         SetQuaternion(const float* quat);
  void         SetQuaternion(float ox, float oy, float oz, float w);

  void         GetPosition(float* pos);
  void         GetPosition(float* x, float* y, float* z);
  void         GetQuaternion(float* quat);
  void         GetQuaternion(float* ox, float* oy, float* oz, float* w);

protected:
  PositionMessage();
  ~PositionMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  igtlInt32    m_PackType;
  
  igtlFloat32  m_Position[3];
  igtlFloat32  m_Quaternion[4];

};


} // namespace igtl

#endif // _igtlPositionMessage_h



