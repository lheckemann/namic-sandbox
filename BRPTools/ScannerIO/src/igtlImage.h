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

#ifndef __IGTL_IMAGE_H
#define __IGTL_IMAGE_H

#include "igtlObject.h"
#include "igtlMacros.h"

namespace igtl {

class Image : public Object
{
public:

  enum {
    COORDINATE_LPS,
    COORDINATE_RAS
  };

  enum {
    ENDIAN_BIG,
    ENDIAN_LITTLE
  };

  enum {
    TYPE_INT8   = 2,
    TYPE_UINT8  = 3,
    TYPE_INT16  = 4,
    TYPE_UINT16 = 5,
    TYPE_INT32  = 6,
    TYPE_UINT32 = 7
  };

  static const int ScalarSizeTable[];

public:
  static Image* New();
  void Delete() { delete this; };
  
  void SetDimensions(int s[3]);
  void SetDimensions(int i, int j, int k);
  void SetSpacing(float s[3]);
  void SetSpacing(float si, float sj, float sk);
  void SetOrigin(float p[3]);
  void SetOrigin(float px, float py, float pz);
  void SetNormals(float o[3][3]);
  void SetNormals(float t[3], float s[3], float n[3]);

  void SetScalarType(int t)    { scalarType = t; };
  void SetScalarTypeToInt8()   { scalarType = TYPE_INT8; };
  void SetScalarTypeToUint8()  { scalarType = TYPE_UINT8; };
  void SetScalarTypeToInt16()  { scalarType = TYPE_INT16; };
  void SetScalarTypeToUint16() { scalarType = TYPE_UINT16; };
  void SetScalarTypeToInt32()  { scalarType = TYPE_INT32; };
  void SetScalarTypeToUint32() { scalarType = TYPE_UINT32; };

  int GetScalarSize()          { return ScalarSizeTable[scalarType]; };
  int GetScalarSize(int type)  { return ScalarSizeTable[type]; };

  void AllocateScalars();
  void* GetScalarPointer();

  void GetPack(int& size, void* pointer);

protected:
  Image();
  virtual ~Image() {};
  
protected:
  int    dimensions[3];
  float  spacing[3];
  float  origin[3];
  float  orientation[3][3];

  int    endian;
  int    scalarType;
  char*  scalar;

};


} // namespace igtl

#endif // _IGTL_IMAGE_H


