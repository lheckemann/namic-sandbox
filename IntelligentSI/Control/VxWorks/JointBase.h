/*****************************************************************************
 * FileName      : JointBase.h
 * Created       : 2008/11/2
 * LastModified  : 2008/11/
 * Author        : Hiroaki KOZUKA
 * Aim           : Joint base class for a motor control
 ****************************************************************************/
#ifndef JOINT_BASE__H
#define JOINT_BASE__H

#include <math.h>
#include <iostream.h>

#include "Common.h"
#include "InterfaceManager.h"
#include "occo.h"

//namespace occs{

/// Interface manager class
class InterfaceManager;

/// joint(position) control using PID
class JointBase : public occo{
  /// Interface manager class pointer
  InterfaceManager* IFM;

public:
  /// A constructor
  JointBase(){}

  /// A destructor
  virtual ~JointBase(){}

  /// Set control parameter
  virtual void InitParam(){}

  /// Set a joint angle
  /// \param angle A joint angle
  virtual void SetAngle(double angle){}

  /// Control angle
  /// \param destAngle Destination a joint angle
  /// \return Current a joint angle
  virtual double CtrlAngle(double destAngle){ return 0; }

  /// Set interface manager class pointer
  /// \param ptr Interface manager class pointer
  void SetIFM(InterfaceManager* ptr){
    IFM = ptr;
  }

  /// Get interface manager class pointer
  /// \return Interface manager class pointer
  InterfaceManager* GetIFM(){
    return IFM;
  }

};

//} // End of namespace occs

#endif // JOINT_BASE__H



