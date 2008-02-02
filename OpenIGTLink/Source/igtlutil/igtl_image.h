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

#ifndef __IGTL_IMAGE_H
#define __IGTL_IMAGE_H

#include "igtl_util.h"

#define IGTL_IMAGE_HEADER_VERSION  1
#define IGTL_IMAGE_HEADER_SIZE     72

/* Data type */
#define IGTL_IMAGE_DTYPE_SCALAR  1
#define IGTL_IMAGE_DTYPE_VECTOR  2

/* Scalar type */
#define IGTL_IMAGE_STYPE_TYPE_INT8      2
#define IGTL_IMAGE_STYPE_TYPE_UINT8     3
#define IGTL_IMAGE_STYPE_TYPE_INT16     4
#define IGTL_IMAGE_STYPE_TYPE_UINT16    5
#define IGTL_IMAGE_STYPE_TYPE_INT32     6
#define IGTL_IMAGE_STYPE_TYPE_UINT32    7

/* Endian */
#define IGTL_IMAGE_ENDIAN_BIG       1
#define IGTL_IMAGE_ENDIAN_LITTLE    2

/* Image coordinate system */
#define IGTL_IMAGE_COORD_RAS           1
#define IGTL_IMAGE_COORD_LPS           2

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

typedef struct {
  unsigned short version;
  unsigned char  data_type;
  unsigned char  scalar_type;
  unsigned char  endian;
  unsigned char  coord;
  unsigned short size[3];
  float          matrix[12];
  unsigned short subvol_offset[3];
  unsigned short subvol_size[3];
} igtl_image_header;

#pragma pack(0)


/** FIXME: Documentation needed here */
long long igtl_image_get_data_size(igtl_image_header * header);

/** FIXME: Documentation needed here */
void igtl_image_get_matrix(float spacing[3], float origin[3],
                            float norm_i[3], float norm_j[3], float norm_k[3],
                            igtl_image_header * header);


/** FIXME: Documentation needed here */
void igtl_image_convert_byte_order(igtl_image_header * header);

/** FIXME: Documentation needed here */
unsigned long igtl_image_get_crc(igtl_image_header * header, void* image);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_IMAGE_H */
