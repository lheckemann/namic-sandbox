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

#include "igtlClientSocket.h"
#include "igtlMultiThreader.h"

class TransferOpenIGTLink : public TransferBase
{
public:
  
  //  static TransferOpenIGTLink* New();
  //  static void Delete();
  
  virtual int  Init();
  virtual int  Connect();
  virtual int  Disconnect();
  
  void SetServer(std::string hostname, int port);

  TransferOpenIGTLink();

protected:
  virtual ~TransferOpenIGTLink()  {};
  
  virtual void Process();
  bool CheckAndConnect();

  void ReceiveProcess();
  static void* CallReceiveProcess(void*);
  
protected:
  std::string Hostname;
  int Port;

  // for TCP/IP connection

  igtl::ClientSocket::Pointer ClientSocket;

  bool connected;

  // for Receive Thread
  bool            StopReceiveThread;
  int ReceiveThreadID;
  igtl::MultiThreader::Pointer ReceiveThread;

};


#endif// __TRANSFER_OPENIGTLINK2_H


