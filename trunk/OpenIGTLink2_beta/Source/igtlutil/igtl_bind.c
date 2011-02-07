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

#include <stdlib.h>
#include <string.h>

#include "igtl_bind.h"
#include "igtl_util.h"


void igtl_export igtl_bind_init_info(igtl_bind_info * bind_info)
{
  if (bind_info)
    {
    bind_info->ncmessages = 0;
    bind_info->child_info_array = NULL;
    }
}


int igtl_export igtl_bind_alloc_info(igtl_bind_info * bind_info, igtl_uint16 ncmessages)
{
  size_t size;
  
  if (bind_info == NULL)
    {
    return 0;
    }

  size = (size_t) ncmessages * sizeof(igtl_bind_child_info);
  bind_info->child_info_array = malloc(size);

  if (bind_info->child_info_array != NULL)
    {
    bind_info->ncmessages = ncmessages;
    memset(bind_info->child_info_array, 0, size);
    return 1;
    }
  else
    {
    bind_info->ncmessages = 0;
    return 0;
    }
}


int igtl_export igtl_bind_free_info(igtl_bind_info * bind_info)
{

  if (bind_info == NULL)
    {
    return 0;
    }
  
  if (bind_info->ncmessages == 0 || bind_info->child_info_array == NULL)
    {
    bind_info->ncmessages == 0;
    return 1;
    }

  free(bind_info->child_info_array);
  return 1;

}


int igtl_export igtl_bind_unpack(void * byte_array, igtl_bind_info * info)
{
  igtl_uint16 i;
  igtl_uint16 ncmessages;
  igtl_uint16 nametable_size;
  size_t      namelen;
  void * ptr;
  void * ptr2;
  
  if (byte_array == NULL || info == NULL)
    {
    return 0;
    }
  
  /* Number of child messages */
  ptr = (void *) byte_array;
  if (igtl_is_little_endian())
    {
    ncmessages = BYTE_SWAP_INT16(*((igtl_uint16*)ptr));
    }
  else
    {
    ncmessages = *((igtl_uint16*)ptr);
    }
  
  /* Allocate an array of bind_info, if neccessary */
  if (ncmessages != info->ncmessages)
    {
    if (igtl_bind_alloc_info(info, ncmessages) == 0)
      {
      return 0;
      }
    }
  
  /* Pointer to the first element in the BIND header section */
  ptr += sizeof(igtl_uint16);
  
  /* Extract types and body sizes from the BIND header section */
  for (i = 0; i < ncmessages; i ++)
    {
    /* Type of child message */
    strncpy(info->child_info_array[i].type, (char*)ptr, IGTL_HEADER_TYPE_SIZE);
    info->child_info_array[i].type[IGTL_HEADER_TYPE_SIZE] = '\0';
    
    /* Body size of child message */
    ptr += IGTL_HEADER_TYPE_SIZE;
    if (igtl_is_little_endian())
      {
      info->child_info_array[i].size = BYTE_SWAP_INT64(*((igtl_uint64 *) ptr));
      }
    else
      {
      info->child_info_array[i].size = *((igtl_uint64 *) ptr);
      }

    ptr += sizeof(igtl_uint64);
    }

  /* Name table size */
  if (igtl_is_little_endian())
    {
    nametable_size = BYTE_SWAP_INT16(*((igtl_uint16 *) ptr));
    }
  else
    {
    nametable_size = *((igtl_uint16 *) ptr);
    }

  /*
   * Check if the name table field is aligned to WORD (The length of
   * the field must be even. 
   */
  if (nametable_size % 2 != 0)
    {
    return 0;
    }
  
  ptr += sizeof(igtl_uint16);
  ptr2 = ptr;

  /* Extract device names from the name table section */
  if (nametable_size > 0)
    {
    for (i = 0; i < ncmessages; i ++)
      {
      strncpy(info->child_info_array[i].name, ptr, IGTL_HEADER_NAME_SIZE);
      info->child_info_array[i].name[IGTL_HEADER_NAME_SIZE] = '\n';
      namelen = strlen(info->child_info_array[i].name);
      ptr += namelen + 1;
      }
    }

  ptr = ptr2 + nametable_size;

  /* Set pointers to the child message bodies */
  for (i = 0; i < ncmessages; i ++)
    {
    info->child_info_array[i].ptr = ptr;
    ptr += info->child_info_array[i].size;
    }
    
  /** TODO: check the total size of the message? **/

  return 1;
}


