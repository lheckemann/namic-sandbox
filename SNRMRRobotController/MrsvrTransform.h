//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrTransform.h,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================


//====================================================================
// Description:
//    Class to convert from RAS coordinate to robot coordiante.
//====================================================================


#ifndef _INC_MRSVR_TRANSFORM
#define _INC_MRSVR_TRANSFORM

#ifdef DEBUG
#define DEBUG_MRSVR_TRANSFORM
#endif

#include <iostream>
//#include <octave/config.h>
//#include <octave/Matrix.h>

#include  "MrsvrRAS.h"

typedef float MrsvrVector[3];
typedef float MrsvrMatrix4x4[4][4];


void PrintMatrix(MrsvrMatrix4x4 &matrix);
void QuaternionToMatrix(float* q, MrsvrMatrix4x4& m);
void MatrixToQuaternion(MrsvrMatrix4x4& m, float* q);
void Cross(float *a, float *b, float *c);
void IdentityMatrix(MrsvrMatrix4x4 &matrix);


class MrsvrTransform {

 private:
  // Transformation parameters.
  // Transformation can be formulated as:
  //     y = Rx + t
  // where x is a position vector in image space, y is a position vector 
  // in robot space, R is a rotation matrix, and t is a translation vector.
  
  const static int MAX_NFDIP = 10;
  MrsvrVector     fidpRbt[MAX_NFDIP];  // fiducial point in robot cordinate 
  MrsvrVector     fidpImg[MAX_NFDIP];  // fiducial point in MRI cordinate  
  int             nFidp;               // number of fiducial points
  float           tre;                 // target registration error (TRE)

 public:
  MrsvrTransform();
  ~MrsvrTransform();

  void initFiducialPoints();
  int addFiducialPoints(MrsvrVector, MrsvrVector);
  //int addFiducialPoints(MrsvrVector, MrsvrVector, MrsvrVector);
  int calibrate();

  void setCalibrationMatrix(float* m);
  void transform(MrsvrVector, MrsvrVector);
  void invTransform(MrsvrVector, MrsvrVector);

  //float matrix[4][4];
  
  MrsvrMatrix4x4 matrix;

};

#endif //_INC_MRSVR_TRANSFORM
