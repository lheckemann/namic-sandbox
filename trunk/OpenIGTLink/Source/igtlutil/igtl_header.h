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
#define IGTL_HEADER_NAMESIZE  12
#define IGTL_HEADER_DEVSIZE   20

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memory */

/*
 * Message header of OpenIGTLink message.
 *
 * igtl_header is an overall data header for OpenIGTLink protocol.
 * It is transfered at beginning of every OpenIGTLink message to give
 * type and size of following data body to a receiver.
 * These parameters allow the receiver to parse or skip the data body.
 */

typedef struct {
  unsigned short version;          /* protocol version number */
  char           name[IGTL_HEADER_NAMESIZE];          /* data type name          */
  char           device_name[IGTL_HEADER_DEVSIZE];  /* device name             */
  long long      timestamp;        /* time stamp message      */
  long long      body_size;        /* size of the body        */
  long long      crc;              /* CRC                     */
} igtl_header;

#pragma pack()

/*
 * Byte order converter for the header structre
 * 
 * igtl_header_convert_byte_order convers endianness of each
 * member variable in igtl_header structre from host byte order
 * to network byte order, or vice versa.
 *
 */
void igtl_header_convert_byte_order(igtl_header * header);

#ifdef __cplusplus
}
#endif

#endif // __IGTL_HEADER_H

