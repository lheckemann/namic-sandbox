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

#include <stdlib.h>
#include <string.h>

#include "igtl_polydata.h"
#include "igtl_util.h"


/* Calculate number of bytes / element */
igtl_uint32 igtl_polydata_get_nbyte(int type)
{
  igtl_uint32 bytes;

  switch (type)
    {
    case IGTL_POLYDATA_TYPE_INT8:
    case IGTL_POLYDATA_TYPE_UINT8:
      bytes = 1;
      break;
    case IGTL_POLYDATA_TYPE_INT16:
    case IGTL_POLYDATA_TYPE_UINT16:
      bytes = 2;
      break;
    case IGTL_POLYDATA_TYPE_INT32:
    case IGTL_POLYDATA_TYPE_UINT32:
    case IGTL_POLYDATA_TYPE_FLOAT32:
      bytes = 4;
      break;
    case IGTL_POLYDATA_TYPE_FLOAT64:
      bytes = 8;
      break;
    default:
      bytes = 0;
      break;
    }

  return bytes;
}

void igtl_export igtl_polydata_init_info(igtl_polydata_info * info)
{
  if (info)
    {
    info->header.npoints = 0;
    info->header.type_point = 0;
    info->header.reserved = 0;
    info->header.nvertices = 0;
    info->header.size_vertices = 0;
    info->header.nlines = 0;
    info->header.size_lines = 0;
    info->header.npolygons = 0;
    info->header.size_polygons = 0;
    info->header.ntriangle_strips = 0;
    info->header.size_triangle_strips = 0;
    info->points = NULL;
    info->vertices = NULL;
    info->lines = NULL;
    info->polygons = NULL;
    info->triangle_strips = NULL;
    }
}


int igtl_export igtl_polydata_alloc_info(igtl_polydata_info * info)
{
  size_t size;
  
  if (info == NULL)
    {
    return 0;
    }

  if (info->points)
    {
    free(info->points);
    }
  info->points = malloc(igtl_polydata_get_nbyte(info->header.type_point)*info->header.npoints*3);
  if (info->points == NULL)
    {
    return 0;
    }

  if (info->vertices)
    {
    free(info->vertices);
    }
  info->vertices = malloc(sizeof(igtl_uint32)*info->header.size_vertices);
  if (info->vertices)
    {
    return 0;
    }

  if (info->lines)
    {
    free(info->lines);
    }
  info->lines = malloc(sizeof(igtl_uint32)*info->header.size_lines);
  if (info->lines)
    {
    return 0;
    }

  if (info->polygons)
    {
    free(info->polygons);
    }
  info->polygons = malloc(sizeof(igtl_uint32)*info->header.size_polygons);  
  if (info->polygons)
    {
    return 0;
    }

  if (info->triangle_strips)
    {
    free(info->triangle_strips);
    }
  info->triangle_strips = malloc(sizeof(igtl_uint32)*info->header.size_triangle_strips);
  if (info->triangle_strips)
    {
    return 0;
    }

  return 1;
  
}


int igtl_export igtl_polydata_free_info(igtl_polydata_info * info)
{

  if (info == NULL)
    {
    return 0;
    }

  if (info->points)
    {
    free(info->points);
    }

  if (info->vertices)
    {
    free(info->vertices);
    }

  if (info->lines)
    {
    free(info->lines);
    }

  if (info->polygons)
    {
    free(info->polygons);
    }

  if (info->triangle_strips)
    {
    free(info->triangle_strips);
    }

  return 1;

}


int igtl_polydata_convert_byteorder_topology(igtl_uint32 * src, igtl_uint32 * dst, igtl_uint32 size)
{
  igtl_uint32 * ptr32_src;
  igtl_uint32 * ptr32_src_end;
  igtl_uint32 * ptr32_dst;

  if (!igtl_is_little_endian())
    {
    memcpy(src, dst, size * sizeof(igtl_uint32));
    }
  else
    {
    ptr32_src = (igtl_uint32 *) src;
    ptr32_src_end = size;
    ptr32_dst = (igtl_uint32 *) dst;
    while (ptr32_src < ptr32_src_end)
      {
      *ptr32_dst = BYTE_SWAP_INT32(*ptr32_src);
      ptr32_dst ++;
      ptr32_src ++;
      }
    }

  return 1;
}


