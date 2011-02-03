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

#define IGTL_BIND_HOST_TO_NETWORK      0
#define IGTL_BIND_NETWORK_TO_HOST      1


#ifdef __cplusplus
extern "C" {
#endif


/*   
 * The igtl_bind_info structure holds information about a child message in an OpenIGTLink
 * BIND message. The structure is used for functions defined in igtl_bind.h
 */

typedef struct {
  char             type[IGTL_HEADER_TYPE_SIZE+1];  /* Data type (OpenIGTLink Device Type string) */
  char             name[IGTL_HEADER_NAME_SIZE+1];   /* Device name */
  igtl_uint64      size;                          /* Data size */
  void*            ptr;                           /* Pointer to the child message */
} igtl_bind_child_info;

typedef struct {
  igtl_uint16             ncmessages;             /* Number of child message */
  igtl_bind_child_info *  child_info_array;       /* Array of igtl_bind_child_info */
} igtl_bind_info;


/*
 * Initialize igtl_bind_info
 */
void igtl_export igtl_bind_init_info(igtl_bind_info * bind_info);

/*
 * Allocate / free an array of igtl_bind_info structure
 *
 * Allocate / free an array of igtl_bind_child_info in bind_info with length of 'ncmessages.'
 * Return 1 if the array is successfully allocated/freed
 */

int igtl_export igtl_bind_alloc_info(igtl_bind_info * bind_info, igtl_uint16 ncmessages);
int igtl_export igtl_bind_free_info(igtl_bind_info * bind_info);

/*
 * Unpack BIND message
 *
 * Extract information about child messages in a byte array of BIND messages and store
 * it in a igtl_bind_info structure. Returns 1 if success, otherwise 0.
 */

int igtl_export igtl_bind_unpack(void * byte_array, igtl_bind_info * info);


/*
 * Pack BIND message
 *
 * Convert an igtl_bind_info structure to a byte array. 
 * 'byte_array' should be allocated prior to calling igtl_bind_pack() with memory size
 * calculated by igtl_bind_get_size().
 * Returns 1 if success, otherwise 0.
 */

int igtl_export igtl_bind_pack(igtl_bind_info * info, void * byte_array);



/*
 * Bind data size
 *
 * igtl_bind_get_size() calculates the size of bind header, consisting of
 * BIND hearder section (including number of child messages) and
 * name table section based on a igtl_bind_header.
 */

igtl_uint32 igtl_export igtl_bind_get_size(igtl_bind_info * info);


/*
 * CRC calculation
 *
 * This function calculates CRC of BIND message. Note that 'info' is used only for
 * getting size of the message.
 *
 */

igtl_uint64 igtl_export igtl_bind_get_crc(igtl_bind_info * info, void* bind_message);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_BIND_H */


