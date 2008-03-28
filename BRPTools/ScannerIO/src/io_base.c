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

int readHostInt(FILE *fp, int32_t *dest)
{
  uint32_t tmp;
  
  if (fread(&tmp, sizeof(uint32_t), 1, fp) != 1) {
    return 0;
  }
  tmp = ntohl(tmp);
  memcpy(dest, &tmp, sizeof(uint32_t));
  return 1;
}


int readHostUShort(FILE *fp, uint16_t * dest)
{
  uint16_t tmp;
  if (fread(&tmp, sizeof(uint16_t), 1, fp) != 1) {
    return 0;
  }
  tmp = ntohs(tmp);
  *dest = tmp;
  return 1;
}


int readHostShort(FILE *fp, uint16_t * dest)
{
  uint16_t tmp;
  if (fread(&tmp, sizeof(uint16_t), 1, fp) != 1) {
    return 0;
  }
  tmp = ntohs(tmp);
  memcpy(dest, &tmp, sizeof(uint16_t));
  return 1;
}


int readHostFloat(FILE *fp, float32_t *dest)
{
  uint32_t tmp;

  if (fread(&tmp, sizeof(uint32_t), 1, fp) != 1) {
    return 0;
  }
  tmp = ntohl(tmp);
  memcpy(dest, &tmp, sizeof(uint32_t));
  return 1;

}


