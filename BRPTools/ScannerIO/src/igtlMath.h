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

#ifndef __IGTL_MATH_H
#define __IGTL_MATH_H

namespace igtl {

typedef float  Matrix4x4[4][4];

void QuaternionToMatrix(float* q, Matrix4x4& m);

}

#endif // __IGTL_MATH_H