int igtl_export igtl_bind_pack(igtl_bind_info * info, void * byte_array)

{
  void * ptr;
  igtl_uint32 i;
  igtl_uint32 nc;
  igtl_uint16 * nts;
  size_t wb;
  size_t len;

  ptr = byte_array;
  nc = info->ncmessages;

  /* Validate info */
  if (info->ncmessages == 0 || info->child_info_array == NULL)
    {
    return 0;
    }
  
  /* Number of child messages */
  if (igtl_is_little_endian())
    {
    *((igtl_uint16*) ptr) = BYTE_SWAP_INT16(info->ncmessages);
    }
  else
    {
    *((igtl_uint16*) ptr) = info->ncmessages;
    }
  ptr += sizeof(igtl_uint16);


  /* BIND header section */
  for (i = 0; i < nc; i ++)
    {
    /* Fill with 0 */
    memset(ptr, 0, IGTL_HEADER_TYPE_SIZE);
    strncpy((char*)ptr, info->child_info_array[i].type, IGTL_HEADER_TYPE_SIZE);
    ptr += IGTL_HEADER_TYPE_SIZE;
    if (igtl_is_little_endian())
      {
      *((igtl_uint64 *) ptr) = BYTE_SWAP_INT64(info->child_info_array[i].size);
      }
    else
      {
      *((igtl_uint64 *) ptr) = info->child_info_array[i].size;
      }
    ptr += sizeof(igtl_uint64);
    }


  /* Name table section */
  nts = ptr; /* save address for name table size field */
  ptr += sizeof(igtl_uint16);

  wb = 0;
  for (i = 0; i < nc; i ++)
    {
    len = strlen(info->child_info_array[i].name);
    if (len > IGTL_HEADER_NAME_SIZE)
      {
      return 0;
      }
    /* copy string + NULL-terminator */
    strncpy(ptr, info->child_info_array[i].name, IGTL_HEADER_NAME_SIZE);
    ptr += (len + 1);
    wb += len + 1;
    }

  /* Add padding if the size of the name table section is not even */
  if (wb % 2 > 0)
    {
    *((igtl_uint8*)ptr) = 0;
    ptr ++;
    wb ++;
    }

  /* Substitute name table size */
  *nts = (igtl_uint16) wb;
  if (igtl_is_little_endian())
    {
    *nts = BYTE_SWAP_INT16(*nts);
    }

  return 1;

}


igtl_uint32 igtl_export igtl_bind_get_size(igtl_bind_info * info)
{
  igtl_uint32 size;
  igtl_uint32 ntable_size;
  igtl_uint16 i;
  igtl_uint16 nc;
  
  nc = info->ncmessages;

  /* Size of NCMESSAGES section */
  size = sizeof(igtl_uint16);

  /* Add size of BIND header section */
  size += (IGTL_HEADER_TYPE_SIZE+sizeof(igtl_uint64)) * nc;

  /* Add size of table size field*/
  size += sizeof(igtl_uint16);

  /* Calculate size of name table */
  ntable_size = 0;
  for (i = 0; i < nc; i ++)
    {
    /* string length + NULL separator */
    ntable_size += strlen(info->child_info_array[i].name) + 1;
    }
  if (ntable_size %2 > 0)
    {
    ntable_size += 1;
    }
  size += ntable_size;

  return size;

}


igtl_uint64 igtl_export igtl_bind_get_crc(igtl_bind_info * info, void* bind_message)
{
  igtl_uint64   crc;
  igtl_uint64   bind_length;
  igtl_uint16   i;
  igtl_uint16   nc;

  bind_length = (igtl_uint32)igtl_bind_get_size(info);
  
  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) bind_message, (int)bind_length, crc);
  
  nc = info->ncmessages;
  for (i = 0; i < nc; i ++)
    {
    crc = crc64((unsigned char*) info->child_info_array[i].ptr, (int)info->child_info_array[i].size, crc);
    }

  return crc;
}

