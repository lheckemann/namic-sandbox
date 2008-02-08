/*=========================================================================

  Program:   Open ITK Link Library
  Module:    $RCSfile: $
  Language:  C
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <string.h>
#include <math.h>

#include "igtl_image.h"
#include "igtl_util.h"
#include "crc32.h"

long long igtl_image_get_data_size(igtl_image_header * header)
{
  long long si;
  long long sj;
  long long sk;
  long long sp;
  long long data_size;

  si = header->subvol_size[0];
  sj = header->subvol_size[1];
  sk = header->subvol_size[2];

  switch (header->scalar_type) {
  case IGTL_IMAGE_STYPE_TYPE_INT8:
  case IGTL_IMAGE_STYPE_TYPE_UINT8:
    sp = 1;
    break;
  case IGTL_IMAGE_STYPE_TYPE_INT16:
  case IGTL_IMAGE_STYPE_TYPE_UINT16:
    sp = 2;
    break;
  case IGTL_IMAGE_STYPE_TYPE_INT32:
  case IGTL_IMAGE_STYPE_TYPE_UINT32:
    sp = 4;
    break;
  default:
    sp = 0;
    break;
  }

  data_size = si*sj*sk*sp;
  return data_size;
}


void igtl_image_set_matrix(float spacing[3], float origin[3],
                            float norm_i[3], float norm_j[3], float norm_k[3],
                            igtl_image_header * header)
{
  header->matrix[0]  = norm_i[0] * spacing[0];
  header->matrix[1]  = norm_i[1] * spacing[0];
  header->matrix[2]  = norm_i[2] * spacing[0];
  header->matrix[3]  = - norm_j[0] * spacing[1];
  header->matrix[4]  = - norm_j[1] * spacing[1];
  header->matrix[5]  = - norm_j[2] * spacing[1];
  header->matrix[6]  = - norm_k[0] * spacing[2];
  header->matrix[7]  = - norm_k[1] * spacing[2];
  header->matrix[8]  = - norm_k[2] * spacing[2];
  header->matrix[9]  = origin[0];
  header->matrix[10] = origin[1];
  header->matrix[11] = origin[2];
}

void igtl_image_get_matrix(float spacing[3], float origin[3],
                            float norm_i[3], float norm_j[3], float norm_k[3],
                            igtl_image_header * header)
{
  float tx;
  float ty;
  float tz;
  float sx;
  float sy;
  float sz;
  float nx;
  float ny;
  float nz;
  float px;
  float py;
  float pz;

  tx = header->matrix[0];
  ty = header->matrix[1];
  tz = header->matrix[2];
  sx = header->matrix[3];
  sy = header->matrix[4];
  sz = header->matrix[5];
  nx = header->matrix[6];
  ny = header->matrix[7];
  nz = header->matrix[8];
  px = header->matrix[9];
  py = header->matrix[10];
  pz = header->matrix[11];

  spacing[0] = sqrt(tx*tx + ty*ty + tz*tz);
  spacing[1] = sqrt(sx*sx + sy*sy + sz*sz);
  spacing[2] = sqrt(nx*nx + ny*ny + nz*nz);
  norm_i[0] = header->matrix[0]  / spacing[0];
  norm_i[1] = header->matrix[1]  / spacing[0];
  norm_i[2] = header->matrix[2]  / spacing[0];
  norm_j[0] = - header->matrix[3] / spacing[1];
  norm_j[1] = - header->matrix[4] / spacing[1];
  norm_j[2] = - header->matrix[5] / spacing[1];
  norm_k[0] = - header->matrix[6] / spacing[2];
  norm_k[1] = - header->matrix[7] / spacing[2];
  norm_k[2] = - header->matrix[8] / spacing[2];
  origin[0] = header->matrix[9];
  origin[1] = header->matrix[10];
  origin[2] = header->matrix[11];

}


void igtl_image_convert_byte_order(igtl_image_header * header)
{
  int i;
  long tmp[12];

  if (igtl_is_little_endian()) 
    {

    header->version = BYTE_SWAP_INT16(header->version);

    for (i = 0; i < 3; i ++) 
      {
      header->size[i] = BYTE_SWAP_INT16(header->size[i]);
      header->subvol_size[i] = BYTE_SWAP_INT16(header->subvol_size[i]);
      header->subvol_offset[i] = BYTE_SWAP_INT16(header->subvol_offset[i]);
      }
    memcpy((void*)tmp, (void*)(header->matrix), sizeof(float)*12);

    for (i = 0; i < 12; i ++) 
      {
      tmp[i] = BYTE_SWAP_INT32(tmp[i]);
      }

    memcpy((void*)(header->matrix), (void*)tmp, sizeof(float)*12);
  }
}

unsigned long igtl_image_get_crc(igtl_image_header * header, void* image)
{
  unsigned long crc = crc32(0L, Z_NULL, 0);
  unsigned int  img_size =  igtl_image_get_data_size(header);

  crc = crc32(crc, (unsigned char*)header, IGTL_IMAGE_HEADER_SIZE);

  /*
   * NOTE: OpenIGTLink protocol supports 64-bit size info but here only allows 32-bit value;
   *       This have to be fixed in the future.
   */
  crc = crc32(crc, (unsigned char*)image, img_size);

  return crc;
}
