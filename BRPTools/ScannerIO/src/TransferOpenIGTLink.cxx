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

#include "TransferOpenIGTLink.h"

// for TCP/IP connection
#include <ace/ACE.h>
#include <ace/OS.h>
#include <ace/FILE.h>
#include <ace/Thread.h>
#include <ace/Synch.h>
#include <ace/Thread_Mutex.h>

#include <ace/INET_Addr.h>
#include <ace/Time_Value.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>


TransferOpenIGTLink::TransferOpenIGTLink()
{
  this->Hostname = "";
  this->Port = -1;
    
}

int TransferOpenIGTLink::Init()
{
}

int TransferOpenIGTLink::Connect()
{
  address.set(this->Port, this->Hostname.c_str());
  this->CheckAndConnect();
  /*
  stop = 0;
  thread = new ACE_thread_t;
  ACE_Thread::spawn((ACE_THR_FUNC)thread_func, 
                    NULL, 
                    THR_NEW_LWP | THR_JOINABLE,
                    (ACE_thread_t *)thread );
  */
  return 1;
}

int TransferOpenIGTLink::Disconnect()
{
  //mutex->acquire();  //lock
  if (connected)
    {
      connected = false;
      sock.close();
    }
  //mutex->release();  //unlock
}

void TransferOpenIGTLink::SetServer(std::string hostname, int port)
{
  this->Hostname = hostname;
  this->Port = port;
}

bool TransferOpenIGTLink::CheckAndConnect()
{
  if (this->Port > 0)
    {
      ACE_Time_Value timeOut(5,0);
      int ret;
      
      //mutex->acquire();  //lock
      if (!connected)
        {
          ret = connector.connect(sock, address, &timeOut);
          if(ret == -1 && errno != ETIME && errno != 0 )
            {
              ACE_DEBUG((LM_ERROR, ACE_TEXT("Error %d : Cannot connect to Slicer Daemon!\n"), errno));
            }
          else
            {
              connected = true;
            }
        }
      //mutex->release();  //unlock
      return connected;
    }

}

void TransferOpenIGTLink::Process()
{
  igtl::ImageMessage::Pointer frame;

  this->ResetTriggerCounter();
  this->EnableTrigger();
  //this->SetTriggerMode(COUNT);
  this->SetTriggerMode(COUNT_WITH_ARG);
  while (1)
    {
      int id = (int) WaitForTrigger();
      std::cerr << "TransferOpenIGTLink::Process(): Triggered." << std::endl;
      if (this->AcquisitionThread)
        {
          frame = this->AcquisitionThread->GetFrameFromBuffer(id);

          if (connected)
            {
              int ret;
              ACE_Time_Value timeOut(1,0);

              ret = sock.send_n(frame->GetPackPointer(), frame->GetPackSize(), &timeOut);
              if (ret <= 0) {
                Disconnect();
                ACE_DEBUG((LM_ERROR, ACE_TEXT("Error %d : Connection Lost!\n"), errno));
              }
            }
        } 
    }
}

