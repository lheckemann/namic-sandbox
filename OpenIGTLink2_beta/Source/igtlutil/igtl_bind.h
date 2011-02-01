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

#pragma pack(1)     /* For 1-byte boundary in memroy */

typedef struct {
  igtl_uint8       type[IGTL_HEADER_TYPESIZE]; /* Data type (OpenIGTLink Device Type string) */
  igtl_uint8       data_size;                  /* Data size */
} igtl_bind_header_element;

typedef struct {
  /** Bind header section**/
  igtl_uint16                 nelement;            /* Number of elements */
  igtl_bind_header_element *  element_array_ptr;   /* Pointer to the first bind header element */
  /** Name table section **/
  igtl_uint16                 name_table_size;     /* Size of name table */
  const char *                name_table_ptr;      /* Name table */
  /** Data section **/
  void *                      data_ptr;            /* Pointer to the first body  in the bind */
} igtl_bind_message_info;

#pragma pack()


/*
 * Unpack BIND message
 *
 * Convert raw byte array received through OpenIGTLink connection to igtl_bind_message structure.
 * Note that the original raw byte array will be overwritten by the unpacked message, if byte_array and
 * message point the same address. This function will overwrite the original byte_array with
 * convereted date. Returns 1 if success, otherwise 0.
 */

int igtl_export igtl_bind_unpack(void * byte_array, igtl_bind_message_info * message_info);


/*
 * Pack BIND message
 *
 * Convert an igtl_bind_message structure to a byte array. 
 * 'byte_array' should be allocated prior to calling igtl_bind_pack() with memory size
 * calculated by igtl_bind_get_size().
 * Returns 1 if success, otherwise 0.
 */

int igtl_export igtl_bind_pack(igtl_bind_message_info ** message_info, void * byte_array);


/*
 * Bind data size
 *
 * igtl_bind_get_size() calculates the size of the bind message
 * (including the bind header) based on a igtl_bind_header and
 * an array of igtl_bind_header_elements.
 */

igtl_uint32 igtl_export igtl_bind_get_size(igtl_bind_message_info * message_info);


/*
 * Device name of N-th message
 *
 * igtl_bind_get_device_name() is used to obtain a device name of
 * the N-th message in the list of device names in the bind message.
 * N is specified with 'index' argument.
 * Returns 0, if the name cannot be obtained.
 */

const char* igtl_export igtl_bind_get_nth_device_name(igtl_bind_message_info * message_info, int index);


/*
 * Size of N-th message body
 *
 * igtl_bind_get_size() gets the size of the N-th message in
 * the data section. N is specified with 'index' argument.
 * Returns NULL, if the message size cannot be calculated.
 */

const char* igtl_export igtl_bind_get_nth_message_size(igtl_bind_message_info * message,  int index);


/*
 * Pointer to the N-th message body
 *
 * igtl_bind_get_bind_get_size() gets the size of the N-th message in
 * the data section. N is specified with 'index' argument.
 * Returns NULL, if the message is not available
 */

void * igtl_export igtl_bind_get_nth_message_body(igtl_bind_mesage_info * message_info, int index);


/*
 * Byte order conversion
 *
 * This function converts endianness from host byte order to network byte order,
 * or vice versa. Note that igtl_bind_convert_byte_order only converts byte oder in
 * message->nelement, message->element_array, message->name_table_size entries.
 * message->nelement->data section should be converented separately.
 * Direction of conversion (IGTL_BIND_HOST_TO_NETWORK or IGTL_BIND_NETWORK_TO_HOST)
 * must be specified in 'dir' when the function is called.
 */

void igtl_export igtl_bind_convert_byte_order(igtl_bind_message_info * message, int dir);


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

#endif /* __IGTL_BIND_H */


