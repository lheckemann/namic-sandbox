/***************************************************************************
 * FileName      : InterfaceSlicer.h
 * Created       : 2008/2/5
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : conect other device
 *
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef INTERFACE_SLICER__H
#define INTERFACE_SLICER__H

#include <iostream>

#include "Common.h"
#include "TCPSock.h"
#include "UDPSock.h"
#include "OpenIGTLink.h"
#include "InterfaceBase.h"
#include "InterfaceDataBaseSet.h"


/// Interface class for master-slave
class
InterfaceSlicer : public InterfaceBase {
private:
  static void Write(void* ptr);
  /// Task delete flag
  static bool FlagD;
  ///
  THRD_ID tID;
  ///
  static IF_MSG_Q_ID IFMsgQID;
  TCPSock Sock;

 public:
  /// A cpnstructor
  InterfaceSlicer();

  /// A destructor
  ~InterfaceSlicer();

  /// Delete interface task
  void Delete();

};

#endif // INTERFACE_SLICER__H

