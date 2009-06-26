/*****************************************************************************
 * FileName      : Joint.h
 * Created       : 2008/11/2
 * LastModified  : 2008/11/
 * Author        : Hiroaki KOZUKA
 * Aim           : Joint class for a motor control
 ****************************************************************************/
#ifndef JOINT__H
#define JOINT__H

#include <iomanip>
#include <math.h>
#include <iostream.h>

#include "Common.h"
#include "JointBase.h"

//namespace occs{

/// driver class
class Driver;
class InterfaceManager;

/// joint(position) control using PID
class Joint : public JointBase {
private:
  /// Driver class pointer
  Driver* drv;

  /// Joint ID
  int jID;

  /// Gear radio
  double SpeedRadio;

  /// Current angle
  double curAngle;

  /// Joint torque
  double Torque;

  /// PID parameter
  double Kp, Ti, Td, Ts;

  /// PID value
  double e, dy, y_, e_, yd, yd_, ie;

  /// PD controller
  /// \param dest Destination data
  /// \param cur Current data (Feedback value)
  /// \return Output data
  double PDController(double dest, double cur);

  double LowPassF_FOL(double data);
public:
  /// A constructor
  /// \param d Driver class pointer
  /// \param id Joint ID
  Joint(Driver* d,  int id);

  /// A destructor
  ~Joint();

  /// Set control parameter
  /// \param kp
  /// \param ti if ti=0 -> not use
  /// \param td
  void SetCtrlParam(double kp, double ti, double td);

  ///
  void SetConst(double);

  /// Set a joint angle
  /// \param angle A joint angle
  void SetAngle(double angle);

  /// Control angle
  /// \param destAngle Destination a joint angle
  /// \return Current a joint angle
  double CtrlAngle(double destAngle);
};

//} // End of namespace occs

#endif // JOINT__H

