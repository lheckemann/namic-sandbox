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

#ifndef __IGTL_TRANSFORM_H
#define __IGTL_TRANSFORM_H

#include "igtl_util.h"

#define IGTL_TRANSFORM_SIZE   48

#ifdef __cplusplus
extern "C" {
#endif

/*
typedef float[12] transform;
*/


void igtl_transform_convert_byte_order(float* transform);

#ifdef __cplusplus
}
#endif
#endif /*__IGTL_TRANSFORM_H*/


