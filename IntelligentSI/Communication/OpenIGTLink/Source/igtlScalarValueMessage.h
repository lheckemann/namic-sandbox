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

#ifndef __igtlScalarValueMessage_h
#define __igtlScalarValueMessage_h

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

namespace igtl
{

class ScalarValueMessage: public MessageBase
{
public:
  typedef ScalarValueMessage           Self;
  typedef MessageBase               Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::ScalarValueMessage, igtl::MessageBase)
  igtlNewMacro(igtl::ScalarValueMessage);

public:

  // VFixture dimensions
  void  SetNumberOfSpheres(int n);
  int   GetNumberOfSpheres();
  void  SetHardness(float h);
  float GetHardness();

  int   SetCenter(int id, float x, float y, float z);
  int   GetCenter(int id, float& x, float& y, float& z);
  int   SetRadius(int id, float r);
  int   GetRadius(int id, float& r);

  int   GetDataSize();
  
  void  AllocateScalars();
  void* GetScalarPointer();

protected:
  ScalarValueMessage();
  ~ScalarValueMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  int     m_NSpheres;
  int     m_Hardness;
  float*  m_CenterX;
  float*  m_CenterY;
  float*  m_CenterZ;
  float*  m_Radius;

  unsigned char*  m_ScalarValueHeader;
  unsigned char*  m_ScalarValue;

};


} // namespace igtl

#endif // _igtlScalarValueMessage_h


