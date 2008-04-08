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

#ifndef _INC_IO_BASE
#define _INC_IO_BASE

/*#define _DEBUG_IO_ANALYZE*/

/*#include <stdint.h>*/
#include <stdio.h>

#include "igtl_types.h"

#ifdef __cplusplus
extern "C" {
#endif


int readHostChar(FILE *fp, char *dest);
int readHostInt(FILE *fp, igtl_int32 *dest);
int readHostUShort(FILE *fp, igtl_uint16 * dest);
int readHostShort(FILE *fp, igtl_int16 * dest);
int readHostFloat(FILE *fp, igtl_float32 *dest);


#ifdef __cplusplus
}
#endif


#endif /* _INC_IO_BASE */
