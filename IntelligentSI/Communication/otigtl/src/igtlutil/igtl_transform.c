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
#include "igtl_transform.h"
#include "igtl_util.h"
#include "crc32.h"

void igtl_transform_convert_byte_order(float* transform)
{
  int i = 0;
  long tmp[12];

  if (igtl_is_little_endian()) {
    memcpy((void*)tmp, (void*)transform, sizeof(float)*12);
    for (i = 0; i < 12; i ++) {
      tmp[i] = BYTE_SWAP_INT32(tmp[i]);
    }
    memcpy((void*)transform, (void*)tmp, sizeof(float)*12);
  }
}

unsigned long igtl_transform_get_crc(float* transform)
{
  unsigned long crc = crc32(0L, Z_NULL, 0);
  crc = crc32(crc, (unsigned char*)transform, sizeof(float)*12);

  return crc;
}
