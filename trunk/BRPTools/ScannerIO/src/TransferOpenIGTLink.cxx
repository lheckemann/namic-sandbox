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
#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlStatusMessage.h"


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

  this->UseFlowControl = 0;
  this->Flow = 1;
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


void TransferOpenIGTLink::FlowControl(int s)
{
  if (s == 0)
    {
    this->UseFlowControl = 0;
    }
  else
    {
    this->UseFlowControl = 1;
    }
}

int TransferOpenIGTLink::Start()
{
  if (this->UseFlowControl)
    {
    this->Flow = 1;
    return 1;
    }
  else
    {
    return 0;
    }
}


int TransferOpenIGTLink::Stop()
{
  if (this->UseFlowControl)
    {
    this->Flow = 0;
    return 1;
    }
  else
    {
    return 0;
    }
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
      this->ReceiveThreadID = this->ReceiveThread
        ->SpawnThread((igtl::igtlThreadFunctionType)TransferOpenIGTLink::CallReceiveProcess, this);
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
      

      if (this->UseFlowControl == 0 ||
          (this->UseFlowControl && this->Flow))
        {
        ret = this->Socket->Send(frame->GetPackPointer(), frame->GetPackSize());
        if (ret == 0)
          {
          Disconnect();
          std::cerr << "Error : Connection Lost!\n";
          }
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

  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();

  while (!this->StopReceiveThread)
    {

    if (this->Socket)
      {
      headerMsg->InitPack();

      // Receive generic header from the socket
      int r = this->Socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());

      if (r != headerMsg->GetPackSize())
        {
        std::cerr << "Error: receiving data." << std::endl;
        }
      
      // Deserialize the header
      headerMsg->Unpack();

      // Check data type and receive data body
      if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
        {
        ReceiveTransform(this->Socket, headerMsg);
        }
      else if (strcmp(headerMsg->GetDeviceType(), "POSITION") == 0)
        {
        ReceivePosition(this->Socket, headerMsg);
        }
      else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
        {
        ReceiveStatus(this->Socket, headerMsg);
        }
      else
        {
        this->Socket->Skip(headerMsg->GetBodySizeToRead(), 0);
        }
      }
    else
      {
      std::cerr << "Error : Socket does not exist!" << std::endl;
      igtl::Sleep(1000);
      }
    }
}


int TransferOpenIGTLink::ReceiveTransform(igtl::ClientSocket::Pointer& socket,
                                          igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving TRANSFORM data type." << std::endl;
  
  // Create a message buffer to receive transform data
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(header);
  transMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {

    // Retrive the transform data
    igtl::Matrix4x4 matrix;
    transMsg->GetMatrix(matrix);

    if (this->AcquisitionThread)
      {
      this->AcquisitionThread->SetMatrix(matrix);
      }
  
    igtl::PrintMatrix(matrix);

    return 1;

    }


  return 0;
}


int TransferOpenIGTLink::ReceivePosition(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving POSITION data type." << std::endl;
  
  // Create a message buffer to receive transform data
  igtl::PositionMessage::Pointer positionMsg;
  positionMsg = igtl::PositionMessage::New();
  positionMsg->SetMessageHeader(header);
  positionMsg->AllocatePack();
  
  // Receive position position data from the socket
  socket->Receive(positionMsg->GetPackBodyPointer(), positionMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = positionMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the transform data
    float position[3];
    float quaternion[4];

    positionMsg->GetPosition(position);
    positionMsg->GetQuaternion(quaternion);

    if (this->AcquisitionThread)
      {
      igtl::Matrix4x4 matrix;
      igtl::QuaternionToMatrix(quaternion, matrix);
      matrix[0][3] = position[0];
      matrix[1][3] = position[1];
      matrix[2][3] = position[2];
      this->AcquisitionThread->SetMatrix(matrix);
      }

    std::cerr << "position   = (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
    std::cerr << "quaternion = (" << quaternion[0] << ", " << quaternion[1] << ", "
              << quaternion[2] << ", " << quaternion[3] << ")" << std::endl << std::endl;

    return 1;
    }

  return 0;
}



int TransferOpenIGTLink::ReceiveStatus(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving STATUS data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->SetMessageHeader(header);
  statusMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = statusMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    std::cerr << "========== STATUS ==========" << std::endl;
    std::cerr << " Code      : " << statusMsg->GetCode() << std::endl;
    std::cerr << " SubCode   : " << statusMsg->GetSubCode() << std::endl;
    std::cerr << " Error Name: " << statusMsg->GetErrorName() << std::endl;
    std::cerr << " Status    : " << statusMsg->GetStatusString() << std::endl;
    std::cerr << "============================" << std::endl;
    }

  return 0;

}
