/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: $
  Language:  C
  Date:      $Date: 2010-11-23 14:47:40 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 6958 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_NDARRAY_H
#define __IGTL_NDARRAY_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#define IGTL_NDARRAY_HEADER_SIZE          2

/* Scalar type */
#define IGTL_NDARRAY_STYPE_TYPE_INT8      2
#define IGTL_NDARRAY_STYPE_TYPE_UINT8     3
#define IGTL_NDARRAY_STYPE_TYPE_INT16     4
#define IGTL_NDARRAY_STYPE_TYPE_UINT16    5
#define IGTL_NDARRAY_STYPE_TYPE_INT32     6
#define IGTL_NDARRAY_STYPE_TYPE_UINT32    7
#define IGTL_NDARRAY_STYPE_TYPE_FLOAT32   10
#define IGTL_NDARRAY_STYPE_TYPE_FLOAT64   11

#define IGTL_NDARRAY_HOST_TO_NETWORK      0
#define IGTL_NDARRAY_NETWORK_TO_HOST      1


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/*
 * NDARRAY is a data type, which is designed to transfer N-dimensional numerical array.
 * The message body consists of N-D array header, size table, and N-D array body.
 */

typedef struct {
  igtl_uint8     type;   /* Scalar type (2:int8 3:uint8 4:int16 5:uint16 6:int32
                            7:uint32 10:float32 11:float64) */
  igtl_uint8     dim;    /* Dimension of array */
} igtl_ndarray_header;

#pragma pack()

/*
 * N-D array data size
 *
 * This function calculates size of N-D array body including
 * size table (defined by UINT16[dim]) and array data.
 * The second argument, 'size' can be either an indepenedent size array or
 * the first part of the N-D array body.
 */

igtl_uint32 igtl_export igtl_ndarray_get_data_size(igtl_ndarray_header * header, igtl_uint16 * size);

/*
 * Byte order conversion
 *
 * This function converts endianness from host byte order to network byte order,
 * or vice versa. Direction of conversion (IGTL_NDARRAY_HOST_TO_NETWORK or
 * IGTL_NDARRAY_NETWORK_TO_HOST) must be specified in 'dir' when the function is called.
 */

void igtl_export igtl_ndarray_convert_byte_order(igtl_ndarray_header * header, void* body, int dir);


/*
 * CRC calculation
 *
 * This function calculates CRC of image data body including header
 * and array of pixel data.
 *
 */

igtl_uint64 igtl_export igtl_ndarray_get_crc(igtl_ndarray_header * header, void* boid);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_NDARRAY_H */