int igtl_export igtl_polydata_unpack(int type, void * byte_array, igtl_polydata_info * info, igtl_uint64 size)
{
  /* size = number of ponits (not number of bytes). In case of vertices, this is specfied 
     by size_vertices in igtl_polydata_header. */
  igtl_polydata_header * header;
  void * ptr;

  igtl_uint16 * ptr16_src;
  igtl_uint16 * ptr16_src_end;
  igtl_uint16 * ptr16_dst;

  igtl_uint32 * ptr32_src;
  igtl_uint32 * ptr32_src_end;
  igtl_uint32 * ptr32_dst;

  igtl_uint64 * ptr64_src;
  igtl_uint64 * ptr64_src_end;
  igtl_uint64 * ptr64_dst;

  if (byte_array == NULL || info == NULL | size == 0)
    {
    return 0;
    }

  /* POLYDATA header */
  header = (igtl_polydata_header *) byte_array;
  if (igtl_is_little_endian())
    {
    info->header.npoints              = BYTE_SWAP_INT32(header->npoints);
    info->header.type_point           = header->type_point;
    info->header.reserved             = header->reserved;
    info->header.nvertices            = BYTE_SWAP_INT32(header->nvertices);
    info->header.size_vertices        = BYTE_SWAP_INT32(header->size_vertices);
    info->header.nlines               = BYTE_SWAP_INT32(header->nlines);
    info->header.size_lines           = BYTE_SWAP_INT32(header->size_lines);
    info->header.npolygons            = BYTE_SWAP_INT32(header->npolygons);
    info->header.size_polygons        = BYTE_SWAP_INT32(header->size_polygons);
    info->header.ntriangle_strips     = BYTE_SWAP_INT32(header->ntriangle_strips);
    info->header.size_triangle_strips = BYTE_SWAP_INT32(header->size_triangle_strips);
    }
  else
    {
    memcpy(info, header, sizeof(igtl_polydata_header));
    }
  
  /* Allocate memory to read data */
  /* TODO: compare the size of info before copying the header. */
  /* If the size doesn't change, avoid reallocation of memory. */
  if (igtl_polydata_alloc_info(info) == 0)
    {
    return 0;
    }
  
  /* POINT serction */
  ptr = byte_array + sizeof(igtl_polydata_header);
  if (igtl_ndarray_get_nbyte(info->header.type_point) == 1 || !igtl_is_little_endian())
    {
    memcpy(info->points, ptr, igtl_polydata_get_nbyte(info->header.type_point)*info->header.npoints*3);
    }
  else if (igtl_ndarray_get_nbyte(info->header.type_point) == 2) /* 16-bit */
    {
    ptr16_src = (igtl_uint16 *) ptr;
    ptr16_src_end = ptr16_src + info->header.npoints*3;
    ptr16_dst = (igtl_uint16 *) info->points;
    while (ptr16_src < ptr16_src_end)
      {
      *ptr16_dst = BYTE_SWAP_INT16(*ptr16_src);
      ptr16_dst ++;
      ptr16_src ++;
      }
    }
  else if (igtl_ndarray_get_nbyte(info->header.type_point) == 4) /* 32-bit */
    {
    ptr32_src = (igtl_uint32 *) ptr;
    ptr32_src_end = ptr32_src + info->header.npoints*3;
    ptr32_dst = (igtl_uint32 *) info->points;
    while (ptr32_src < ptr32_src_end)
      {
      *ptr32_dst = BYTE_SWAP_INT32(*ptr32_src);
      ptr32_dst ++;
      ptr32_src ++;
      }
    }
  else if (igtl_ndarray_get_nbyte(info->header.type_point) == 8) /* 64-bit */
    {
    ptr64_src = (igtl_uint64 *) ptr;
    ptr64_src_end = ptr64_src + info->header.npoints*3;
    ptr64_dst = (igtl_uint64 *) info->points;
    while (ptr64_src < ptr64_src_end)
      {
      *ptr64_dst = BYTE_SWAP_INT64(*ptr64_src);
      ptr64_dst ++;
      ptr64_src ++;
      }
    }
  else
    {
    return 0;
    }
  ptr += igtl_polydata_get_nbyte(info->header.type_point)*info->header.npoints*3;

  /* VERTICES section */
  igtl_polydata_convert_byteorder_topology(info->vertices, ptr, info->header.size_vertices);
  ptr += info->header.size_vertices * sizeof(igtl_uint32);

  /* LINES section */
  igtl_polydata_convert_byteorder_topology(info->lines, ptr, info->header.size_lines);
  ptr += info->header.size_lines * sizeof(igtl_uint32);

  /* POLYGONS section */
  igtl_polydata_convert_byteorder_topology(info->polygons, ptr, info->header.size_polygons);
  ptr += info->header.size_polygons * sizeof(igtl_uint32);

  /* TRIANGLE_STRIPS section */
  igtl_polydata_convert_byteorder_topology(info->triangle_strips, ptr, info->header.size_triangle_strips);
  /*ptr += info->header.size_triangle_strips * sizeof(igtl_uint32);*/

  return 1;
}


