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

#include "igtlImage.h"

namespace igtl {

const int Image::ScalarSizeTable[] = 
  {
    0, 0,
    1, 1,   // INT8 / UINT8
    2, 2,   // INT16 / UINT16
    4, 4,   // INT32 / UINT32
  };


Image* Image::New()
{
  return new Image;
}

Image::Image()
{
  for (int i = 0; i < 3; i ++)
    {
      dimensions[i] = 0;
      spacing[i]    = 0.0;
      origin[i]     = 0.0;
      for (int j = 0; j < 3; j )
        {
          orientation[i][j] = 0.0;
        }
    }

  endian     = ENDIAN_BIG;
  scalarType = 0;
  scalar     = NULL;
}

void Image::SetDimensions(int s[3])
{
}

void Image::SetDimensions(int i, int j, int k)
{
}

void Image::SetSpacing(float s[3])
{
}

void Image::SetSpacing(float si, float sj, float sk)
{
}

void Image::SetOrigin(float p[3])
{
}

void Image::SetOrigin(float px, float py, float pz)
{
}

void Image::SetNormals(float o[3][3])
{
}

void Image::SetNormals(float t[3], float s[3], float n[3])
{
}

void Image::AllocateScalars()
{
}

void* Image::GetScalarPointer()
{
}

void Image::GetPack(int& size, void* pointer)
{
}



} // namespace igtl



