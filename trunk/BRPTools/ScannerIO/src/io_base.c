/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*
 * File Reader/Writer  common routines
 * 
 * Written by J.Tokuda (junichi@atre.t.u-tokyo.ac.jp)
 *
 * Descrpition:
 * MR image reader for ANALYZE image file format
 *
 */

#include "io_base.h"
#include <string.h>

int readHostChar(FILE *fp, char *dest)
{
  if (fread(dest, sizeof(char), 1, fp) != 1) {
    return 0;
  }
  return 1;
}

int readHostInt(FILE *fp, igtl_int32 *dest)
{
  igtl_uint32 tmp;
  
  if (fread(&tmp, sizeof(igtl_uint32), 1, fp) != 1) {
    return 0;
  }
  tmp = ntohl(tmp);
  memcpy(dest, &tmp, sizeof(igtl_uint32));
  return 1;
}


int readHostUShort(FILE *fp, igtl_uint16 * dest)
{
  igtl_uint16 tmp;
  if (fread(&tmp, sizeof(igtl_uint16), 1, fp) != 1) {
    return 0;
  }
  tmp = ntohs(tmp);
  *dest = tmp;
  return 1;
}


int readHostShort(FILE *fp, igtl_int16 * dest)
{
  igtl_uint16 tmp;
  if (fread(&tmp, sizeof(igtl_uint16), 1, fp) != 1) {
    return 0;
  }
  tmp = ntohs(tmp);
  memcpy(dest, &tmp, sizeof(igtl_uint16));
  return 1;
}


int readHostFloat(FILE *fp, igtl_float32 *dest)
{
  igtl_uint32 tmp;

  if (fread(&tmp, sizeof(igtl_uint32), 1, fp) != 1) {
    return 0;
  }
  tmp = ntohl(tmp);
  memcpy(dest, &tmp, sizeof(igtl_uint32));
  return 1;

}


