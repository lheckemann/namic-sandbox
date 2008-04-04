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

#include "igtl_header.h"
#include "igtl_transform.h"

#include "igtlMacro.h"
#include "igtlMultiThreader.h"


TransferOpenIGTLink::TransferOpenIGTLink()
{
  this->Hostname = "";
  this->Port = -1;

  this->ReceiveThreadID = -1;
  this->ReceiveThread = igtl::MultiThreader::New();
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
              std::cerr << "Error " << errno << " : Cannot connect to Slicer Daemon!\n";
              this->ClientSocket->Delete();
            }
        }
      //mutex->release();  //unlock
      // launch receive thread
      this->ReceiveThreadID = this->ReceiveThread->SpawnThread(TransferOpenIGTLink::CallReceiveProcess, this);
      
      return connected;
    }

  return false;

}


void* TransferOpenIGTLink::CallReceiveProcess(void*ptr)
{
  igtl::MultiThreader::ThreadInfo* vinfo = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  TransferOpenIGTLink* pRT = reinterpret_cast<TransferOpenIGTLink*>(vinfo->UserData);

  pRT->StopReceiveThread = false;
  pRT->ReceiveProcess();
  return (void*) 0;
}


void TransferOpenIGTLink::ReceiveProcess()
{
  int retval;
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
              std::cerr << "Error " << errno << " : receiving header data!\n";
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

              std::cerr << "PackSize:  " << frame->GetPackSize() << std::endl;

              ret = this->ClientSocket->Send(frame->GetPackPointer(), frame->GetPackSize());
              if (ret == 0)
                {
                  Disconnect();
                  std::cerr << "Error " << errno << " : Connection Lost!\n";
                }
            }
        } 
    }
}



