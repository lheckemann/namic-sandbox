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
  this->Mode     = MODE_UNDEFINED;
  this->Hostname = "";
  this->Port     = -1;

  this->State    = STATE_OFF;

  this->StopReceiveThread = true;
  this->ReceiveThreadID = -1;
  this->ReceiveThread = igtl::MultiThreader::New();

  this->Mutex = igtl::MutexLock::New();

}


int TransferOpenIGTLink::Init()
{
  return 1;
}


int TransferOpenIGTLink::Connect()
{
  //this->CheckAndConnect();
  return 1;
}


int TransferOpenIGTLink::Disconnect()
{
  //mutex->acquire();  //lock
  if (this->Socket.IsNull())
    {
    //sock.close();
    this->Socket->CloseSocket();
    this->Socket->Delete();
    }
  //mutex->release();  //unlock
  return 1;

}

void TransferOpenIGTLink::SetServer(std::string hostname, int port)
{
  SetClientMode(hostname, port);
}


void TransferOpenIGTLink::SetClientMode(std::string hostname, int port)
{
  if (this->Socket.IsNull())
    {
    this->Mode     = MODE_CLIENT;
    this->Hostname = hostname;
    this->Port     = port;
    }
}

void TransferOpenIGTLink::SetServerMode(int port)
{
  if (this->Socket.IsNull())
    {
    this->Mode = MODE_SERVER;
    this->Port = port;
    }
}


igtl::ClientSocket::Pointer TransferOpenIGTLink::WaitForConnection()
{
  igtl::ClientSocket::Pointer socket = NULL;

  if (this->Mode == MODE_CLIENT)
    {
    socket = igtl::ClientSocket::New();
    }

  while (!this->ServerStopFlag)
    {
    if (this->Mode == MODE_SERVER)
      {
      std::cerr << "TransferOpenIGTLink: Waiting for client @ port #"
                << this->Port << std::endl;
      socket = this->ServerSocket->WaitForConnection(1000);
      if (socket.IsNotNull()) // if client connected
        {
        std::cerr << "TransferOpenIGTLink: connected." << std::endl;
        return socket;
        }
      }
    else if (this->Mode == MODE_CLIENT) // if this->Mode == MODE_CLIENT
      {
      std::cerr << "TransferOpenIGTLink: Connecting to server..." << std::endl;
      int r = socket->ConnectToServer(this->Hostname.c_str(), this->Port);
      if (r == 0) // if connected to server
        {
        return socket;
        }
      else
        {
        break;
        }
      }
    else
      {
      this->ServerStopFlag = true;
      }
    }

  if (socket.IsNotNull())
    {
    std::cerr << "TransferOpenIGTLink::WaitForConnection(): Socket Closed." << std::endl;
    socket->CloseSocket();
    socket->Delete();
    }

  return NULL;
}


void TransferOpenIGTLink::Process()
{
  
  this->ServerStopFlag = false;

  this->State = STATE_WAIT_CONNECTION;
  
  if (this->Mode == MODE_SERVER)
    {
    this->ServerSocket = igtl::ServerSocket::New();
    this->ServerSocket->CreateServer(this->Port);
    }
  
  // Communication -- common to both Server and Client
  while (!this->ServerStopFlag)
    {
    std::cerr << "TransferOpenIGTLink::Process(): alive." << std::endl;
    this->Mutex->Lock();
    this->Socket = this->WaitForConnection();
    this->Mutex->Unlock();
    if (this->Socket.IsNotNull())
      {
      this->State = STATE_CONNECTED;
      std::cerr << "TransferOpenIGTLink::Process(): Client Connected." << std::endl;
      this->ReceiveThreadID = this->ReceiveThread->SpawnThread((igtl::igtlThreadFunctionType)TransferOpenIGTLink::CallReceiveProcess, this);
      this->ReceiveController();
      this->State = STATE_WAIT_CONNECTION;
      }
    }

  if (this->ServerSocket.IsNotNull())
    {
    this->ServerSocket->CloseSocket();
    this->ServerSocket->Delete();
    }
  if (this->Socket.IsNotNull())
    {
    this->Socket->CloseSocket();
    this->Socket->Delete();
    this->Socket = NULL;
    }
  this->ThreadID = -1;
  this->State = STATE_OFF;
}


void TransferOpenIGTLink::ReceiveController()
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

      int ret;
      
      std::cerr << "PackSize:  " << frame->GetPackSize() << std::endl;
      std::cerr << "BodyMode:  " << frame->GetBodyType() << std::endl;
      
      ret = this->Socket->Send(frame->GetPackPointer(), frame->GetPackSize());
      if (ret == 0)
        {
        Disconnect();
        std::cerr << "Error : Connection Lost!\n";
        }
      }
    }
}


void* TransferOpenIGTLink::CallReceiveProcess(igtl::MultiThreader::ThreadInfo* vinfo)
{
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

    retval = this->Socket->Receive(&header, IGTL_HEADER_SIZE);
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
    memcpy((void*)deviceType, header.name, 12);
    if (strcmp("TRANSFORM", deviceType))
      {
      float matrix[12];
      retval = this->Socket->Receive((char*)matrix, IGTL_TRANSFORM_SIZE);
      if (retval != IGTL_TRANSFORM_SIZE)
        {
        std::cerr << "Error : receiving header data!\n";
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


