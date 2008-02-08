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

#include "igtl_header.h"
#include "igtl_image.h"

namespace igtl {

const int Image::ScalarSizeTable[] = 
  {
    0, 0,
    1, 1,   // INT8 / UINT8
    2, 2,   // INT16 / UINT16
    4, 4,   // INT32 / UINT32
  };


Image::Image()
{
  for (int i = 0; i < 3; i ++)
    {
      dimensions[i] = 0;
      spacing[i]    = 0.0;
      subDimensions[i] = 0;
      subOffset[i] = 0;
    }
  for (int i = 0; i < 3; i ++)
    {
      for (int j = 0; j < 4; j )
        {
          matrix[i][j] = 0.0;
        }
    }

  endian     = ENDIAN_BIG;
  dataType   = DTYPE_SCALAR;
  coordinate = COORDINATE_RAS;
  scalarType = TYPE_UINT8;
  header     = NULL;
  image      = NULL;
}

Image::~Image()
{
  if (header)
    {
      delete header;
    }
}

void Image::SetDimensions(int s[3])
{
  dimensions[0] = s[0];
  dimensions[1] = s[1];
  dimensions[2] = s[2];

  // initialize sub-volume
  subDimensions[0] = dimensions[0];
  subDimensions[1] = dimensions[1];
  subDimensions[2] = dimensions[2];
  subOffset[0] = 0;
  subOffset[1] = 0;
  subOffset[2] = 0;
}

void Image::SetDimensions(int i, int j, int k)
{
  dimensions[0] = i;
  dimensions[1] = j;
  dimensions[2] = k;

  // initialize sub-volume
  subDimensions[0] = dimensions[0];
  subDimensions[1] = dimensions[1];
  subDimensions[2] = dimensions[2];
  subOffset[0] = 0;
  subOffset[1] = 0;
  subOffset[2] = 0;
}

int Image::SetSubVolume(int dim[3], int off[3])
{
  // make sure that sub-volume fits in the dimensions
  if (off[0] + dim[0] <= dimensions[0] &&
      off[1] + dim[1] <= dimensions[1] &&
      off[2] + dim[2] <= dimensions[2])
    {
      subDimensions[0] = dim[0];
      subDimensions[1] = dim[1];
      subDimensions[2] = dim[2];
      subOffset[0] = off[0];
      subOffset[1] = off[1];
      subOffset[2] = off[2];
      return 1;
    }
  else
    {
      return 0;
    }
}

int Image::SetSubVolume(int dimi, int dimj, int dimk, int offi, int offj, int offk)
{
  // make sure that sub-volume fits in the dimensions
  if (offi + dimi <= dimensions[0] &&
      offj + dimj <= dimensions[1] &&
      offk + dimk <= dimensions[2])
    {
      subDimensions[0] = dimi;
      subDimensions[1] = dimj;
      subDimensions[2] = dimk;
      subOffset[0] = offi;
      subOffset[1] = offj;
      subOffset[2] = offk;
      return 1;
    }
  else
    {
      return 0;
    }
}

void Image::SetSpacing(float s[3])
{
  spacing[0] = s[0];
  spacing[1] = s[1];
  spacing[2] = s[2];
}

void Image::SetSpacing(float si, float sj, float sk)
{
  spacing[0] = si;
  spacing[1] = sj;
  spacing[2] = sk;
}

void Image::SetOrigin(float p[3])
{
  matrix[0][3] = p[0];
  matrix[1][3] = p[1];
  matrix[2][3] = p[2];
}

void Image::SetOrigin(float px, float py, float pz)
{
  matrix[0][3] = px;
  matrix[1][3] = py;
  matrix[2][3] = pz;
}

void Image::SetNormals(float o[3][3])
{
  matrix[0][0] = o[0][0];
  matrix[0][1] = o[0][1];
  matrix[0][2] = o[0][2];
  matrix[1][0] = o[1][0];
  matrix[1][1] = o[1][1];
  matrix[1][2] = o[1][2];
  matrix[2][0] = o[2][0];
  matrix[2][1] = o[2][1];
  matrix[2][2] = o[2][2];
}

void Image::SetNormals(float t[3], float s[3], float n[3])
{
  matrix[0][0] = t[0];
  matrix[1][0] = t[1];
  matrix[2][0] = t[2];
  matrix[0][1] = s[0];
  matrix[1][1] = s[1];
  matrix[2][1] = s[2];
  matrix[0][2] = n[0];
  matrix[1][2] = n[1];
  matrix[2][2] = n[2];
}

void Image::SetMatrix(Matrix4x4& mat)
{
  matrix[0][0] = mat[0][0];
  matrix[1][0] = mat[1][0];
  matrix[2][0] = mat[2][0];
  matrix[0][1] = mat[0][1];
  matrix[1][1] = mat[1][1];
  matrix[2][1] = mat[2][1];
  matrix[0][2] = mat[0][2];
  matrix[1][2] = mat[1][2];
  matrix[2][2] = mat[2][2];
}

void Image::AllocateScalars()
{
  // Calculate memory size
  // Both header and image areas are allocated at the same time
  int size = GetImageSize() + IGTL_IMAGE_HEADER_SIZE;
  
  header = new unsigned char[size];
  image  = (unsigned char*)&header[IGTL_IMAGE_HEADER_SIZE];
}

void* Image::GetScalarPointer()
{
  return (void*)image;
}

void* Image::GetPackPointer()
{
  igtl_image_header* image_header = (igtl_image_header*)header;

  image_header->version     = IGTL_IMAGE_HEADER_VERSION;
  image_header->data_type   = this->dataType;
  image_header->scalar_type = this->scalarType;
  image_header->endian      = this->endian;
  image_header->coord       = this->coordinate;
  image_header->size[0]     = this->dimensions[0];
  image_header->size[1]     = this->dimensions[1];
  image_header->size[2]     = this->dimensions[2];
  image_header->subvol_offset[0] = this->subOffset[0];
  image_header->subvol_offset[1] = this->subOffset[1];
  image_header->subvol_offset[2] = this->subOffset[2];
  image_header->subvol_size[0] = this->subDimensions[0];
  image_header->subvol_size[1] = this->subDimensions[1];
  image_header->subvol_size[2] = this->subDimensions[2];
  igtl_image_convert_byte_order(image_header);

  return (void*)image_header;
}

int Image::GetPackSize()
{
  return GetImageSize() + IGTL_IMAGE_HEADER_SIZE;
}


} // namespace igtl



