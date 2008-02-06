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

#include <math.h>
#include "igtlMath.h"

namespace igtl {

void QuaternionToMatrix(float* q, Matrix4x4& m)
{
  // normalize
  float mod = sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
  q[0] = q[0] / mod;
  q[1] = q[1] / mod;
  q[2] = q[2] / mod;
  q[3] = q[3] / mod;

  // convert to the matrix
  const float x(q[0]);
  const float y(q[1]);
  const float z(q[2]);
  const float w(q[3]);
  
  const float s( 2.0 / (x * x + y * y + z * z + w * w) );
  
  const float xx(x * x * s);
  const float xy(x * y * s);
  const float xz(x * z * s);
  const float xw(x * w * s);
  
  const float yy(y * y * s);
  const float yz(y * z * s);
  const float yw(y * w * s);
  
  const float zz(z * z * s);
  const float zw(z * w * s);
  
  m[0][0] = 1.0 - (yy + zz);
  m[1][0] = xy + zw;
  m[2][0] = xz - yw;
  
  m[0][1] = xy - zw;
  m[1][1] = 1.0 - (xx + zz);
  m[2][1] = yz + xw;
  
  m[0][2] = xz + yw;
  m[1][2] = yz - xw;
  m[2][2] = 1.0 - (xx + yy);
  
  m[3][3] = 1;
}

} // namespace igtl
