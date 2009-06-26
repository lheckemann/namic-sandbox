/******************************************************************************
 * FileName      : FrameInverse.h
 * Created       : 2007/12/17
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : computed inverse kinematics
 * OS            : VxWorks 5.5.1
 *****************************************************************************/
#ifndef FRAME_INVERSE__H
#define FRAME_INVERSE__H

#include <math.h>
#include "Common.h"
#include "FrameBase.h"

/// For salve robot inverse kenematics
class FrameInverse : public FrameBase {
private:
  /// constants
  double L;
  double l1;
  double l2;
  double l3;
  double rho;
  double epsilon;

  /// valuses
  double px,py,pz;
  double sigma;
  double mu,eta,lambda,psi;
  double t1,t2,t4;
  double alpha,beta;
  double x2,y2,z2;
  double kappa1,kappa2;
  double a,b,c,d,e;
  double x1,y1,z1;

  double q[4];

  /// Initialize
  void Init();

  /// Limit angles
  void limitAngle(double* angle);

public:
  /// Get angles
  /// \param i Joint ID
  double getAngle(int i);

  /// Caluculate inverse kenematics
  /// \param P The end effector position
  /// \param JNum The number of Joint
  /// \param angle Joint anlges
  void InvKinematics( Coord_6DoF &P, int JNum, double* angle );

  /// A constructor
  FrameInverse();

  /// A destructor
  ~FrameInverse();

};


#endif // FRAME_INVERSE__H

