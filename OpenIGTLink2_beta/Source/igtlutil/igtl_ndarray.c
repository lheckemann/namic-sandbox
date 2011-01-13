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

#include <string.h>
#include <math.h>

#include "igtl_ndarray.h"
#include "igtl_util.h"


igtl_uint32 igtl_ndarray_calculate_nelement(igtl_uint16* size, int ndim)
{
  int i;
  igtl_uint32 nelements;
  nelements = 1;

  for (i = 0; i < ndim; i ++)
    {
    nelements *= (igtl_uint32) size[i];
    }

  return nelements;

}


igtl_uint32 igtl_export igtl_ndarray_get_data_size(igtl_ndarray_header * header, igtl_uint16 * size)
{

  int i;
  int dim;     /* number of dimensions */
  igtl_uint32 data_size;

  dim = header->dim;
  data_size = igtl_ndarray_calculate_nelement(size, dim);

  /* multiply number of bytes per element */
  switch (header->type)
    {
    case IGTL_NDARRAY_STYPE_TYPE_INT8:
    case IGTL_NDARRAY_STYPE_TYPE_UINT8:
      data_size *= 1;
      break;
    case IGTL_NDARRAY_STYPE_TYPE_INT16:
    case IGTL_NDARRAY_STYPE_TYPE_UINT16:
      data_size *= 2;
      break;
    case IGTL_NDARRAY_STYPE_TYPE_INT32:
    case IGTL_NDARRAY_STYPE_TYPE_UINT32:
    case IGTL_NDARRAY_STYPE_TYPE_FLOAT32:
      data_size *= 4;
      break;
    case IGTL_NDARRAY_STYPE_TYPE_FLOAT64:
      data_size *= 8;
      break;
    default:
      data_size *= 0;
      break;
    }

  /* adding the size of size table */
  data_size += (igtl_uint32) (dim * sizeof(igtl_uint16));

  return data_size;
}


void igtl_export igtl_ndarray_convert_byte_order(igtl_ndarray_header* header, void* data, int dir)
{

  int i;
  int dim;
  int nbyte;  /* bytes per element (either 1, 2, 4, 8) */
  int nelements;
  igtl_uint16* size;
  igtl_uint8*  p8, p8e;
  igtl_uint16* p16, p16e;
  igtl_uint32* p32, p32e;
  igtl_uint64* p64, p64e;

  if (igtl_is_little_endian()) 
    {
    dim = (int) header->dim;
    size = (igtl_uint16*) data;

    /* if the function is called to convert from the host byte order to the network byte order,
       first calculate the number of elements in the N-D array data */
    if (dir == IGTL_NDARRAY_HOST_TO_NETWORK)
      {
      nelements = igtl_ndarray_calculate_nelement(size, dim);
      /* convert the byte order of the size table */
      for (i = 0; i < header->dim; i ++)
        {
        size[i] = BYTE_SWAP_INT16(size[i]);
        }
      }
    else /* if (dir == IGTL_NDARRAY_HOST_TO_NETWORK) */
      {
      /* convert the byte order of the size table */
      for (i = 0; i < header->dim; i ++)
        {
        size[i] = BYTE_SWAP_INT16(size[i]);
        }
      nelements = igtl_ndarray_calculate_nelement(size, dim);
      }

    /* convert the byte order of the N-D array */
    /* check the data type (TYPE field does not endian-dependent) */
    switch (header->type)
      {
      case IGTL_NDARRAY_STYPE_TYPE_INT8:
      case IGTL_NDARRAY_STYPE_TYPE_UINT8:
        p8 = (igtl_uint8*) data;
        p8e = p8 + nelements;
        while (p8 < p8e)
          {
          *p8 = BYTE_SWAP_INT8(*p8);
          p8 ++;
          }
        break;
      case IGTL_NDARRAY_STYPE_TYPE_INT16:
      case IGTL_NDARRAY_STYPE_TYPE_UINT16:
        p16 = (igtl_uint16*) data;
        p16e = p16 + nelements;
        while (p16 < p16e)
          {
          *p16 = BYTE_SWAP_INT16(*p16);
          p16 ++;
          }
        break;
      case IGTL_NDARRAY_STYPE_TYPE_INT32:
      case IGTL_NDARRAY_STYPE_TYPE_UINT32:
      case IGTL_NDARRAY_STYPE_TYPE_FLOAT32:
        p32 = (igtl_uint32*) data;
        p32e = p32 + nelements;
        while (p32 < p32e)
          {
          *p32 = BYTE_SWAP_INT32(*p32);
          p32 ++;
          }
        break;
      case IGTL_NDARRAY_STYPE_TYPE_FLOAT64:
        p64 = (igtl_uint64*) data;
        p64e = p64 + nelements;
        while (p64 < p64e)
          {
          *p64 = BYTE_SWAP_INT64(*p64);
          p64 ++;
          }
        break;
      default:
        break;
      }
    }
  
}


igtl_uint64 igtl_export igtl_ndarray_get_crc(igtl_ndarray_header * header, void* data)
{
  int i;
  igtl_uint64   crc;
  int           data_size;
  igtl_uint16*  size;

  /* calculate the size of N-D array data (size table + body) */ 
  size = (igtl_uint16*) data;
  /* convert byte order (since it is already in network byte order) */
  if (igtl_is_little_endian()) 
    {
    for (i = 0; i < header->dim; i ++)
      {
      size[i] = BYTE_SWAP_INT16(size[i]);
      }
    }
  data_size = (int) igtl_ndarray_get_data_size(header, size);
  /* convert byte order to the original */
  if (igtl_is_little_endian()) 
    {
    for (i = 0; i < header->dim; i ++)
      {
      size[i] = BYTE_SWAP_INT16(size[i]);
      }
    }

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) header, IGTL_NDARRAY_HEADER_SIZE, crc);
  crc = crc64((unsigned char*) data, (int)data_size, crc);

  return crc;
}
