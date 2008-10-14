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

#include "igtlTransformMessage.h"

#include "igtl_header.h"
#include "igtl_transform.h"

namespace igtl {

TransformMessage::TransformMessage()
{
  m_BodyType = "TRANSFORM";

  AllocatePack();
  m_Transform = m_Body;
}

TransformMessage::~TransformMessage()
{
}

void TransformMessage::SetPosition(float p[3])
{
  matrix[0][3] = p[0];
  matrix[1][3] = p[1];
  matrix[2][3] = p[2];
}

void TransformMessage::SetPosition(float px, float py, float pz)
{
  matrix[0][3] = px;
  matrix[1][3] = py;
  matrix[2][3] = pz;
}

void TransformMessage::SetNormals(float o[3][3])
{
  matrix[0][0] = o[0][0];
  matrix[0][1] = o[0][1];
  matrix[0][2] = o[0][2];
  matrix[1][0] = o[1][0];
  matrix[1][1] = o[1][1];
  matrix[1][2] = o[1][2];
  matrix[2][0] = o[2][0];
  matrix[2][1] = o[2][1];
  matrix[2][2] = o[2][2];
}

void TransformMessage::SetNormals(float t[3], float s[3], float n[3])
{
  matrix[0][0] = t[0];
  matrix[1][0] = t[1];
  matrix[2][0] = t[2];
  matrix[0][1] = s[0];
  matrix[1][1] = s[1];
  matrix[2][1] = s[2];
  matrix[0][2] = n[0];
  matrix[1][2] = n[1];
  matrix[2][2] = n[2];
}

void TransformMessage::SetMatrix(Matrix4x4& mat)
{
  matrix[0][0] = mat[0][0];
  matrix[1][0] = mat[1][0];
  matrix[2][0] = mat[2][0];
  matrix[0][1] = mat[0][1];
  matrix[1][1] = mat[1][1];
  matrix[2][1] = mat[2][1];
  matrix[0][2] = mat[0][2];
  matrix[1][2] = mat[1][2];
  matrix[2][2] = mat[2][2];
  matrix[0][3] = mat[0][3];
  matrix[1][3] = mat[1][3];
  matrix[2][3] = mat[2][3];
}

int TransformMessage::GetBodyPackSize()
{
  return IGTL_TRANSFORM_SIZE;
}

void TransformMessage::PackBody()
{
  igtl_float32* transform = (igtl_float32*)this->m_Transform;

  for (int i = 0; i < 3; i ++) {
    transform[i]   = matrix[i][0];
    transform[i+3] = matrix[i][1];
    transform[i+6] = matrix[i][2];
    transform[i+9] = matrix[i][3];
  }
  
  igtl_transform_convert_byte_order(transform);
  
}


} // namespace igtl





