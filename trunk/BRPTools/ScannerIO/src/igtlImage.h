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

#ifndef __igtlImage_h
#define __igtlImage_h

#include "igtlObject.h"
#include "igtlMacros.h"
#include "igtlMath.h"

namespace igtl
{

class Image: public Object
{
public:
  typedef Image               Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::Image, igtl::Object)
  igtlNewMacro(igtl::Image);

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
    DTYPE_SCALAR = 1,
    DTYPE_VECTOR = 2
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

  void Delete() { delete this; };
  
  void SetDimensions(int s[3]);
  void SetDimensions(int i, int j, int k);
  int  SetSubVolume(int dim[3], int off[3]);
  int  SetSubVolume(int dimi, int dimj, int dimk, int offi, int offj, int offk);

  void SetSpacing(float s[3]);
  void SetSpacing(float si, float sj, float sk);
  void SetOrigin(float p[3]);
  void SetOrigin(float px, float py, float pz);
  void SetNormals(float o[3][3]);
  void SetNormals(float t[3], float s[3], float n[3]);
  void SetMatrix(Matrix4x4& mat);

  void SetScalarType(int t)    { scalarType = t; };
  void SetScalarTypeToInt8()   { scalarType = TYPE_INT8; };
  void SetScalarTypeToUint8()  { scalarType = TYPE_UINT8; };
  void SetScalarTypeToInt16()  { scalarType = TYPE_INT16; };
  void SetScalarTypeToUint16() { scalarType = TYPE_UINT16; };
  void SetScalarTypeToInt32()  { scalarType = TYPE_INT32; };
  void SetScalarTypeToUint32() { scalarType = TYPE_UINT32; };

  int  GetScalarSize()         { return ScalarSizeTable[scalarType]; };
  int  GetScalarSize(int type) { return ScalarSizeTable[type]; };

  // TBD: Should returned value be 64-bit integer?
  int  GetImageSize()
  {
    return dimensions[0]*dimensions[1]*dimensions[2]*GetScalarSize();
  };

  void AllocateScalars();
  void* GetScalarPointer();

  void* GetPackPointer();
  int   GetPackSize();

protected:
  Image();
  ~Image();
  
protected:
  int    dimensions[3];
  float  spacing[3];
  int    subDimensions[3];
  int    subOffset[3];

  Matrix4x4 matrix;

  int    endian;
  int    dataType;
  int    scalarType;
  int    coordinate;

  // Pointers to header and image
  //  To prevent large copy of byte array in GetPack() function,
  //  header byte array is concatinated to that of image.
  //  Consequently,
  //     scalar = header + sizeof (igtl_image_header)
  //  after these areas are allocated.
  //
  unsigned char*  header;
  unsigned char*  image;

};


} // namespace igtl

#endif // _igtlImage_h


