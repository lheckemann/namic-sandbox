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

#ifndef __IGTL_SENSOR_H
#define __IGTL_SENSOR_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#define IGTL_SENSOR_HEADER_SIZE          10

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/*
 * Image data header for OpenIGTLinik protocol
 *
 * Image data consists of image data header, which is defined in this
 * structure, folowed by array of image pixel data.
 * igtl_image_header helps a receiver to load array of image pixel data.
 * The header supports "partial volume update", where a fraction of volume
 * image is transferred from a sender to receiver. This fraction called
 * "sub-volume" in this protocol, and its size and starting index is
 * specified in 'subvol_size' and 'subvol_offset'.
 * In case of transferring entire image in one message, 'size' and
 * 'subvol_size' should be same, and 'subvol_offset' equals (0, 0, 0).
 */

typedef struct {
  igtl_uint8     larray;           /* Length of array (0-255) */
  igtl_uint8     status;           /* (reserved) sensor status */
  igtl_uint64    unit;             /* Unit */
} igtl_sensor_header;

#pragma pack()

/*
 * Sensor data size
 *
 * This function calculates size of the pixel array, which will be
 * transferred with the specified header.
 */

igtl_uint32 igtl_export igtl_sensor_get_data_size(igtl_sensor_header * header);

/*
 * Byte order conversion
 *
 * This function converts endianness from host byte order to network byte order,
 * or vice versa.
 */

void igtl_export igtl_sensor_convert_byte_order(igtl_sensor_header * header, igtl_float64* data);


/*
 * CRC calculation
 *
 * This function calculates CRC of image data body including header
 * and array of pixel data.
 *
 */

igtl_uint64 igtl_export igtl_sensor_get_crc(igtl_sensor_header * header, igtl_float64* data);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_SENSOR_H */
