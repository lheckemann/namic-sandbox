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
  //for (int i = 0; i < 4; i ++) {
  //  for (int j = 0; j < 4; j ++) {
  //    this->matrix[i][j] = m[i*4+j];
  //  }
  //}

  Matrix Trz;
  Matrix Tzp;

  Trz.ReSize(4,4);
  Trz = IdentityMatrix(4);
  
  Trz.element(0, 3) = -37.5;
  Trz.element(1, 3) = -47.0;
  Trz.element(2, 3) = 30.0;
  
  // Zframe to patient transform
  Tzp.ReSize(4, 4);
  for (int i = 0; i < 4; i ++) {
    for (int j = 0; j < 4; j ++) {
      Tzp.element(i, j) = m[i*4+j];
    }
  }

  // Calculate robot to patient transform
  Trp.ReSize(4,4);
  Trp = Tzp * Trz;

  // Calculate patient to robot transform
  Tpr.ReSize(4, 4);

  Tpr = Trp.i();

}


// Convert target position from the patient coordinate system to the robot coordinate system
void MrsvrTransform::transform(MrsvrVector x, MrsvrVector y)
{
  //// x = My + t
  //// inv(M) (x - t) = y
  //
  //MrsvrMatrix4x4 inv;
  //
  //// Calculate inverse rotation matrix
  //float det = this->matrix[0][0]*this->matrix[1][1]*this->matrix[2][2]
  //  + this->matrix[0][1]*this->matrix[1][2]*this->matrix[2][0] 
  //  + this->matrix[0][2]*this->matrix[1][0]*this->matrix[2][1]
  //  - this->matrix[0][0]*this->matrix[1][2]*this->matrix[2][1] 
  //  - this->matrix[0][1]*this->matrix[1][0]*this->matrix[2][2]
  //  - this->matrix[0][2]*this->matrix[1][1]*this->matrix[2][0];
  //
  //inv[0][0] = (this->matrix[1][1]*this->matrix[2][2] - this->matrix[1][2]*this->matrix[2][1])/det;
  //inv[0][1] = (this->matrix[0][2]*this->matrix[2][1] - this->matrix[0][1]*this->matrix[2][2])/det;
  //inv[0][2] = (this->matrix[0][1]*this->matrix[1][2] - this->matrix[0][2]*this->matrix[1][1])/det;
  //inv[1][0] = (this->matrix[1][2]*this->matrix[2][0] - this->matrix[1][0]*this->matrix[2][2])/det;
  //inv[1][1] = (this->matrix[0][0]*this->matrix[2][2] - this->matrix[0][2]*this->matrix[2][0])/det;
  //inv[1][2] = (this->matrix[0][2]*this->matrix[1][0] - this->matrix[0][0]*this->matrix[1][2])/det;
  //inv[2][0] = (this->matrix[1][0]*this->matrix[2][1] - this->matrix[1][1]*this->matrix[2][0])/det;
  //inv[2][1] = (this->matrix[0][1]*this->matrix[2][0] - this->matrix[0][0]*this->matrix[2][1])/det;
  //inv[2][2] = (this->matrix[0][0]*this->matrix[1][1] - this->matrix[0][1]*this->matrix[1][0])/det;
  //
  //for (int i = 0; i < 3; i ++) {
  //  x[i] = x[i] - this->matrix[i][3];
  //}
  //
  //for (int i = 0; i < 3; i ++) {
  //  y[i] = inv[i][0] * x[0] + inv[i][1] * x[1] + inv[i][2] * x[2];
  //}

  Matrix vx;
  Matrix vy;
  vx.ReSize(4,1);
  vy.ReSize(4,1);

  vx.element(0, 0) = x[0];
  vx.element(1, 0) = x[1];
  vx.element(2, 0) = x[2];
  vx.element(3, 0) = 0.0;

  vy = Tpr*vx;

  y[0] = vy.element(0, 0);
  y[1] = vy.element(1, 0);
  y[2] = vy.element(2, 0);


}


// Convert target position from the robot coordinate system to the image coordinate system
void MrsvrTransform::invTransform(MrsvrVector x, MrsvrVector y) 
{
  //// Compute x = M * y
  //
  //for (int i = 0; i < 3; i ++)
  //  {
  //  y[i] = this->matrix[i][0] * x[0] + this->matrix[i][1] * x[1] + this->matrix[i][2] * x[2]+ this->matrix[i][3];
  //  }


  Matrix vx;
  Matrix vy;
  vx.ReSize(4,1);
  vy.ReSize(4,1);

  vx.element(0, 0) = x[0];
  vx.element(1, 0) = x[1];
  vx.element(2, 0) = x[2];
  vx.element(3, 0) = 0.0;

  vy = Trp*vx;

  y[0] = vy.element(0, 0);
  y[1] = vy.element(1, 0);
  y[2] = vy.element(2, 0);

}
 

