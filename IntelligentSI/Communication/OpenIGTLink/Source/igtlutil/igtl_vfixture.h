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

#ifndef __IGTL_VFIXTURE_H
#define __IGTL_VFIXTURE_H

#include "igtl_util.h"
#include "igtl_types.h"

#define IGTL_VFIXTURE_HEADER_VERSION       1
#define IGTL_VFIXTURE_HEADER_SIZE          sizeof(igtl_vfixture_header)

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

typedef struct {
  igtl_uint16   version;           /* data format version number      */
  igtl_uint16   nspheres;          /* number of spheres               */
  igtl_float32  hardness;          /* hardness parameter              */
} igtl_vfixture_header;

typedef struct {
  igtl_float32  x;                 /* x coordinate (mm) */
  igtl_float32  y;                 /* y coordinate (mm) */
  igtl_float32  z;                 /* z coordinate (mm) */
  igtl_float32  r;                 /* radius       (mm) */
} igtl_vfixture_data;

#pragma pack()

/*
 * Virtual Fixture data size
 *
 * This function calculates size of the pixel array, which will be
 * transferred with the specified header.
 */

igtl_uint32 igtl_vfixture_data_size(igtl_vfixture_header * header);

/*
 * Byte order conversion for the header and data
 *
 * This function converts endianness of each member variable
 * in igtl_vfixture_header from host byte order to network byte order,
 * or vice versa.
 */

void igtl_vfixture_convert_byte_order(igtl_vfixture_header * header);
void igtl_vfixture_data_convert_byte_order(int n, void* data);

/*
 * CRC calculation
 *
 * This function calculates CRC of virtual fixture data body including header
 * and array of pixel data.
 *
 */

igtl_uint64 igtl_vfixture_get_crc(igtl_vfixture_header * header, void* data);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_VFIXTURE_H */
