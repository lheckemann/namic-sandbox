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

#include "igtl_image.h"
#include "igtl_util.h"

long long igtl_image_get_data_size(struct igtl_image_header* header)
{
  long long si, sj, sk, sp;

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

  return si*sj*sk*sp;
}


void igtl_image_get_matrix(float spacing[3], float origin[3],
                            float norm_i[3], float norm_j[3], float norm_k[3],
                            struct igtl_image_header* header)
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

void igtl_image_convert_byte_order(struct igtl_image_header* header)
{
  int i;
  long tmp[12];

  if (igtl_is_little_endian()) {
    header->version = BYTE_SWAP_INT16(header->version);
    for (i = 0; i < 3; i ++) {
      header->size[i] = BYTE_SWAP_INT16(header->size[i]);
      header->subvol_size[i] = BYTE_SWAP_INT16(header->subvol_size[i]);
      header->subvol_offset[i] = BYTE_SWAP_INT16(header->subvol_offset[i]);
    }
    memcpy((void*)tmp, (void*)(header->matrix), sizeof(float)*12);
    for (i = 0; i < 12; i ++) {
      tmp[i] = BYTE_SWAP_INT32(tmp[i]);
    }
    memcpy((void*)(header->matrix), (void*)tmp, sizeof(float)*12);
  }
}

