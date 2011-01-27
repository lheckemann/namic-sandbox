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


/*
void MrsvrTransform::setTipOffset(float l)
  // haven't  fully implemented yet. 
{
  tipOffset = l;
}
*/

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


/*
int MrsvrTransform::addFiducialPoints(MrsvrVector x, MrsvrVector r, MrsvrVector dir)
{
  if (nFidp < MAX_NFDIP) {
    for (int i = 0; i < 3; i ++) {
      fidpRbt[nFidp][i] = x[i];
      fidpImg[nFidp][i] = r[i] - dir[i]*tipOffset;
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
*/


int MrsvrTransform::calibrate()
{
  //#define DEBUG_MRSVR_TRANSFORM_calibrate

//  if (nFidp < 3) {
//#ifdef DEBUG_MRSVR_TRANSFORM
//    cout << "MrsvrTransform::calibrate(): Number of iducial points is too small.\n";
//#endif    
//    return 0;
//  }
//
//  ColumnVector crImg(3);
//  ColumnVector crRbt(3);
//  int i, j;
//
//  // Compute the centroid of the fiducial points in each space.
//  Matrix fpImg(3, nFidp);
//  Matrix fpRbt(3, nFidp);
//  Matrix fpdImg(3, nFidp);
//  Matrix fpdRbt(3, nFidp);
//  for (i = 0; i < nFidp; i ++) {
//    for (j = 0; j < 3; j ++) {
//      fpImg(j, i) = fidpImg[i][j];
//      fpRbt(j, i) = fidpRbt[i][j];
//    }
//  }
//  crImg.fill(0.0);
//  crRbt.fill(0.0);
//  for (i = 0; i < nFidp; i ++) {
//    crImg += fpImg.column(i);
//    crRbt += fpRbt.column(i);
//  }
//  crImg = crImg/(double)nFidp;
//  crRbt = crRbt/(double)nFidp;
//
//  // Compute the displacement from the centroid
//  for (i = 0; i < nFidp; i ++) {
//    fpdImg.insert((ColumnVector)(fpImg.column(i) - crImg), 0, i);
//    fpdRbt.insert((ColumnVector)(fpRbt.column(i) - crRbt), 0, i);
//  }
//
//#ifdef DEBUG_MRSVR_TRANSFORM_calibrate
//  cout << "centroid  rbt" << endl << crRbt << endl;
//  cout << "centroid  img" << endl << crImg << endl;
//  cout << "displacement from centroid rbt" << endl << fpdRbt << endl;
//  cout << "displacement from centroid img" << endl << fpdImg << endl;
//#endif
//
//  // Compute the fiducial covariance matirx
//  Matrix h(3, 3);
//  h.fill(0.0);
//  for (i = 0; i < nFidp; i ++) {
//    h += fpdImg.column(i)*(fpdRbt.column(i).transpose());
//  }
//
//  // Perform singular value decomposition (SVD)
//  SVD svd(h);
//  
//  // Compute the rotation matrix
//  Matrix dg(3,3);
//  dg.fill(0.0);
//  dg(0,0) = 1.0;
//  dg(1,1) = 1.0;
//  dg(2,2) = (svd.right_singular_matrix()*svd.left_singular_matrix())
//    .determinant().value();
//
//  rotMat = svd.right_singular_matrix() * dg * 
//    (svd.left_singular_matrix().transpose());
//  invRotMat = rotMat.inverse();
//
//  // Compute offset
//  baseOffset = crRbt - rotMat*crImg;
//
//  // Compute TRE
//  ColumnVector vtre;
//  float tresqr = 0;
//  for (i = 0; i < nFidp; i ++) {
//#ifdef DEBUG_MRSVR_TRANSFORM_calibrate
//    cout << " T(x) = " << endl <<
//      (ColumnVector)(rotMat*fpImg.column(i) + baseOffset) << endl <<
//      " y    = " << endl << (ColumnVector)(fpRbt.column(i)) << endl;
//#endif
//    vtre = (rotMat*fpImg.column(i) + baseOffset - fpRbt.column(i));
//    tresqr += (vtre*vtre.transpose())(0,0);
//  }
//  tre = sqrt(tresqr / (float)nFidp);
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


