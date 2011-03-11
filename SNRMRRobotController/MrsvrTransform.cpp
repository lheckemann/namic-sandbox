//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrTransform.cpp,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================


//====================================================================
// Description: 
//    Class to convert from RAS coordinate to robot coordiante.
//====================================================================


#include "MrsvrTransform.h"
#include <math.h>

void PrintMatrix(MrsvrMatrix4x4 &matrix)
{
  std::cout << "=============" << std::endl;
  std::cout << matrix[0][0] << ", " << matrix[0][1] << ", " << matrix[0][2] << ", " << matrix[0][3] << std::endl;
  std::cout << matrix[1][0] << ", " << matrix[1][1] << ", " << matrix[1][2] << ", " << matrix[1][3] << std::endl;
  std::cout << matrix[2][0] << ", " << matrix[2][1] << ", " << matrix[2][2] << ", " << matrix[2][3] << std::endl;
  std::cout << matrix[3][0] << ", " << matrix[3][1] << ", " << matrix[3][2] << ", " << matrix[3][3] << std::endl;
  std::cout << "=============" << std::endl;
}

void QuaternionToMatrix(float* q, MrsvrMatrix4x4& m)
{

  // normalize
  float mod = sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);

  // convert to the matrix
  const float x = q[0] / mod;
  const float y = q[1] / mod; 
  const float z = q[2] / mod; 
  const float w = q[3] / mod;
  
  const float xx = x * x * 2.0;
  const float xy = x * y * 2.0;
  const float xz = x * z * 2.0;
  const float xw = x * w * 2.0;
  const float yy = y * y * 2.0;
  const float yz = y * z * 2.0;
  const float yw = y * w * 2.0;
  const float zz = z * z * 2.0;
  const float zw = z * w * 2.0;
  
  m[0][0] = 1.0 - (yy + zz);
  m[1][0] = xy + zw;
  m[2][0] = xz - yw;
  
  m[0][1] = xy - zw;
  m[1][1] = 1.0 - (xx + zz);
  m[2][1] = yz + xw;
  
  m[0][2] = xz + yw;
  m[1][2] = yz - xw;
  m[2][2] = 1.0 - (xx + yy);

  m[3][0] = 0.0;
  m[3][1] = 0.0;
  m[3][2] = 0.0;
  m[3][3] = 1.0;

  m[0][3] = 0.0;
  m[1][3] = 0.0;
  m[2][3] = 0.0;

}


void MatrixToQuaternion(MrsvrMatrix4x4& m, float* q)
{
  float trace = m[0][0] + m[1][1] + m[2][2];

  if( trace > 0.0 ) {

    float s = 0.5f / sqrt(trace + 1.0f);

    q[3] = 0.25f / s;
    q[0] = ( m[2][1] - m[1][2] ) * s;
    q[1] = ( m[0][2] - m[2][0] ) * s;
    q[2] = ( m[1][0] - m[0][1] ) * s;

  } else {

    if ( m[0][0] > m[1][1] && m[0][0] > m[2][2] ) {

      float s = 2.0f * sqrt( 1.0f + m[0][0] - m[1][1] - m[2][2]);

      q[3] = (m[2][1] - m[1][2] ) / s;
      q[0] = 0.25f * s;
      q[1] = (m[0][1] + m[1][0] ) / s;
      q[2] = (m[0][2] + m[2][0] ) / s;

    } else if (m[1][1] > m[2][2]) {

      float s = 2.0f * sqrt( 1.0f + m[1][1] - m[0][0] - m[2][2]);

      q[3] = (m[0][2] - m[2][0] ) / s;
      q[0] = (m[0][1] + m[1][0] ) / s;
      q[1] = 0.25f * s;
      q[2] = (m[1][2] + m[2][1] ) / s;

    } else {

      float s = 2.0f * sqrt( 1.0f + m[2][2] - m[0][0] - m[1][1] );

      q[3] = (m[1][0] - m[0][1] ) / s;
      q[0] = (m[0][2] + m[2][0] ) / s;
      q[1] = (m[1][2] + m[2][1] ) / s;
      q[2] = 0.25f * s;

    }
  }
}
  

  
void Cross(float *a, float *b, float *c)
{
    a[0] = b[1]*c[2] - c[1]*b[2];
    a[1] = c[0]*b[2] - b[0]*c[2];
    a[2] = b[0]*c[1] - c[0]*b[1];
}


void IdentityMatrix(MrsvrMatrix4x4 &matrix)
{
  matrix[0][0] = 1.0;
  matrix[1][0] = 0.0;
  matrix[2][0] = 0.0;
  matrix[3][0] = 0.0;

  matrix[0][1] = 0.0;
  matrix[1][1] = 1.0;
  matrix[2][1] = 0.0;
  matrix[3][1] = 0.0;

  matrix[0][2] = 0.0;
  matrix[1][2] = 0.0;
  matrix[2][2] = 1.0;
  matrix[3][2] = 0.0;

  matrix[0][3] = 0.0;
  matrix[1][3] = 0.0;
  matrix[2][3] = 0.0;
  matrix[3][3] = 1.0;
}


MrsvrTransform::MrsvrTransform()
{
//  rotMat = Matrix(3, 3);
//  invRotMat = Matrix(3, 3);
//  baseOffset = ColumnVector(3);
//  //tipOffset = ColumnVector(3);
//
//  rotMat.fill(0.0);
//  invRotMat.fill(0.0);
//  baseOffset.fill(0.0);
//  //tipOffset.fill(0.0);
//  //tipOffset = 0.0;
//
//  for (int i = 0; i < 3; i ++) {
//    rotMat(i, i)     = 1.0;
//    invRotMat(i, i)  = 1.0;
//  }
}



MrsvrTransform::~MrsvrTransform()
{
}


void MrsvrTransform::initFiducialPoints()
{
  nFidp = 0;
}


int MrsvrTransform::addFiducialPoints(MrsvrVector x, MrsvrVector r)
{
  if (nFidp < MAX_NFDIP) {
    for (int i = 0; i < 3; i ++) {
      fidpRbt[nFidp][i] = x[i];
      fidpImg[nFidp][i] = r[i];
    }
    nFidp ++;
    return nFidp;
  } else {
#ifdef DEBUG_MRSVR_TRANSFORM
    cout << "MrsvrTransform::addFiducialPoints(): Number of iducial points reaches maximum.\n";
#endif    
    return 0;
  }
}


int MrsvrTransform::calibrate()
{
  return 1;
  
}


void MrsvrTransform::setCalibrationMatrix(float* m)
{
  for (int i = 0; i < 4; i ++) {
    for (int j = 0; j < 4; j ++) {
      this->matrix[i][j] = m[i*4+j];
    }
  }
}


void MrsvrTransform::transform(MrsvrVector y, MrsvrVector x)
{
  // not implemented.
  for (int i = 0; i < 3; i ++) {
    y[i] = x[i];
  }
}


void MrsvrTransform::invTransform(MrsvrVector x, MrsvrVector y) 
{
  // not implemented.
  for (int i = 0; i < 3; i ++) {
    x[i] = y[i];
  }
} 


