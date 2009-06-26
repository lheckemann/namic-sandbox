
#ifndef DRIVER__H
#define DRIVER__H

#include <iostream>

#include "Common.h"
#include "occo.h"

class DeviceBase;
class InterfaceManager;

/// This class manageme device and board driver transaction.
class Driver : public occo {
private:
  /// Number of joint
  int JointNum;
  Driver* DRV;
  InterfaceManager* IFM;

public:
  /// Device type
  enum DeviceType{
    ANGLE_SENSOR=0, ///< Encoder, potentiometer, and so on
    ACTUATOR,       ///< Motor
    SWITCH,         ///< Switch
    DVC_TYPE_NUM    ///< Number of device type
  };

private:
  /// Number of each device
  int DvcNum[DVC_TYPE_NUM];

  /// Transfer Joint ID to device ID
  int* J2D[DVC_TYPE_NUM];

  /// Transfer Joint ID to device tag
  int* J2DT[DVC_TYPE_NUM];

  /// Device pointer
  DeviceBase** Dvc[DVC_TYPE_NUM];

public:
  /// A constructor
  /// \param jNum Number of Joint
  Driver(int jNum);

  /// A destructor
  ~Driver();

  /// Set Device ID
  /// \param jid Joint ID
  /// \param type Decivetype
  /// \param dvcTag Decive Tag
  /// \param did Device ID
  void SetDID(int jid, DeviceType type, int dvcTag, int did);

  /// Get Device ID
  /// \param type Decivetype
  /// \param jid Joint ID
  /// \return Device ID
  int DID(DeviceType type, int jid);

  /// Get Device Tag
  /// \param type Decivetype
  /// \param jid Joint ID
  /// \return Device ID
  int DTg(DeviceType type, int jid);
  /// Write joint torque to board.
  void WriteTorque();

  /// Read joint angle from board
  void ReadAngle();

  /// Write joint angle to board
  void WriteAngle();

  /// Set each joint anlge
  /// \param jid Joint ID
  /// \param angle Joint angle
  void SetAngle(int jid, double angle);

  /// Get each joint angle
  /// \param jid Joint ID
  /// \return Joint angle
  double GetAngle(int jid);

  /// Set each joint torque
  /// \param jid Joint ID
  /// \param torque Joint Toruqe
  void SetTorque(int jid, double torque);

  /// Set number of device
  /// \param type Device type
  /// \param num Number of device
  void SetDeviceNum(DeviceType type, int num);

  /// Set device
  /// \param type Device type
  /// \param id Device ID
  /// \param ptr Device class pointer
  void SetDevice(DeviceType type, int id, DeviceBase* ptr);

  /// Set toruqe zero
  void TorqueZero();

};


#endif //DRIVER__H

