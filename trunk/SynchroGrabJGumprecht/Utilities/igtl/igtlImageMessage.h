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

#ifndef __igtlImageMessage_h
#define __igtlImageMessage_h

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

namespace igtl
{

class ImageMessage: public MessageBase
{
public:
  typedef ImageMessage              Self;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::ImageMessage, igtl::Object)
  igtlNewMacro(igtl::ImageMessage);

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

  void GetDimensions(int s[3]);
  void GetDimensions(int &i, int &j, int &k);
  void GetSpacing(float s[3]);
  void GetSpacing(float &si, float &sj, float &sk);
  int  GetScalarType()         { return scalarType; };

  void GetSubVolume(int dim[3], int off[3]);
  void GetSubVolume(int &dimi, int &dimj, int &dimk, int &offi, int &offj, int &offk);

  int  GetScalarSize()         { return ScalarSizeTable[scalarType]; };
  int  GetScalarSize(int type) { return ScalarSizeTable[type]; };

  // TBD: Should returned value be 64-bit integer?
  int  GetImageSize()
  {
    return dimensions[0]*dimensions[1]*dimensions[2]*GetScalarSize();
  };

  int  GetSubVolumeImageSize()
  {
    return subDimensions[0]*subDimensions[1]*subDimensions[2]*GetScalarSize();
  };

  void  AllocateScalars();
  void* GetScalarPointer();

protected:
  ImageMessage();
  ~ImageMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual void PackBody();
  
  int    dimensions[3];
  float  spacing[3];
  int    subDimensions[3];
  int    subOffset[3];

  Matrix4x4 matrix;

  int    endian;
  int    dataType;
  int    scalarType;
  int    coordinate;

  unsigned char*  m_ImageHeader;
  unsigned char*  m_Image;

};


} // namespace igtl

#endif // _igtlImageMessage_h


