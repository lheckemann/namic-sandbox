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

#ifndef __TRANSFER_OPENIGTLINK2_H
#define __TRANSFER_OPENIGTLINK2_H

#include <string>

#include "igtlMath.h"
#include "igtlImageMessage.h"
#include "TransferBase.h"

#include "igtlClientSocket.h"

#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>

class TransferOpenIGTLink2 : public TransferBase
{
public:
  
  //  static TransferOpenIGTLink2* New();
  //  static void Delete();
  
  virtual int  Init();
  virtual int  Connect();
  virtual int  Disconnect();
  
  void SetServer(std::string hostname, int port);

  TransferOpenIGTLink2();

protected:
  virtual ~TransferOpenIGTLink2()  {};
  
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
  ACE_thread_t    ReceiveThread;
  ACE_hthread_t   ReceiveHthread;
  bool            StopReceiveThread;
  

};


#endif// __TRANSFER_OPENIGTLINK2_H