int igtl_export igtl_polydata_pack(igtl_polydata_info * info, void * byte_array, int type)
{
  /* size = number of ponits (not number of bytes). In case of vertices, this is specfied 
     by size_vertices in igtl_polydata_header. */
  igtl_polydata_header * header;
  void * ptr;

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

  /* POLYDATA header */
  header = (igtl_polydata_header *) byte_array;
  if (igtl_is_little_endian())
    {
    header->npoints              = BYTE_SWAP_INT32(info->header.npoints);
    header->type_point           = info->header.type_point;
    header->reserved             = info->header.reserved;
    header->nvertices            = BYTE_SWAP_INT32(info->header.nvertices);
    header->size_vertices        = BYTE_SWAP_INT32(info->header.size_vertices);
    header->nlines               = BYTE_SWAP_INT32(info->header.nlines);
    header->size_lines           = BYTE_SWAP_INT32(info->header.size_lines);
    header->npolygons            = BYTE_SWAP_INT32(info->header.npolygons);
    header->size_polygons        = BYTE_SWAP_INT32(info->header.size_polygons);
    header->ntriangle_strips     = BYTE_SWAP_INT32(info->header.ntriangle_strips);
    header->size_triangle_strips = BYTE_SWAP_INT32(info->header.size_triangle_strips);
    }
  else
    {
    memcpy(header, info, sizeof(igtl_polydata_header));
    }
  
  /* POINT serction */
  ptr = byte_array + sizeof(igtl_polydata_header);
  if (igtl_ndarray_get_nbyte(info->header.type_point) == 1 || !igtl_is_little_endian())
    {
    memcpy(info->points, ptr, igtl_polydata_get_nbyte(info->header.type_point)*info->header.npoints*3);
    }
  else if (igtl_ndarray_get_nbyte(info->header.type_point) == 2) /* 16-bit */
    {
    ptr16_src = (igtl_uint16 *) info->points;
    ptr16_src_end = ptr16_src + info->header.npoints*3;
    ptr16_dst = (igtl_uint16 *) ptr;
    while (ptr16_src < ptr16_src_end)
      {
      *ptr16_dst = BYTE_SWAP_INT16(*ptr16_src);
      ptr16_dst ++;
      ptr16_src ++;
      }
    }
  else if (igtl_ndarray_get_nbyte(info->header.type_point) == 4) /* 32-bit */
    {
    ptr32_src = (igtl_uint32 *) info->points;
    ptr32_src_end = info->header.npoints*3;
    ptr32_dst = (igtl_uint32 *) ptr;
    while (ptr32_src < ptr32_src_end)
      {
      *ptr32_dst = BYTE_SWAP_INT32(*ptr32_src);
      ptr32_dst ++;
      ptr32_src ++;
      }
    }
  else if (igtl_ndarray_get_nbyte(info->header.type_point) == 8) /* 64-bit */
    {
    ptr64_src = (igtl_uint64 *) info->points;
    ptr64_src_end = info->header.npoints*3;
    ptr64_dst = (igtl_uint64 *) ptr;
    while (ptr64_src < ptr64_src_end)
      {
      *ptr64_dst = BYTE_SWAP_INT64(*ptr64_src);
      ptr64_dst ++;
      ptr64_src ++;
      }
    }
  else
    {
    return 0;
    }
  ptr += igtl_polydata_get_nbyte(info->header.type_point)*info->header.npoints*3;

  /* VERTICES section */
  igtl_polydata_convert_byteorder_topology(ptr, info->vertices, info->header.size_vertices);
  ptr += info->header.size_vertices * sizeof(igtl_uint32);

  /* LINES section */
  igtl_polydata_convert_byteorder_topology(ptr, info->lines, info->header.size_lines);
  ptr += info->header.size_lines * sizeof(igtl_uint32);

  /* POLYGONS section */
  igtl_polydata_convert_byteorder_topology(ptr, info->polygons, info->header.size_polygons);
  ptr += info->header.size_polygons * sizeof(igtl_uint32);

  /* TRIANGLE_STRIPS section */
  igtl_polydata_convert_byteorder_topology(ptr, info->triangle_strips, info->header.size_triangle_strips);
  /*ptr += info->header.size_triangle_strips * sizeof(igtl_uint32);*/

  return 1;
}


igtl_uint64 igtl_export igtl_polydata_get_size(igtl_polydata_info * info, int type)
{
  igtl_uint64 data_size;

  data_size = sizeof(igtl_polydata_header);
  data_size += igtl_polydata_get_nbyte(info->header.type_point)*info->header.npoints*3;
  data_size += sizeof(igtl_uint32) * 
    (info->header.size_vertices + info->header.size_lines 
     + info->header.size_polygons + info->header.size_triangle_strips);

  return data_size;
}


igtl_uint64 igtl_export igtl_polydata_get_crc(igtl_polydata_info * info, int type, void* polydata_message)
{
  igtl_uint64   crc;
  igtl_uint64   polydata_length;
  igtl_uint16   i;
  igtl_uint16   nc;

  polydata_length = (igtl_uint32)igtl_polydata_get_size(info, type);
  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) polydata_message, (int)polydata_length, crc);

  return crc;
}

