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

#ifndef __IGTL_SCALARVALUE_H
#define __IGTL_SCALARVALUE_H

#include "igtl_util.h"
#include "igtl_types.h"

#define IGTL_SCALARVALUE_HEADER_VERSION       1
#define IGTL_SCALARVALUE_HEADER_SIZE          72

/* Data type */
#define IGTL_SCALARVALUE_DTYPE_SCALAR         1
#define IGTL_SCALARVALUE_DTYPE_VECTOR         2

/* Scalar type */
#define IGTL_SCALARVALUE_STYPE_TYPE_INT8      2
#define IGTL_SCALARVALUE_STYPE_TYPE_UINT8     3
#define IGTL_SCALARVALUE_STYPE_TYPE_INT16     4
#define IGTL_SCALARVALUE_STYPE_TYPE_UINT16    5
#define IGTL_SCALARVALUE_STYPE_TYPE_INT32     6
#define IGTL_SCALARVALUE_STYPE_TYPE_UINT32    7
#define IGTL_SCALARVALUE_STYPE_TYPE_FLOAT32   10
#define IGTL_SCALARVALUE_STYPE_TYPE_FLOAT64   11

/* Endian */
#define IGTL_SCALARVALUE_ENDIAN_BIG           1
#define IGTL_SCALARVALUE_ENDIAN_LITTLE        2


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/*
 * Scalarvalue data header for OpenIGTLinik protocol
 *
 * Scalarvalue data consists of scalarvalue data header, which is defined in this
 * structure, folowed by array of scalarvalue pixel data.
 * igtl_scalarvalue_header helps a receiver to load array of scalarvalue pixel data.
 * The header supports "partial volume update", where a fraction of volume
 * scalarvalue is transferred from a sender to receiver. This fraction called
 * "sub-volume" in this protocol, and its size and starting index is
 * specified in 'subvol_size' and 'subvol_offset'.
 * In case of transferring entire scalarvalue in one message, 'size' and
 * 'subvol_size' should be same, and 'subvol_offset' equals (0, 0, 0).
 */

typedef struct {
  igtl_uint16    version;          /* data format version number      */
  igtl_uint8     scalar_type;      /* scalar type                     */
  igtl_uint8     endian;           /* endian type of scalarvalue data       */
  igtl_uint16    data_size;   /* sub volume size                 */
} igtl_scalarvalue_header;

#pragma pack()


/*
 * Scalarvalue data size
 *
 * This function calculates size of the pixel array, which will be
 * transferred with the specified header.
 */

igtl_uint64 igtl_scalarvalue_get_data_size(igtl_scalarvalue_header * header);


/*
 * Byte order conversion for the header structure
 *
 * This function converts endianness of each member variable
 * in igtl_scalarvalue_header from host byte order to network byte order,
 * or vice versa.
 */

void igtl_scalarvalue_convert_byte_order(igtl_scalarvalue_header * header);


/*
 * CRC calculation
 *
 * This function calculates CRC of scalarvalue data body including header
 * and array of pixel data.
 *
 */

igtl_uint64 igtl_scalarvalue_get_crc(igtl_scalarvalue_header * header, void* scalarvalue);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_SCALARVALUE_H */
