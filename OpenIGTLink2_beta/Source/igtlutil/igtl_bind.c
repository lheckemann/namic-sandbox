/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: $
  Language:  C
  Date:      $Date: 2010-07-21 14:15:29 -0400 (Wed, 21 Jul 2010) $
  Version:   $Revision: 6836 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <bind.h>
#include <math.h>

#include "igtl_bind.h"
#include "igtl_util.h"

igtl_uint32 igtl_export igtl_bind_get_bind_length(igtl_bind_header * header)
{
 
  igtl_uint32 length;
  length = (igtl_uint32)(header->length);
  
  return length;
}


void igtl_export igtl_bind_convert_byte_order(igtl_bind_header* header)
{
  int i;

  if (igtl_is_little_endian()) 
    {
    header->encoding = BYTE_SWAP_INT64(header->encoding);
    header->length   = BYTE_SWAP_INT64(header->length);
    }
}


igtl_uint64 igtl_export igtl_bind_get_crc(igtl_bind_header * header, void* bind)
{
  igtl_uint64   crc;
  igtl_uint64   bind_length;

  bind_length = (igtl_uint32)(header->length);

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) header, IGTL_BIND_HEADER_SIZE, crc);
  crc = crc64((unsigned char*) bind, (int)bind_length, crc);

  return crc;
}

