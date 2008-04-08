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
#include "igtlImageMessage.h"
#include "TransferBase.h"

#include "igtlServerSocket.h"
#include "igtlClientSocket.h"
#include "igtlMultiThreader.h"

class TransferOpenIGTLink : public TransferBase
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

  
protected:

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


