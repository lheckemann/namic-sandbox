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

#include "newmatio.h"

MrsvrTransform::MrsvrTransform()
{
  Trp.ReSize(4,4);
  Trp << 1.0 << 0.0 << 0.0 << 0.0
      << 0.0 << 1.0 << 0.0 << 0.0
      << 0.0 << 0.0 << 1.0 << 0.0
      << 0.0 << 0.0 << 0.0 << 1.0;

  Tpr.ReSize(4,4);
  Trp << 1.0 << 0.0 << 0.0 << 0.0
      << 0.0 << 1.0 << 0.0 << 0.0
      << 0.0 << 0.0 << 1.0 << 0.0
      << 0.0 << 0.0 << 0.0 << 1.0;
}



MrsvrTransform::~MrsvrTransform()
{
}


void MrsvrTransform::setCalibrationMatrix(float* m)
{
  Matrix Trz(4,4);
  Matrix Tzp(4,4);

  Trz << 1.0 << 0.0 << 0.0 << -37.5
      << 0.0 << 1.0 << 0.0 << -47.0
      << 0.0 << 0.0 << 1.0 << 30.0
      << 0.0 << 0.0 << 0.0 << 1.0;
  
  // Zframe to patient transform
  for (int i = 0; i < 4; i ++) {
    for (int j = 0; j < 4; j ++) {
      Tzp.element(i, j) = m[i*4+j];
    }
  }

  // Calculate robot to patient transform
  Trp = Tzp * Trz;

  std::cout << Trp << std::endl;
  std::cout << Tzp << std::endl;
  std::cout << Trz << std::endl;

  // Calculate patient to robot transform
  Tpr = Trp.i();

  std::cout << Tpr << std::endl;

}


// Convert target position from the patient coordinate system to the robot coordinate system
void MrsvrTransform::transform(MrsvrVector x, MrsvrVector y)
{
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
 

