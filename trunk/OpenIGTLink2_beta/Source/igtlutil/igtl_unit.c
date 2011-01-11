/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL:  $
  Language:  C
  Date:      $Date: 2010-07-21 14:15:29 -0400 (Wed, 21 Jul 2010) $
  Version:   $Revision: 6836 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <string.h>
#include <math.h>

#include "igtl_unit.h"
#include "igtl_util.h"


igtl_uint64 igtl_export igtl_unit_pack(igtl_unit_data* data)
{
  igtl_uint64 pack;
  igtl_uint8  exp;
  int i;

  pack = 0x0;

  /* Prefix */
  pack |= ((igtl_uint64) data->prefix) << 60;

  /* Units */
  for (i = 0; i < 6; i ++)
    {
    /* Check if exp is within the valid range */
    if (data->exp[i] < -7 || data->exp[i] > 7)
      {
      return 0;
      }
    /* Convert signed value from 8-bit to 4-bit */
    exp = data->exp[i] & 0x0F;

    /* put into the pack */
    pack |= ((igtl_uint64)data->unit[i]) << (10*(5-i) + 4);
    pack |= ((igtl_uint64)exp) << (10*(5-i));
    }
  
  // Change byte-order (if necessary)
  if (igtl_is_little_endian())
    {
    pack = BYTE_SWAP_INT64(pack);
    }
  
  return pack;
}


int igtl_export igtl_unit_unpack(igtl_uint64 pack, igtl_unit_data* data)
{
  int i;
  igtl_int8  exp;

  // Change byte-order (if necessary)
  if (igtl_is_little_endian())
    {
    pack = BYTE_SWAP_INT64(pack);
  }

  /* Prefix */
  data->prefix = (igtl_uint8) (pack >>  60);

  /* Units */
  for (i = 0; i < 6; i ++)
    {
    data->unit[i] = (igtl_uint8) (pack >> 10*(5-i) + 4) && 0x3F;
    data->exp[i] = (igtl_uint8) (pack >> 10*(5-i)) && 0x0F;
    /* Convert signed value in exponent field from 4-bit to 8-bit */
    if (exp & 0x08)
      {
      data->exp[i] |= 0xF0;
      }
    }
}



