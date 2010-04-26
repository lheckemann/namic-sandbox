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

#ifndef __IGTL_TRAJECTORY_H
#define __IGTL_TRAJECTORY_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"

#define  IGTL_TRAJECTORY_ELEMENT_SIZE           149

#define IGTL_TRAJECTORY_TYPE_ENTRY_ONLY          1
#define IGTL_TRAJECTORY_TYPE_TARGET_ONLY         2
#define IGTL_TRAJECTORY_TYPE_BOTH                3

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */
/*
 * Status data header for OpenIGTLinik protocol
 *
 */

typedef struct {
  igtl_int8    name[64];          /* Name or description of the trajectory */
  igtl_int8    group_name[32];    /* Can be "Trajectory",  ... */
  igtl_int8    type;              /* Trajectory type (see IGTL_TRAJECTORY_TYPE_* macros) */
  igtl_int8    rgba[4];           /* Color in R/G/B/A */
  igtl_float32 entry_pos[3];      /* Coordinate of the entry point */
  igtl_float32 target_pos[3];     /* Coordinate of the target point */
  igtl_float32 radius;            /* Radius of the trajectory. Can be 0. */
  igtl_int8    owener_name[20];   /* Device name of the ower image */
} igtl_trajectory_element;

#pragma pack()


/*
 * Macros for trajectory data size
 *
 * igtl_trajectory_get_data_size(n) calculates the size of body based on the number
 * of trajectorys. The size of body is used in the message header.
 * igtl_trajectory_get_data_n(size) calculates the number of images in the body, based on
 * the body size. This function may be used when a client program parses a TRAJECTORY message.
 *
 */

#define igtl_trajectory_get_data_size(n)  ((n) * IGTL_TRAJECTORY_ELEMENT_SIZE)
#define igtl_trajectory_get_data_n(size)  ((size) / IGTL_TRAJECTORY_ELEMENT_SIZE)

/*
 * Byte order conversion for an array of trajectory data structure
 *
 * This function converts endianness of each element in an array of
 * igtl_igtl_trajectory_element from host byte order to network byte order,
 * or vice versa.
 */

void igtl_export igtl_trajectory_convert_byte_order(igtl_trajectory_element** trajectorylist, int nelem);


/*
 * CRC calculation
 *
 * This function calculates CRC of trajectory message
 *
 */

igtl_uint64 igtl_export igtl_trajectory_get_crc(igtl_trajectory_element** trajectorylist, int nelem);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_POSITION_H */

