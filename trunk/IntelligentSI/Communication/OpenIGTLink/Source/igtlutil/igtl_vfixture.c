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

#include "igtl_vfixture.h"
#include "igtl_util.h"

igtl_uint32 igtl_vfixture_data_size(igtl_vfixture_header * header)
{
  if (header->version == IGTL_VFIXTURE_HEADER_VERSION)
    {
    return header->nspheres * sizeof(igtl_vfixture_data);
    }
  else
    {
    return 0;
    }
}

void igtl_vfixture_convert_byte_order(igtl_vfixture_header * header)
{
  igtl_uint32 tmp;

  if (igtl_is_little_endian())
    {
    header->version     = BYTE_SWAP_INT16(header->version);
    header->nspheres    = BYTE_SWAP_INT16(header->nspheres);
    memcpy((void*)&tmp, (void*)(&(header->hardness)), sizeof(igtl_float32));
    tmp = BYTE_SWAP_INT32(tmp);
    memcpy((void*)(&(header->hardness)), (void*)&tmp, sizeof(igtl_float32));
    }
}

void igtl_vfixture_data_convert_byte_order(int n, void* data)
{
  int i;
  igtl_uint32 tmp;
  igtl_vfixture_data* vftmp;

  vftmp = (igtl_vfixture_data*)data;
  for (i = 0; i < n; i ++)
    {
    /* x coordinate */
    memcpy((void*)&tmp, (void*)(&(vftmp[i].x)), sizeof(igtl_float32));
    tmp = BYTE_SWAP_INT32(tmp);
    memcpy((void*)(&(vftmp[i].x)), (void*)&tmp, sizeof(igtl_float32));
    /* y coordinate */
    memcpy((void*)&tmp, (void*)(&(vftmp[i].y)), sizeof(igtl_float32));
    tmp = BYTE_SWAP_INT32(tmp);
    memcpy((void*)(&(vftmp[i].y)), (void*)&tmp, sizeof(igtl_float32));
    /* z coordinate */
    memcpy((void*)&tmp, (void*)(&(vftmp[i].z)), sizeof(igtl_float32));
    tmp = BYTE_SWAP_INT32(tmp);
    memcpy((void*)(&(vftmp[i].z)), (void*)&tmp, sizeof(igtl_float32));
    }
}

igtl_uint64 igtl_vfixture_get_crc(igtl_vfixture_header * header, void* data)
{
  igtl_uint64          crc;
  igtl_uint32          data_size;
  igtl_vfixture_header tmpheader;

  if (igtl_is_little_endian())
    {
    memcpy((void*)&tmpheader, (void*)header, sizeof(igtl_vfixture_header));
    igtl_vfixture_convert_byte_order((igtl_vfixture_header*)&tmpheader);
    data_size = igtl_vfixture_data_size((igtl_vfixture_header*)&tmpheader);
    }
  else
    {
    data_size = igtl_vfixture_data_size(header);
    }

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*)header, sizeof(igtl_vfixture_header), crc);
  crc = crc64((unsigned char*) data, data_size, crc);

  return crc;
    
}
