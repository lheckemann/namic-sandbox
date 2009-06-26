/***************************************************************************
 * FileName      : robot.h
 * Created       : 2007/09/14
 * LastModified  : 2007/09/1
 * Author        : Hiroaki KOZUKA
 * Aim           : Main program for a Robot Control
                   robot class
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef ROBOT__H
#define ROBOT__H

#include "Common.h"
#include "Driver.h"
#include "InterfaceManager.h"
#include "JointBase.h"
#include "FrameBase.h"
#include "occo.h"

/// Robot control main class
class Robot : public occo{
private:
  /// Joint base class pointer
  JointBase** Jt;

  /// Frame base class pointer
  FrameBase* FrmI;

  /// Frame base class pointer
  FrameBase* FrmF;

  /// Driver class
  Driver* Dr;

  /// Interface manager class pointer
  InterfaceManager IFM;

#ifdef OS__VXWORKS
  /// Semaphore
  SEM_ID timingSem;

  /// Task create timing for real time handle
  /// \param sem SEM_ID(Semaphore ID)
  static void Timing (void* sem);

  static void TimeErr(unsigned long long T);
#else
  /// Semaphore
  SEM_ID timingSem;

  /// Task create timing for real time handle
  /// \param sem SEM_ID(Semaphore ID)
  static void Timing (void* sem);
#endif

  /// Number of joint
  int JointNum;

  /// Destination end effector position
  Coord_6DoF destPee;

  /// Destination end effecotr position
  Coord_6DoF destPee_1;

  /// Origin end effector position
  Coord_6DoF destPee_0;

  /// Destination joint angles
  double* destAngle;

  /// Current joint angles
  double* curAngle;

  /// Origin joint angles
  double* Angle_0;

  /// Struct for state information
  struct StateInfo{
    EVENT Event;   ///< Event
    STATE State;   ///< State
    STATE State_1; ///< State
  };

  /// Struct for state information
  StateInfo SInfo;

  /// state transition
  /// \param S State information
  void StateTransition(StateInfo& S);

  /// Get destination end effector position
  /// \param T Time [ms]
  /// \param S State information
  /// \param destPee destination end efector position
  /// \param destPee_1 destination end efector position
  void GetDestPosition(StateInfo& S, Coord_6DoF& Pee, Coord_6DoF& Pee_1);

  /// initialize
  /// \param T Time [ms]
  /// \param S State information
  void Init( StateInfo& S);

  /// Exit
  void Exit();

  /// Robot robot control
  /// \param T Time [ms]
  /// \param S State information
  void Control(StateInfo& S);

public:
  /// A constructor
  /// \param Number of joint
  Robot(int jNum);

  /// A destructor
  ~Robot();

  /// Set Joint
  /// \param jID Joint ID
  /// \param ptr Joint class pointer
  void SetJoint(int jID, JointBase* ptr);

  /// Set Frame
  /// \param ptr Frame class pointer
  void SetFrameInv(FrameBase* ptr);

  /// Set Frame
  /// \param ptr Frame class pointer
  void SetFrameFor(FrameBase* ptr);

  /// Get driver class pointer
  /// \return Driver class pointer
  Driver* DrPtr();

  /// Get interface manager classs pointer
  /// \return Interface manager classs pointer
  InterfaceManager* IFMPtr();

  /// Robot main function
  static void RobotMain( void* ptr );

  /// Set origin angle
  /// \param jID Joint ID
  /// \param angle Joint angle
  void SetAngOrigin(int jID, double angle);

  /// Set origin end effector position
  /// \param x Coordinate x
  /// \param y Coordinate y
  /// \param z Coordinate z
  /// \param alpha Lotation alpha
  /// \param beta Lotation beta
  /// \param gamma Lotation gamma
  void SetPosOrigin(double x,double y, double z,
                    double alpha,double beta,double gamma );


  double TimeGet();
};

#endif // ROBOT__H
