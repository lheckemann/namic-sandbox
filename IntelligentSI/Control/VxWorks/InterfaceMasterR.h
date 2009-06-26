/***************************************************************************
 * FileName      : InterfaceMasterR.h
 * Created       : 2008/2/5
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : conect other device
 *
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef INTERFACE_MASTER_R__H
#define INTERFACE_MASTER_R__H

#include <iostream>

#include "Common.h"
#include "UDPSock.h"
#include "OpenIGTLink.h"
#include "InterfaceBase.h"
#include "MessageQData.h"

/// Interface class for master-slave
class
InterfaceMasterR : public InterfaceBase {
private:
  UDPSock Sock;

  /// Task delete flag
  static bool FlagD;

  /// Read from socket
  /// \param ptr This class pointer
  static void Read(void* ptr);

  THRD_ID tID;

  /// Delete interface task
  void Delete();

  static IF_MSG_Q_ID IFMsgQID;

  static double TimeGet();

  static void FilterMovingAvrg(Coord_6DoF_N& Pee);

  static void MovingAvrg(int num, Coord_6DoF_N& data, Coord_6DoF_N* data_);

 public:
  /// A cpnstructor
  InterfaceMasterR();

  /// A destructor
  ~InterfaceMasterR();

};

#endif // INTERFACE_MASTER_R__H

