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

#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>

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

  ACE_INET_Addr   address;
  ACE_SOCK_Stream sock;
  ACE_SOCK_Connector connector;


  bool connected;

  // for Receive Thread
  ACE_thread_t    ReceiveThread;
  ACE_hthread_t   ReceiveHthread;
  bool            StopReceiveThread;
  

};


#endif// __TRANSFER_OPENIGTLINK_H




