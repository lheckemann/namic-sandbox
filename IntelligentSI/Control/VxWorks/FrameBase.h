/*****************************************************************************
 * FileName      : FrameBase.h
 * Created       : 2008/11/2
 * LastModified  : 2008/11/
 * Author        : Hiroaki KOZUKA
 * Aim           : Joint base class for a motor control
 ****************************************************************************/
#ifndef FRAME_BASE__H
#define FRAME_BASE__H

#include <math.h>
#include <iostream>

#include "Common.h"
#include "occo.h"

//namespace occs{

/// Kinematics model
class FrameBase : public occo{
 public:
  /// A constructor
  FrameBase(){}

  /// A destructor
  virtual ~FrameBase(){}

  /// Caluculate kinematics
  /// \param JNum The number of joint
  /// \param angle Joint angles
  /// \param Pee The end effector position
  virtual void Kinematics(int JNum, double* angle,  Coord_6DoF& Pee){}

  /// Caluculate inverse kinematics
  /// \param Pee The end effector position
  /// \param JNum The number of joint
  /// \param angle Joint angles
  virtual void InvKinematics( Coord_6DoF& Pee, int JNum, double* angle ){}

  void FilterFirstOrderLag(Coord_6DoF& Pee, Coord_6DoF& Pee_1);

  void FilterMovingAvrg(Coord_6DoF& Pee);

  void MovingAvrg(int num, Coord_6DoF& data, Coord_6DoF* data_);

};

//} // End of namespace occs

#endif // FRAME_BASE__H



