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

#ifndef __IGTL_BIND_H
#define __IGTL_BIND_H

#include "igtl_win32header.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#define IGTL_BIND_HEADER_SIZE          1
#define IGTL_BIND_HEADER_ELEMENT_SIZE  20

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

typedef struct {
  igtl_uint16      nelement;  /* Number of elements */
} igtl_bind_header;

typedef struct {
  igtl_uint8       name[IGTL_HEADER_NAMESIZE]; /* Data type (OpenIGTLink Device Type string) */
  igtl_uint8       data_size;                  /* Data size */
} igtl_bind_header_element;

#pragma pack()

/*
 * Bind data size
 *
 * This function calculates size of the pixel array, which will be
 * transferred with the specified header.
 */

igtl_uint32 igtl_export igtl_bind_get_bind_length(igtl_bind_header * header);

/*
 * Byte order conversion
 *
 * This function converts endianness from host byte order to network byte order,
 * or vice versa.
 * NOTE: It is developer's responsibility to have the bind body with BOM
 * (byte order mark) or in big endian ordrer.
 */

void igtl_export igtl_bind_convert_byte_order(igtl_bind_header * header);

/*
 * CRC calculation
 *
 * This function calculates CRC of image data body including header
 * and array of pixel data.
 *
 */

igtl_uint64 igtl_export igtl_bind_get_crc(igtl_bind_header * header, void* bind);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_STRING_H */


