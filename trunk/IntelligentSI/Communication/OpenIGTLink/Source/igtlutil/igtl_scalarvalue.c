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

#include "igtl_scalarvalue.h"
#include "igtl_util.h"

igtl_uint64 igtl_scalarvalue_get_data_size(igtl_scalarvalue_header * header)
{
  igtl_uint64 si;
  igtl_uint64 sp;
  igtl_uint64 return_data_size;

  si = header->dimension;
  

  switch (header->scalar_type) {
  case IGTL_SCALARVALUE_STYPE_TYPE_INT8:
  case IGTL_SCALARVALUE_STYPE_TYPE_UINT8:
    sp = 1;
    break;
  case IGTL_SCALARVALUE_STYPE_TYPE_INT16:
  case IGTL_SCALARVALUE_STYPE_TYPE_UINT16:
    sp = 2;
    break;
  case IGTL_SCALARVALUE_STYPE_TYPE_INT32:
  case IGTL_SCALARVALUE_STYPE_TYPE_UINT32:
    sp = 4;
    break;
  default:
    sp = 0;
    break;
  }

  return_data_size = si*sp;
  return return_data_size;
}



void igtl_scalarvalue_convert_byte_order(igtl_scalarvalue_header * header)
{

  if (igtl_is_little_endian()) 
    {

      header->version = BYTE_SWAP_INT16(header->version);
      
      header->data_size = BYTE_SWAP_INT16(header->data_size);

  }
}


igtl_uint64 igtl_scalarvalue_get_crc(igtl_scalarvalue_header * header, void* scalarvalue)
{
  igtl_uint64   crc;
  igtl_uint64   scalarvalue_size;

  scalarvalue_size = igtl_scalarvalue_get_data_size(header);
  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) header, IGTL_SCALARVALUE_HEADER_SIZE, crc);
  crc = crc64((unsigned char*) scalarvalue, scalarvalue_size, crc);

  return crc;
}
