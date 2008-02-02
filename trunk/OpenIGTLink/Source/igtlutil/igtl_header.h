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

#ifndef __IGTL_HEADER_H
#define __IGTL_HEADER_H

#define IGTL_HEADER_VERSION   1
#define IGTL_HEADER_SIZE      58

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** FIXME: Documentation needed here */
typedef struct {
  unsigned short version;
  char           name[8];
  char           device_name[20];
  long long      timestamp;
  long long      body_size;
  long long      crc;
} igtl_header;

#pragma pack(0)

/** FIXME: Documentation needed here */
void igtl_header_convert_byte_order(igtl_header * header);

#ifdef __cplusplus
}
#endif

#endif // __IGTL_HEADER_H

