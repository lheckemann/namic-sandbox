/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: $
  Language:  C
  Date:      $Date: 2010-07-21 14:15:29 -0400 (Wed, 21 Jul 2010) $
  Version:   $Revision: 6836 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <bind.h>
#include <math.h>

#include "igtl_bind.h"
#include "igtl_util.h"


int igtl_export igtl_bind_unpack(void * byte_array, igtl_bind_message_info * message_info)
{

  int i;
  void * ptr;

  ptr = (void *) byte_array;
  message_info->nelement = BYTE_SWAP_INT16(*((igtl_uint16*)ptr));
  ptr += sizeof(igtl_uint16);
  message_info->element_array_ptr = (igtl_bind_header_eleement *) ptr;
  ptr += sizeof(igtl_bind_header_element) * message_info->nelement;
  message_info->name_table_size = BYTE_SWAP_INT16(*((igtl_uint16 *) ptr));
  /* TODO: check if name_table_size is even (aligned to WORD)? */

  ptr += sizeof(igtl_uint16);
  message_info->name_table = (const char *) ptr;
  ptr += message_info->name_table_size;
  message_info->data_ptr = (void* ) ptr;

  igtl_bind_convert_byte_order(message_info, IGTL_BIND_NETWORK_TO_HOST);
  
}


int igtl_export igtl_bind_pack(igtl_bind_message_info * message_info, void * byte_array)
{
  int i;
  int nelem;
  igtl_bind_header_element *elem;
  void * ptr;

  ptr = byte_array;
  memcpy(ptr, message_info->nelement, sizeof(igtl_uint16));
  ptr += sizeof(igtl_uint16);
  nelem = (int) message_info->nelement;
  memcpy(ptr, message_info->element_array_ptr, sizeof(igtl_bind_header_element)*nelem);

  for (i = 0; i < nelem; i ++)
    {
    }

  ptr += sizeof(igtl_bind_header_element)*message_info->nelement;
  memcpy(ptr, message_info->name_table_size, sizeof(igtl_uint16));
  ptr += sizeof(igtl_uint16);
  memcpy(ptr, message_info->name_table_ptr, message_info->name_table_size);
  ptr += message_info->name_table_size;

  
  

}








igtl_uint32 igtl_export igtl_bind_get_bind_length(igtl_bind_header * header)
{
 
  igtl_uint32 length;
  length = (igtl_uint32)(header->length);
  
  return length;
}


void igtl_export igtl_bind_convert_byte_order(igtl_bind_header* header)
{
  int i;

  if (igtl_is_little_endian()) 
    {
    header->encoding = BYTE_SWAP_INT64(header->encoding);
    header->length   = BYTE_SWAP_INT64(header->length);
    }
}


igtl_uint64 igtl_export igtl_bind_get_crc(igtl_bind_header * header, void* bind)
{
  igtl_uint64   crc;
  igtl_uint64   bind_length;

  bind_length = (igtl_uint32)(header->length);

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) header, IGTL_BIND_HEADER_SIZE, crc);
  crc = crc64((unsigned char*) bind, (int)bind_length, crc);

  return crc;
}

