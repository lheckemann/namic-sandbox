/******************************************************************************
 * FileName      : FrameInverse.h
 * Created       : 2007/12/17
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : computed inverse kinematics
 * OS            : VxWorks 5.5.1
 *****************************************************************************/
#ifndef FRAME_FORWARD__H
#define FRAME_FORWARD__H

#include "Common.h"
#include "FrameBase.h"

/// For salve robot inverse kenematics
class FrameForward : public FrameBase {
private:
  /// constants
  double L;
  double l1;
  double l2;
  double l3;
  double rho;
  double epsilon;

  /// valuses
  double q1,q2,q3,q4;
  double sigma;
  double t1,t2,t3,t4;
  double ax,ay,az,nx,ox;
  double mu,eta,lambda,psi;
  double theta1,theta2;
  double x1,y1,z1;
  double a,b,c;
  double x2,y2,z2;
  double px,py,pz;

  double q[4];

  /// Initialize
  void Init();

public:
  /// Caluculate inverse kenematics
  /// \param P The end effector position
  /// \param JNum The number of Joint
  /// \param angle Joint anlges
  void Kinematics( int JNum, double* angle, Coord_6DoF &P );

  /// A constructor
  FrameForward();

  /// A destructor
  ~FrameForward();

};


#endif // FRAME_FORWARD__H

