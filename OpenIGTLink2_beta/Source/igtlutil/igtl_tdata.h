/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlutil/igtl_position.h $
  Language:  C
  Date:      $Date: 2009-11-13 11:37:44 -0500 (Fri, 13 Nov 2009) $
  Version:   $Revision: 5335 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_TDATA_H
#define __IGTL_TDATA_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"

#define  IGTL_TDATA_ELEMENT_SIZE           34
#define  IGTL_STT_TDATA_SIZE               24
#define  IGTL_RTS_TDATA_SIZE               1

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */
/*
 * Status data header for OpenIGTLinik protocol
 *
 */

typedef struct {
  igtl_int8    name[20];       /* Name of instrument / tracker */
  igtl_int8    type;           /* Can be "Labeled Tdata", "Landmark", Fiducial", ... */
  igtl_int8    reserved;       /* Reserved byte */
  igtl_float32 transform[12];  /* same as TRANSFORM */
} igtl_tdata_element;


typedef struct {
  igtl_int32   resolution;     /* Minimum time between two frames. Use 0 for as fast as possible. */
                               /* If e.g. 50 ms is specified, the maximum update rate will be 20 Hz. */
  igtl_int8    coord_name[20]; /* Name of the coordinate system */
} igtl_stt_tdata;

typedef struct {
  igtl_int8    status;         /* 0: Success 1: Error */
} igtl_rts_tdata;

#pragma pack()

/*
 * Macros for tdata data size
 *
 * igtl_tdata_get_data_size(n) calculates the size of body based on the number
 * of tdatas. The size of body is used in the message header.
 * igtl_tdata_get_data_n(size) calculates the number of images in the body, based on
 * the body size. This function may be used when a client program parses a TDATA message.
 *
 */

#define igtl_tdata_get_data_size(n)  ((n) * IGTL_TDATA_ELEMENT_SIZE)
#define igtl_tdata_get_data_n(size)  ((size) / IGTL_TDATA_ELEMENT_SIZE)

/*
 * Byte order conversion for an array of TDATA and STT_TDATA data structure
 *
 * This function converts endianness of each element in an array of
 * igtl_igtl_tdata_element from host byte order to network byte order,
 * or vice versa.
 */

void igtl_export igtl_tdata_convert_byte_order(igtl_tdata_element** tdatalist, int nelem);
void igtl_export igtl_stt_tdata_convert_byte_order(igtl_stt_tdata* stt_tdata);
void igtl_export igtl_rts_tdata_convert_byte_order(igtl_stt_tdata* stt_tdata);


/*
 * CRC calculation
 *
 * This function calculates CRC of TDATA and STT_TDATA message
 *
 */

igtl_uint64 igtl_export igtl_tdata_get_crc(igtl_tdata_element** tdatalist, int nelem);
igtl_uint64 igtl_export igtl_stt_tdata_get_crc(igtl_stt_tdata* stt_tdata);
igtl_uint64 igtl_export igtl_rts_tdata_get_crc(igtl_stt_tdata* stt_tdata);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_POSITION_H */


