/*=========================================================================

  Program:   Imaging Scanner Interface
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __TRANSFER_OPENIGTLINK_H
#define __TRANSFER_OPENIGTLINK_H

#include <string>

#include "igtlMath.h"
#include "igtlWin32Header.h"
#include "igtlImageMessage.h"
#include "TransferBase.h"

#include "igtlServerSocket.h"
#include "igtlClientSocket.h"
#include "igtlMultiThreader.h"

class IGTLCommon_EXPORT TransferOpenIGTLink : public TransferBase
{
public:
  enum {
    MODE_UNDEFINED = 0,
    MODE_SERVER,
    MODE_CLIENT,
  };
  enum {
    STATE_OFF,
    STATE_WAIT_CONNECTION,
    STATE_CONNECTED,
    NUM_STATE
  };

public:
  
  //  static TransferOpenIGTLink* New();
  //  static void Delete();
  
  virtual int  Init();
  virtual int  Connect();
  virtual int  Disconnect();
  
  void FlowControl(int s);  // Activate / deactivate flow control
  virtual int  Start();
  virtual int  Stop();
  
  void SetServer(std::string hostname, int port);
  void SetClientMode(std::string hostname, int port);
  void SetServerMode(int port);

  TransferOpenIGTLink();

protected:
  virtual ~TransferOpenIGTLink()  {};
  igtl::ClientSocket::Pointer WaitForConnection();

  virtual void Process();
  void         ReceiveController();
  static void* CallReceiveProcess(igtl::MultiThreader::ThreadInfo* vinfo);
  void         ReceiveProcess();

  int          ReceiveTransform(igtl::ClientSocket::Pointer& socket,
                                igtl::MessageHeader::Pointer& header);
  int          ReceivePosition(igtl::ClientSocket::Pointer& socket,
                               igtl::MessageHeader::Pointer& header);
  int          ReceiveStatus(igtl::ClientSocket::Pointer& socket,
                             igtl::MessageHeader::Pointer& header);
  
protected:

  int UseFlowControl; // flow control flag -- 0: don't use Flow / 1: use Flow
  int Flow;        // flow control -- 0: stop transfer / 1: do transfer

  int Mode;
  int State;
  std::string Hostname;
  int Port;

  // for TCP/IP connection

  igtl::ServerSocket::Pointer ServerSocket;
  igtl::ClientSocket::Pointer Socket;

  bool connected;
  bool ServerStopFlag;

  // for Receive Thread
  bool          StopReceiveThread;
  int           ReceiveThreadID;
  igtl::MultiThreader::Pointer ReceiveThread;
  igtl::MutexLock::Pointer Mutex;

};


#endif// __TRANSFER_OPENIGTLINK2_H


