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


void igtl_export igtl_ndarray_init_info(igtl_ndarray_info * info)
{
  info->type  = 0;
  info->dim   = 0;
  info->size  = NULL;
  info->array = NULL;
}


/* Calculate number of bytes / element */
igtl_uint32 igtl_ndarray_get_nbyte(int type)
{
  igtl_uint32 bytes;

  switch (type)
    {
    case IGTL_NDARRAY_STYPE_TYPE_INT8:
    case IGTL_NDARRAY_STYPE_TYPE_UINT8:
      bytes = 1;
      break;
    case IGTL_NDARRAY_STYPE_TYPE_INT16:
    case IGTL_NDARRAY_STYPE_TYPE_UINT16:
      bytes = 2;
      break;
    case IGTL_NDARRAY_STYPE_TYPE_INT32:
    case IGTL_NDARRAY_STYPE_TYPE_UINT32:
    case IGTL_NDARRAY_STYPE_TYPE_FLOAT32:
      bytes = 4;
      break;
    case IGTL_NDARRAY_STYPE_TYPE_FLOAT64:
      bytes = 8;
      break;
    case IGTL_NDARRAY_STYPE_TYPE_COMPLEX:
      bytes = 16;
      break;
    default:
      bytes = 0;
      break;
    }

  return bytes;
}


int igtl_export igtl_ndarray_alloc_info(igtl_ndarray_info * info, const igtl_uint16 * size)
{
  int i;
  igtl_uint64 len;

  if (info->size == NULL && info->array == NULL)
    {
    info->size = malloc(sizeof(igtl_uint16) * (igtl_uint16) igtl_uint8);
    if (info->size == NULL)
      {
      return 0;
      }
    len = 1;

    for (i = 0; i < info->dim; i ++)
      {
      info->size[i] = size[i];
      len *=  size[i];
      }
    
    info->array = malloc(igtl_ndarray_get_nbyte(info->type) * len);

    if (info->array)
      {
      free(info->size);
      return 0;
      }

    return 1;
    }
  else
    {
    return 0;
    }
}


int igtl_export igtl_ndarray_free_info(igtl_ndarray_info * info)
{
  if (info->size)
    {
    free(info->size);
    }
  if (info->array)
    {
    free(info->array);
    }

  return 1;
}


int igtl_export igtl_ndarray_unpack(int type, void * byte_array, igtl_ndarray_info * info, igtl_uint64 pack_size)
{
  void * ptr;
  igtl_uint16 dim;
  igtl_uint16 * size;
  igtl_uint16 i;
  igtl_uint64 len;

  igtl_uint16 * ptr16_src;
  igtl_uint16 * ptr16_src_end;
  igtl_uint16 * ptr16_dst;

  igtl_uint32 * ptr32_src;
  igtl_uint32 * ptr32_src_end;
  igtl_uint32 * ptr32_dst;

  igtl_uint64 * ptr64_src;
  igtl_uint64 * ptr64_src_end;
  igtl_uint64 * ptr64_dst;


  if (byte_array == NULL || info == NULL)
    {
    return 0;
    }

  igtl_ndarray_init_info(info);
  ptr = byte_array;

  /*** Type field ***/
  info->type = *ptr;
  ptr ++;

  /*** Dimension field ***/
  info->dim  = *ptr;
  ptr ++;

  /*** Size array field ***/
  size = (igtl_uint16 *) ptr;
  if (igtl_is_little_endian())
    {
    /* Change byte order -- this overwrites memory area for the pack !!*/
    for (i = 0; i < dim; i ++)
      {
      size[i] = BYTE_SWAP_INT16(size[i]);
      }
    }

  igtl_ndarray_alloc_info(info, size);

  memcpy(info->size, size, 1, sizeof(igtl_uint16) * dim);
  if (igtl_is_little_endian())
    {
    /* Resotore the overwritten memory area */
    /* Don't use size[] array after this !! */
    for (i = 0; i < dim; i ++)
      {
      size[i] = BYTE_SWAP_INT16(size[i]);
      }
    }
  ptr += sizeof(igtl_uint16) * dim;

  /*** N-D array field ***/
  /* Calculate number of elements in N-D array */
  len = 1;
  for (i = 0; i < dim; i ++)
    {
    len *= info->size[i];
    }

  /* Copy array */
  if (igtl_ndarray_get_nbyte(info->type) == 1 || !igtl_is_little_endian())
    {
    /* If single-byte data type is used or the program runs on a big-endian machine,
       just copy the array from the pack to the strucutre */
    memcpy(info->array, ptr, 1, len * igtl_ndarray_get_nbyte(info->type));
    }
  else if (igtl_ndarray_get_nbyte(info->type) == 2) /* 16-bit */
    {
    ptr16_src = (igtl_uint16 *) ptr;
    ptr16_src_end += len;
    ptr16_dst = (igtl_uint16 *) info->array;
    while (ptr16_src < ptr16_src_end)
      {
      *ptr16_dst = BYTE_SWAP_INT16(*ptr16_src);
      ptr16_dst ++;
      ptr16_src ++;
      }
    }
  else if (igtl_ndarray_get_nbyte(info->type) == 4) /* 32-bit */
    {
    ptr32_src = (igtl_uint32 *) ptr;
    ptr32_src_end += len;
    ptr32_dst = (igtl_uint32 *) info->array;
    while (ptr32_src < ptr32_src_end)
      {
      *ptr32_dst = BYTE_SWAP_INT32(*ptr32_src);
      ptr32_dst ++;
      ptr32_src ++;
      }
    }
  else /* 64-bit or Complex type */
    {
    ptr64_src = (igtl_uint64 *) ptr;
    /* Adding number of elements to the pointer -- 64-bit: len * 1; Complex: len * 2*/
    ptr64_src_end += len * igtl_ndarray_get_nbyte(info->type)/8;
    ptr64_dst = (igtl_uint64 *) info->array;
    while (ptr64_src < ptr64_src_end)
      {
      *ptr64_dst = BYTE_SWAP_INT64(*ptr64_src);
      ptr64_dst ++;
      ptr64_src ++;
      }
    }

  /* TODO: check if the pack size is valid */

  return 1;

}


