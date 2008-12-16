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
  void SetDimension(int i);
  void GetDimension(int i);
  typedef ScalarValueMessage              Self;
  typedef MessageBase               Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::ScalarValueMessage, igtl::MessageBase)
  igtlNewMacro(igtl::ScalarValueMessage);

public:

  enum {
    ENDIAN_BIG,
    ENDIAN_LITTLE
  };

  enum {
    TYPE_INT8    = 2,
    TYPE_UINT8   = 3,
    TYPE_INT16   = 4,
    TYPE_UINT16  = 5,
    TYPE_INT32   = 6,
    TYPE_UINT32  = 7,
    TYPE_FLOAT32 = 10,
    TYPE_FLOAT64 = 11
  };

  static const int ScalarSizeTable[];

public:

  // ScalarValue scalar type
  void SetScalarType(int t)    { scalarType = t; };
  void SetScalarTypeToInt8()   { scalarType = TYPE_INT8; };
  void SetScalarTypeToUint8()  { scalarType = TYPE_UINT8; };
  void SetScalarTypeToInt16()  { scalarType = TYPE_INT16; };
  void SetScalarTypeToUint16() { scalarType = TYPE_UINT16; };
  void SetScalarTypeToInt32()  { scalarType = TYPE_INT32; };
  void SetScalarTypeToUint32() { scalarType = TYPE_UINT32; };
  int  GetScalarType()         { return scalarType; };
  int  GetScalarSize()         { return ScalarSizeTable[scalarType]; };
  int  GetScalarSize(int type) { return ScalarSizeTable[type]; };

  // TBD: Should returned value be 64-bit integer?
  int  GetScalarValueSize()
  {
    return dimension * GetScalarSize();
  };

  void  AllocateScalars();
  void* GetScalarPointer();

protected:
  ScalarValueMessage();
  ~ScalarValueMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  

  int dimension;

  int    endian;
  int    scalarType;

  unsigned char*  m_ScalarValueHeader;
  unsigned char*  m_ScalarValue;

};


} // namespace igtl

#endif // _igtlScalarValueMessage_h


