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


#include "igtl_header.h"
#include "igtl_transform.h"


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
  this->CheckAndConnect();
  return 1;
}

int TransferOpenIGTLink::Disconnect()
{
  //mutex->acquire();  //lock
  if (connected)
    {
      connected = false;
      //sock.close();
      this->ClientSocket->CloseSocket();
      this->ClientSocket->Delete();
    }
  //mutex->release();  //unlock

  this->StopReceiveThread = false;

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
          
          this->ClientSocket = igtl::ClientSocket::New();
          ret = this->ClientSocket->ConnectToServer(this->Hostname.c_str(), this->Port);
          if (ret == 0)
            {
              connected = true;
            }
          else
            {
              ACE_DEBUG((LM_ERROR, ACE_TEXT("Error %d : Cannot connect to Slicer Daemon!\n"), errno));
              this->ClientSocket->Delete();
            }
        }
      //mutex->release();  //unlock
      // launch receive thread
      ACE_Thread::spawn((ACE_THR_FUNC)CallReceiveProcess, 
                        (void*)this, 
                        THR_NEW_LWP | THR_JOINABLE,
                        &ReceiveThread, &ReceiveHthread);

      return connected;
    }

  return false;

}


void* TransferOpenIGTLink::CallReceiveProcess(void*ptr)
{
  TransferOpenIGTLink* pRT = reinterpret_cast<TransferOpenIGTLink*>(ptr);
  pRT->StopReceiveThread = false;
  pRT->ReceiveProcess();
  return (void*) 0;
}


void TransferOpenIGTLink::ReceiveProcess()
{
  int retval;
  ACE_Time_Value timeOut( 10, 0 );
  unsigned int trans_bytes = 0;

  while (!this->StopReceiveThread)
    {
      // J. Tokuda 02/26/2007
      // igtl_header should be wrapped by C++ class in future.
      igtl_header header;

      retval = this->ClientSocket->Receive(&header, IGTL_HEADER_SIZE);
      if (retval != IGTL_HEADER_SIZE)
        {
          std::cerr << "Irregluar size." << std::endl;
        }

      /*
        {
          if( errno != ETIME && errno != 0 )
            {
              ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d receiving header data !\n"), errno));
              this->Disconnect();
              return;
            }
      */
          
      igtl_header_convert_byte_order(&header);
      if (header.version != IGTL_HEADER_VERSION)
        {
          std::cerr << "Unsupported OpenIGTLink version." << std::endl;
        }

      char deviceType[13];
      deviceType[12] = 0;
      memcpy((void*)deviceType, header.name, 8);
      if (strcmp("TRANSFORM", deviceType))
        {
          float matrix[12];
          retval = this->ClientSocket->Receive((char*)matrix, IGTL_TRANSFORM_SIZE);
          if (retval != IGTL_TRANSFORM_SIZE)
            {
              ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d receiving header data !\n"), errno));
              this->Disconnect();
              return;
            }

          // J. Tokuda 02/26/2008
          // CRC should be checked here...

          if (this->AcquisitionThread)
            {
              this->AcquisitionThread->SetMatrix(matrix);
            }
          
        }
        
    }
}


void TransferOpenIGTLink::Process()
{
  //igtl::ImageMessage::Pointer frame;
  igtl::MessageBase::Pointer frame;

  this->ResetTriggerCounter();
  this->EnableTrigger();
  //this->SetTriggerMode(COUNT);
  this->SetTriggerMode(COUNT_WITH_ARG);
  while (1)
    {
      int id =(long) WaitForTrigger(); 
      std::cerr << "TransferOpenIGTLink::Process(): Triggered." << std::endl;
      if (this->AcquisitionThread)
        {
          frame = this->AcquisitionThread->GetFrameFromBuffer(id);

          if (connected)
            {
              int ret;
              //ACE_Time_Value timeOut(1,0);

              std::cerr << "PackSize:  " << frame->GetPackSize() << std::endl;

              ret = this->ClientSocket->Send(frame->GetPackPointer(), frame->GetPackSize());
              if (ret == 0)
                {
                  Disconnect();
                  ACE_DEBUG((LM_ERROR, ACE_TEXT("Error %d : Connection Lost!\n"), errno));
                }
            }
        } 
    }
}