int igtl_export igtl_ndarray_pack(igtl_ndarray_info * info, void * byte_array, int type)
{
  void * ptr;
  igtl_uint16 dim;
  igtl_uint16 * size;
  igtl_uint16 i;
  igtl_uint64 len;

  igtl_uint16 * ptr16_src;
  igtl_uint16 * ptr16_src_end;
  igtl_uint16 * ptr16_dst;

  igtl_uint32 * ptr32_src;
  igtl_uint32 * ptr32_src_end;
  igtl_uint32 * ptr32_dst;

  igtl_uint64 * ptr64_src;
  igtl_uint64 * ptr64_src_end;
  igtl_uint64 * ptr64_dst;


  if (byte_array == NULL || info == NULL)
    {
    return 0;
    }

  ptr = byte_array;

  /*** Type field ***/
  *ptr = info->type;
  ptr ++;

  /*** Dimension field ***/
  *ptr = info->dim;
  ptr ++;

  /*** Size array field ***/
  size = (igtl_uint16 *) ptr;
  if (igtl_is_little_endian())
    {
    for (i = 0; i < info->dim; i ++)
      {
      size[i] = BYTE_SWAP_INT16(info->size[i]);
      }
    }
  else
    {
    memcpy(ptr, info->size, 1, sizeof(igtl_uint16) * info->dim);
    }

  ptr += sizeof(igtl_uint16) * dim;

  /*** N-D array field ***/
  /* Calculate number of elements in N-D array */
  len = 1;
  for (i = 0; i < dim; i ++)
    {
    len *= info->size[i];
    }

  /* Copy array */
  if (igtl_ndarray_get_nbyte(info->type) == 1 || !igtl_is_little_endian())
    {
    /* If single-byte data type is used or the program runs on a big-endian machine,
       just copy the array from the pack to the strucutre */
    memcpy(ptr, info->array, 1, len * igtl_ndarray_get_nbyte(info->type));
    }
  else if (igtl_ndarray_get_nbyte(info->type) == 2) /* 16-bit */
    {
    ptr16_src = (igtl_uint16 *) info->array;
    ptr16_src_end += len;
    ptr16_dst = (igtl_uint16 *) ptr;
    while (ptr16_src < ptr16_src_end)
      {
      *ptr16_dst = BYTE_SWAP_INT16(*ptr16_src);
      ptr16_dst ++;
      ptr16_src ++;
      }
    }
  else if (igtl_ndarray_get_nbyte(info->type) == 4) /* 32-bit */
    {
    ptr32_src = (igtl_uint32 *) info->array;
    ptr32_src_end += len;
    ptr32_dst = (igtl_uint32 *) ptr;
    while (ptr32_src < ptr32_src_end)
      {
      *ptr32_dst = BYTE_SWAP_INT32(*ptr32_src);
      ptr32_dst ++;
      ptr32_src ++;
      }
    }
  else /* 64-bit or Complex type */
    {
    ptr64_src = (igtl_uint64 *) info->array;
    /* Adding number of elements to the pointer -- 64-bit: len * 1; Complex: len * 2*/
    ptr64_src_end += len * igtl_ndarray_get_nbyte(info->type)/8;
    ptr64_dst = (igtl_uint64 *) ptr;
    while (ptr64_src < ptr64_src_end)
      {
      *ptr64_dst = BYTE_SWAP_INT64(*ptr64_src);
      ptr64_dst ++;
      ptr64_src ++;
      }
    }

  return 1;

}


igtl_uint32 igtl_export igtl_ndarray_get_data_size(igtl_ndarray_header * header, igtl_uint16 * size)
{

  int i;
  int dim;     /* number of dimensions */
  igtl_uint32 data_size;

  dim = header->dim;
  data_size = igtl_ndarray_calculate_nelement(size, dim);

  /* multiply number of bytes per element */
  data_size *= igtl_ndarray_get_nbyte(header->type)

  
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
    /* check the data type (TYPE field is not endian-dependent) */
    switch (header->type)
      {
      case IGTL_NDARRAY_STYPE_TYPE_INT8:
      case IGTL_NDARRAY_STYPE_TYPE_UINT8:
        p8 = (igtl_uint8*) data;
        p8e = p8 + nelements;
        while (p8 < p8e)
          {
          *p8 = *p8;
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
      case IGTL_NDARRAY_STYPE_TYPE_COMPLEX:
        p64 = (igtl_uint64*) data;
        p64e = p64 + nelements * 2;
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
  /* convert byte order (since the data is already in network byte order) */
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
