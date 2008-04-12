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

#ifndef __igtlTransformMessage_h
#define __igtlTransformMessage_h

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

namespace igtl
{

class TransformMessage: public MessageBase
{
public:
  typedef TransformMessage               Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::TransformMessage, igtl::Object)
  igtlNewMacro(igtl::TransformMessage);

public:

public:

  void Delete() { delete this; };
  
  void SetPosition(float p[3]);
  void SetPosition(float px, float py, float pz);
  void SetNormals(float o[3][3]);
  void SetNormals(float t[3], float s[3], float n[3]);
  void SetMatrix(Matrix4x4& mat);

protected:
  TransformMessage();
  ~TransformMessage();
  
protected:

  virtual int GetBodyPackSize();
  virtual void PackBody();
  
  Matrix4x4 matrix;

  unsigned char*  m_Transform;

};


} // namespace igtl

#endif // _igtlTransformMessage_h



